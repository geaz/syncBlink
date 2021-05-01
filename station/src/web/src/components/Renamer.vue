<template>
    <div>
        <Modal :title="'Rename Node'" v-if="show" @close="$emit('close')">
            <template v-slot:content>
                <input type="text" v-model="newLabel"/>
            </template>
            <template v-slot:footer>
                <button @click="rename" :disabled="false" class="button blue" :class="{ disabled: false }">Save</button>
            </template>
        </Modal>
        <Loader v-if="loading" :message="'Renaming. Please wait...'"></Loader>
    </div>
</template>

<script lang="ts">
    import { Component, Vue, Prop, Model } from "vue-property-decorator";
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
        @Model('change', { type: Boolean }) readonly checked!: boolean

        public newLabel: string = this.label;

        async rename() : Promise<void> {
            console.log(this.newLabel);
            if(confirm("Are you sure?")) {
                this.loading = true;
                await fetch('/api/mesh/rename?targetId=' + this.clientId + '&label=' + this.newLabel, {method: "GET" });
                this.newLabel = "";
                this.$emit('close');
            }
        }
    }
</script>

<style lang="scss">
    @import "./assets/sass/global.scss";

    input {
        display: block;
        padding: 5px;
        border: 1px solid #ccc;
        margin: 0;
        width: 100%;
        box-sizing: border-box;
    }
</style>