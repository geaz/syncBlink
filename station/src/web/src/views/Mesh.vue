<template>
    <div id="mesh" class="flex-content flex-centered">
        <Loader v-if="loading"></Loader>
        <template v-else>
            <Updater :clientId="updaterClientId" :label="updaterLabel" :show="showUpdater" @close="showUpdater=false"></Updater>
            <MeshNode :nodeInfo="mesh" :isStation="true" @rename="rename" @upload="upload"></MeshNode>
        </template>
    </div>
</template>

<script lang="ts">
    import { Component, Vue } from "vue-property-decorator";
    import Loader from "../components/Loader.vue";
    import Updater from "../components/Updater.vue";
    import MeshNode from "../components/MeshNode.vue";

    @Component({
        components: {
            Loader,
            Updater,
            MeshNode
        },
    })
    export default class Mesh extends Vue {
        private mesh: any = undefined;
        private loading: boolean = true;

        private showUpdater: boolean = false;
        private updaterClientId: number = 0;
        private updaterLabel: string = "";

        async beforeMount() {
            this.reload();
        }

        async reload() : Promise<void> {
            try {
                this.loading = true;
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

        rename(clientId: number, label: string)
        {
            console.log(clientId + " " + label);
        }

        upload(clientId: number, label: string, isStation: boolean)
        {
            this.updaterClientId = clientId;
            this.updaterLabel = label;
            this.showUpdater = true;
        }
    }
</script>

<style lang="scss">
    @import "./assets/sass/global.scss";
</style>