<template>
    <div id="mesh" class="flex-content flex-centered">   
        <form method='POST' action='/api/mesh/flash' enctype='multipart/form-data'><input type='file' name='firmware'><input type='submit' value='Update'></form>";
     
        <Loader v-if="loading"></Loader>
        <template v-else>
            <MeshNode :nodeInfo="mesh"></MeshNode>
        </template>
    </div>
</template>

<script lang="ts">
    import { Component, Vue } from "vue-property-decorator";
    import Loader from "../components/Loader.vue";
    import MeshNode from "../components/MeshNode.vue";

    @Component({
        components: {
            Loader,
            MeshNode
        },
    })
    export default class Mesh extends Vue {
        private loading: boolean = true;
        private mesh: any = undefined;

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
    }
</script>

<style lang="scss">
    @import "./assets/sass/global.scss";
</style>