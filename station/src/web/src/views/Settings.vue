<template>
    <div id="settings" class="flex-content">        
        <Loader v-if="loading"></Loader>
        <template v-else>
            <div class="flex-section">                
                <div class="flex-heading"><h2>WiFi Settings</h2><small>Connected to: {{connectedNetwork}}</small></div>
                <div class="form-line">
                    <label class="left-col" for="ssid">SSID</label>
                    <input class="right-col" v-model="ssid" length=32 />
                    <label class="left-col" for="pass">Pass</label>
                    <input class="right-col" v-model="pass" length=64 />
                </div>
                <button v-on:click="setWifiSettings()" class="button">Save WiFi Settings</button>
            </div>
        </template>
    </div>
</template>

<script lang="ts">
    import { Component, Vue } from "vue-property-decorator";
    import Loader from "../components/Loader.vue";

    @Component({
        components: {
            Loader
        },
    })
    export default class Settings extends Vue {
        private loading: boolean = true;
        private connectedNetwork: string = "NONE";

        private ssid : string = "";
        private pass : string = "";

        async beforeMount() {
            try {
                await this.getWifiSettings();
                this.loading = false;
            }
            catch(ex) {
                alert(ex);
            }            
        }

        async getWifiSettings() : Promise<void> {
            let response = await fetch("/api/wifi/get", {
                method: "GET"
            });
            if(response.ok) {
                let wifiInfo = await response.json();
                this.connectedNetwork = wifiInfo.connected ? wifiInfo.ssid : "NONE";
            }
            else { throw("Error during WiFi info request!"); }
        }

        async setWifiSettings() : Promise<void> {
            this.loading = true;
            let response = await fetch(`/api/wifi/set?ssid=${this.ssid}&pass=${this.pass}`, {
                method: "GET"
            });
            if(response.ok && (await response.json()).saved) {
                this.loading = false;
            }
            else { alert("Error saving WiFi settings!"); }
        }
    }
</script>

<style lang="scss">
    @import "./assets/sass/global.scss";
</style>