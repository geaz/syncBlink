<template>
    <Loader v-if="loading"></Loader>
    <div v-else>
        <Renamer :clientId="renamerClientId" :label="renamerLabel" :show="showRenamer" @close="(renamed) => { showRenamer=false; if(renamed) reload(); }"></Renamer>
        <Updater :clientId="updaterClientId" :label="updaterLabel" :show="showUpdater" @close="showUpdater=false"></Updater>
        <div id="mesh" class="flex-content flex-centered">
            <div class="flex-section">
                <h2>Mesh Settings</h2>
                <div class="form-line">
                    <label class="left-col" for="name">Active Script</label>
                    <select class="right-col" v-model="activeScript" name="name">
                        <option v-for="script in availableScripts" :key="script">{{script}}</option>
                    </select>
                    <label class="left-col" for="source">Active Source</label>
                    <select class="right-col" v-model="activeSource" name="source">
                        <option value="0">Microphone</option>
                        <option value="1">Desktop</option>
                    </select>
                </div>
                <button v-on:click="setMeshSettings()" class="button">Save Mesh Settings</button>
            </div>
            <div class="flex-section only-desktop">
                <h2>Mesh</h2>
                <MeshNode :nodeInfo="mesh" :isStation="true" @rename="rename" @upload="upload"></MeshNode>
            </div>
        </div>
    </div>
</template>

<script lang="ts">
    import { Component, Vue } from "vue-property-decorator";
    import Loader from "../components/Loader.vue";
    import Updater from "../components/Updater.vue";
    import Renamer from "../components/Renamer.vue";
    import MeshNode from "../components/MeshNode.vue";

    @Component({
        components: {
            Loader,
            Updater,
            Renamer,
            MeshNode
        },
    })
    export default class Mesh extends Vue {
        private mesh: any = undefined;
        private activeScript : string = "";
        private activeSource : number = 0;
        private availableScripts : Array<string> = new Array<string>();

        private loading: boolean = true;

        private showUpdater: boolean = false;
        private updaterClientId: number = 0;
        private updaterLabel: string = "";

        private showRenamer: boolean = false;
        private renamerClientId: number = 0;
        private renamerLabel: string = "";

        async beforeMount() {
            this.reload();
        }

        async reload() : Promise<void> {
            try {
                this.loading = true;
                await this.loadMeshInfo();
                await this.loadScriptList();
                await this.getMeshSettings();
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

        async loadScriptList() : Promise<void> {            
            let response = await fetch("/api/mods/list", {
                method: "GET"
            });
            if(response.ok) {
                this.availableScripts = (await response.json()).mods;
            }
            else { throw("Error during script list request!"); }
        }

        async getMeshSettings() : Promise<void> {            
            let response = await fetch("/api/mods/getModSettings", {
                method: "GET"
            });
            if(response.ok) {
                let scriptSettings = await response.json();
                this.activeScript = scriptSettings.name;
                this.activeSource = scriptSettings.source;
            }
            else { throw("Error during mesh settings request!"); }
        }

        async setMeshSettings() : Promise<void> {
            this.loading = true;
            let response = await fetch(`/api/mods/setModSettings?name=${this.activeScript}&source=${this.activeSource}`, {
                method: "GET"
            });
            if(response.ok && (await response.json()).saved) {
                this.loading = false;
            }
            else { alert("Error saving mesh settings!"); }
        }

        rename(clientId: number, label: string)
        {
            this.renamerClientId = clientId;
            this.renamerLabel = label;
            this.showRenamer = true;
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