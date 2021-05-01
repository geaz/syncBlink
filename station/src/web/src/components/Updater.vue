<template>
    <div>
        <Modal :title="'Upload Firmware'" v-if="show" @close="() => { $emit('close'); fileSelected=false; }">
            <template v-slot:content>
                <div class="node-info"><small>Select update for</small><p>{{ label ? label : clientId.toString(16).toUpperCase() }}</p></div>
                <div class="file-info" v-if="fileSelected">
                    <small>Selected file</small>
                    <p>{{file.name}}</p>
                    <p v-if="firmwareError" class="file-error">{{errorMessage}}</p>
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
        <Loader v-if="loading" :message="'Firmware uploading, please check the status on the station display...'"></Loader>
    </div>
</template>

<script lang="ts">
    import { Component, Vue, Prop } from "vue-property-decorator";
    import Modal from "../components/Modal.vue";
    import Loader from "../components/Loader.vue";

    @Component({
        components: {
            Modal,
            Loader
        },
    })
    export default class Updater extends Vue {
        private loading: boolean = false;
        @Prop() private show!: boolean;
        @Prop() private clientId!: number;
        @Prop() private label!: string;

        private file!: File;
        private errorMessage: string = "";
        private fileSelected: boolean = false;
        private firmwareError: boolean = false;

        checkFile(event : any) : void {
            this.file = event.target.files[0];
            this.fileSelected = true;
            
            if(this.file.size > 1000000) {
                this.firmwareError = true;
                this.errorMessage = "File to big!";
            }
            else if(this.file.type != "application/octet-stream") {
                this.firmwareError = true;
                this.errorMessage = "Wrong file type!";
            }
            else this.firmwareError = false;
        }

        async uploadFirmware() : Promise<void> {
            if(confirm("Are you sure?")) {
                this.loading = true;
                this.fileSelected = false;

                let formData = new FormData();
                formData.append("firmware", this.file);
                await fetch('/api/mesh/flash?targetId=' + this.clientId + '&size=' + this.file.size, {method: "POST", body: formData, });
                
                this.$emit('close');
                this.loading = false;
            }
        }
    }
</script>

<style lang="scss">
    @import "./assets/sass/global.scss";

    .node-info, .file-info {
        margin-bottom: 15px;
        text-align: center;

        p{
            margin: 0;
            line-height: 1rem;
            text-align: center;
            font-weight: bold;
        }

        button {
            margin-top: 10px;
        }
    }

    .file-error {
        color:$color-red;
        margin-top: 10px !important;
    }
</style>