<template>
  <div id="loader">
      <p>{{message}}</p>
      <i class="material-icons-outlined">cached</i>
  </div>
</template>

<script lang="ts">
    import { Component, Prop, Vue } from "vue-property-decorator";

    @Component
    export default class Loader extends Vue {
        @Prop({ default: 'Loading...' })
        private message!: string;
    }
</script>

<style scoped lang="scss">
    @import "./assets/sass/shared/_var.scss";

    #loader {
        height: 100%;
        width: 100%;
        position: absolute;
        background: $background-color;
        text-align: center;
        padding-top: 40vh;
        top:0;
        left:0;
        z-index:9999;
        box-sizing: border-box;

        p { text-align: center; }
    }

    i {
        -webkit-animation:spin 4s linear infinite;
        -moz-animation:spin 4s linear infinite;
        animation:spin 4s linear infinite;
    }

    @-moz-keyframes spin { 100% { -moz-transform: rotate(-360deg); } }
    @-webkit-keyframes spin { 100% { -webkit-transform: rotate(-360deg); } }
    @keyframes spin { 100% { -webkit-transform: rotate(-360deg); transform:rotate(-360deg); } }
</style>
