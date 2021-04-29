<template>
    <div id="mesh" class="flex-content flex-centered">
        <Modal :title="'Upload Node Firmware'" v-if="showFirmwareUpload" @close="fileSelected=showFirmwareUpload=false">
            <template v-slot:content>
                <div class="file-info" v-if="fileSelected">
                    <p>{{file.name}}</p>
                    <p v-if="firmwareError" class="file-error">{{modalError}}</p>
                    <button @click="fileSelected=false" class="button">Clear</button>
                </div>
                <label v-else class="file-button">
                    <div class="button">
                        <span>Select Firmware</span>
                    </div>
                    <input type="file" @change="checkFile" accept=".bin"/>
                </label>
            </template>
            <template v-slot:footer>
                <button @click="uploadFirmware" :disabled="!fileSelected || firmwareError" class="button red" :class="{ disabled: !fileSelected || firmwareError }">Upload</button>
            </template>
        </Modal>

        <Loader v-if="loading"></Loader>
        <template v-else>
            <button @click="showFirmwareUpload = true" class="button">Upload</button>
            <MeshNode :nodeInfo="mesh"></MeshNode>
        </template>
    </div>
</template>

<script lang="ts">
    import { Component, Vue } from "vue-property-decorator";
    import Modal from "../components/Modal.vue";
    import Loader from "../components/Loader.vue";
    import MeshNode from "../components/MeshNode.vue";

    @Component({
        components: {
            Modal,
            Loader,
            MeshNode
        },
    })
    export default class Mesh extends Vue {
        private mesh: any = undefined;
        private loading: boolean = true;

        private file!: File;
        private modalError: string = "";
        private fileSelected: boolean = false;
        private firmwareError: boolean = false;
        private showFirmwareUpload: boolean = false;

        async beforeMount() {
            try {
                await this.loadMeshInfo();
                this.loading = false;
            }
            catch(ex) {
                alert(ex);
            }            
        }

        async loadMeshInfo() : Promise<void> {            
            let response = await fetch("/api/mesh/info", {
                method: "GET"
            });
            if(response.ok) {
                let root = undefined;
                let all : any = {};
                
                (await response.json()).nodes.forEach((node : any) => {
                    all[node.clientId] = node;
                });

                Object.keys(all).forEach((id : string) => {
                    let node = all[id];
                    if(node.parentId === 0) root = node;
                    else if(node.parentId in all) {
                        let parent = all[node.parentId];
                        if(!('children' in parent)) {
                            parent.children = [];
                        }
                        parent.children.push(node);
                    }
                });

                this.mesh = root;
            }
            else { throw("Error during mesh info request!"); }
        }
        
        checkFile(event : any) : void {
            this.file = event.target.files[0];
            this.fileSelected = true;
            
            if(this.file.size > 1000000) {
                this.firmwareError = true;
                this.modalError = "File to big!";
            }
            else if(this.file.type != "application/octet-stream") {
                this.firmwareError = true;
                this.modalError = "Wrong file type!";
            }
            else this.firmwareError = false;
        }

        async uploadFirmware() : Promise<void> {
            if(confirm("Are you sure?")) {
                this.loading = true;
                this.fileSelected = false;
                this.showFirmwareUpload = false;

                let formData = new FormData();
                formData.append("firmware", this.file);
                await fetch('/api/mesh/flash?size=' + this.file.size, {method: "POST", body: formData, });

                this.loading = false;
            }
        }
    }
</script>

<style lang="scss">
    @import "./assets/sass/global.scss";

    .file-info p {
        text-align: center;
        font-weight: bold;
    }

    .file-error {
        color:$color-red;
    }
</style>