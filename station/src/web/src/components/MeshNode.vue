<template>
  <div class="mesh-node">      
      <div class="node-header">
        <span class="node-label">{{ nodeInfo.label ? nodeInfo.label : "(No Label)" }}</span>
        <div>
            <span class="node-id">ID {{nodeInfo.clientId.toString(16).toUpperCase()}}</span>
            <div class="node-info">
                <span class="node-version">v{{nodeInfo.majorVersion}}.{{nodeInfo.minorVersion}}</span> / <span class="node-leds">{{nodeInfo.ledCount}} LEDs</span>
            </div>
        </div>
      </div>
      <div class="node-buttons">
          <button v-if="isStation && nodeInfo.children && nodeInfo.children.length > 0" @click="$emit('upload', 0, 'All Nodes', true)" class="button small red">Update all nodes</button>
          <button v-if="!isStation" @click="$emit('rename', nodeInfo.clientId, nodeInfo.label)" class="button small gray">Rename</button>
          <button v-if="!isStation" @click="$emit('upload', nodeInfo.clientId, nodeInfo.label, false)" class="button small red">Update</button>
      </div>
      <div class="node-children">          
        <MeshNode v-for="child in nodeInfo.children" :key="child.clientId" 
            :nodeInfo="child" 
            :isStation="false" 
            @rename="(id, label) => $emit('rename', id, label)" 
            @upload="(id, label, isStation) => $emit('upload', id, label, isStation)">
        </MeshNode>
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
        @Prop() private isStation!: boolean;
    }
</script>

<style scoped lang="scss">
    .mesh-node {
        border-spacing: 10px;
        display: table-cell;
        text-align: center;
        border: 1px solid #ccc;
        border-radius: 5px;
        box-shadow: 0 0px 2px rgba(0,0,0,.33);
    }

    .node-header {
        padding: 10px;
        background: white;
        border-radius: 5px;
        border-bottom: 1px solid #ccc;
    }

    .node-id, .node-info {
        line-height: 0.75rem;
        font-size: 0.833em;
        color: #777777;
    }

    .node-buttons {
        display: flex;
        flex-direction: column;
        gap: 4px;
        padding: 10px;
    }

    .node-children {
        display: table;
        border-spacing: 10px;
    }
</style>
