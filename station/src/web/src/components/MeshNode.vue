<template>
  <div class="mesh-node">      
      <div class="node-label">Node {{nodeInfo.clientId.toString(16)}}<small>v{{nodeInfo.firmwareVersion}}</small></div>
      <div class="node-leds">{{nodeInfo.ledCount}} LEDs</div>
      <div class="node-children">          
        <MeshNode v-for="child in nodeInfo.children" :key="child.clientId" :nodeInfo="child"></MeshNode>
      </div>
  </div>
</template>

<script lang="ts">
    import { Component, Prop, Vue } from "vue-property-decorator";

    @Component({
        name: 'MeshNode',
        components: { MeshNode }
    })
    export default class MeshNode extends Vue {
        @Prop() private nodeInfo!: any;
    }
</script>

<style scoped lang="scss">
    .mesh-node {
        border-spacing: 10px;
        display: table-cell;
        text-align: center;
        border: 1px solid #ccc;
        border-radius: 5px;
        padding: 10px;
    }

    .node-label {
        margin-right: 5px;
    }

    .node-children {
        display: table;
        border-spacing: 10px;
    }
</style>
