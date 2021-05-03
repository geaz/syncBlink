<template>
    <div id="scripts" class="grid-container">
        <Loader v-if="loading"></Loader>
        <template v-else>
            <aside class="grid-aside">
                <div id="avail-scripts">
                    <label>Available Scripts</label>
                    <a href="#" v-on:click="addScriptVisible = !addScriptVisible"><i class="material-icons-outlined">add_box</i></a>
                </div>
                <ul>
                    <li :class="{ 'active': activeScript === script }" v-for="script in availableScripts" :key="script">
                        <a href="#" class="main-link" v-on:click="loadScript(script)">{{script}}</a>

                        <span v-if="activeScript === script && scriptContent === originalScriptContent" class="deactivated-tool"><i class="material-icons-outlined">save</i></span>
                        <a v-else-if="activeScript === script && scriptContent !== originalScriptContent" v-on:click="saveScript()" href="#" class="tool"><i class="material-icons-outlined blue">save</i></a>

                        <a href="#" v-on:click="deleteScript(script)" :class="{ 'tool': true, 'hover-hidden': true, 'visible': activeScript === script }"><i class="material-icons-outlined hover-red">delete</i></a>
                    </li>
                </ul>
                <div id="add-script" v-if="addScriptVisible">
                    <input v-focus v-on:keyup.enter="addScript()" v-model="newScriptName" type="text" placeholder="Enter Script Name"/>
                </div> 
            </aside>
            <CodeEditor class="grid-editor" v-model="scriptContent" />
        </template>
    </div>
</template>

<script lang="ts">
    import { Component, Vue, Watch } from "vue-property-decorator";
    import Loader from "../components/Loader.vue";
    import CodeEditor from "../components/CodeEditor.vue";

    @Component({
        components: {
            Loader,
            CodeEditor
        },
    })
    export default class Scripts extends Vue {
        private loading : boolean = true;
        private availableScripts : Array<string> = new Array<string>();
        private activeScript : string = "";
        private scriptContent : string = "";
        private originalScriptContent : string = "";
        private newScriptName : string = "";
        private addScriptVisible : boolean = false;

        async beforeMount() : Promise<void> {
            await this.loadScriptList();
            this.loading = false;
        }

        async loadScriptList() : Promise<void> {            
            let response = await fetch("/api/mods/list", {
                method: "GET"
            });
            if(response.ok) {
                this.availableScripts = (await response.json()).mods;
                if(this.availableScripts.length > 0 && this.activeScript === "") await this.loadScript(this.availableScripts[0]);
            }
        }

        async loadScript(scriptName : string) : Promise<void> {
            if(this.scriptContent !== this.originalScriptContent) {
                if(confirm(`Unsaved changes available in "${this.activeScript}"! Do you want to save them?`)) {
                    await this.saveScript();
                }
            }

            this.activeScript = scriptName;
            let response = await fetch(`/api/mods/get?name=${this.activeScript}`, {
                method: "GET"
            });
            if(response.ok) {
                let scriptInfo = await response.json();
                this.scriptContent = scriptInfo.content;
                this.originalScriptContent = scriptInfo.content;
            }
        }

        async addScript() : Promise<void> {
            this.loading = true;
            if(this.newScriptName != "") {
                let response = await fetch(`/api/mods/add?name=${this.newScriptName}`, {
                    method: "GET"
                });
                if(response.ok) {
                    await this.loadScript(this.newScriptName);
                    await this.loadScriptList();
                    
                    this.newScriptName = "";
                    this.addScriptVisible = false;
                    this.loading = false;
                }
                else {
                    alert("Error during script creation!");
                }                
            }
        }

        async saveScript() : Promise<void> {
            let script = {
                name: this.activeScript,
                content: this.scriptContent
            };
            let response = await fetch(`/api/mods/save`, {
                method: "POST",
                body: JSON.stringify(script)
            });
            if(response.ok) {
                this.originalScriptContent = this.scriptContent;
            }
            else {
                alert("Error during script save!");
            }
        }

        async deleteScript(scriptName : string) : Promise<void> {
            if(confirm(`Do you really want to delete "${scriptName}"?`)) {
                this.loading = true;
                let response = await fetch(`/api/mods/delete?name=${scriptName}`, {
                    method: "GET"
                });
                if(response.ok) {
                    this.activeScript = this.activeScript === scriptName ? "" : this.activeScript;
                    await this.loadScriptList();
                    this.loading = false;
                }
                else {
                    alert("Error during script deletion!");
                }
            }
        }
    }
</script>

<style scoped lang="scss">
    @import "./assets/sass/global.scss";

    .grid-aside { grid-area: aside; }
    .grid-editor { grid-area: editor; }

    .grid-container {
        display: grid;
        grid-template-rows: auto;
        grid-template-columns: auto 1fr;
        grid-template-areas:
            'aside editor';
    }

    #scripts {
        width: 100%;
        height: 100%;
        max-height: 100%;
    }

    aside {
        width: 300px;
        margin-left: -2rem;
        box-sizing: border-box;

        ul {
            margin: 0;
            padding: 0;
            list-style: none;
        }

        li {
            display:flex;
            padding: 10px 1rem 10px 3rem;

            .main-link {
                flex-grow: 1;
                display: inline-block;
                box-sizing: border-box;
                text-decoration: none;
            }
        }

        li:hover {  
            .main-link { color: $primary-color; }          
            .hover-hidden { visibility: visible; }
        }

        .tool,
        .deactivated-tool {
            padding-right: 5px;
            i { color: lighten($text-color-faded, 20%); }
            i:hover { color: $primary-color; }

            .blue { color: $color-blue; }
            .hover-red:hover { color: $color-red; }
        }

        .deactivated-tool {
            i { color: lighten($text-color-faded, 30%); }
            i:hover { color: lighten($text-color-faded, 30%); }
        }

        .active { background: darken($background-color, 20%); }

        .hover-hidden { visibility: hidden; }
        .visible { visibility: visible; }
    }

    #avail-scripts {
        display: flex;
        align-items: center;
        margin: 0 1rem 1rem 1.5rem;

        label { 
            flex-grow: 1;
            font-weight: lighter;
            font-size: 1.1rem;
            color: $text-color-faded;
        }

        a {
            padding: 5px;
        }
    }

    #add-script {
        box-sizing: border-box;
        margin-top: 1rem;
        padding: 0 1.5rem;

        input { 
            padding: 5px; 
            width: 100%;
            box-sizing: border-box;
        }
    }

    #code-editor {
        margin-right: -2rem;
        box-shadow: $box-shadow-outset;
    }
</style>