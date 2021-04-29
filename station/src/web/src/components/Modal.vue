<template>
    <transition name="modal">
        <div class="modal-mask">
            <div class="modal-wrapper">
                <div class="modal-container">
                    <div class="modal-header">
                        <h3>{{title}}</h3>
                    </div>
                    <div class="modal-content">
                        <slot name="content"></slot>
                    </div>
                    <div class="modal-footer">
                        <slot name="footer"></slot>
                        <button @click="$emit('close')" class="button green">Close</button>
                    </div>
                </div>
            </div>
        </div>
    </transition>
</template>

<script lang="ts">
    import { Component, Prop, Vue } from "vue-property-decorator";

    @Component
    export default class Modal extends Vue {
        @Prop({ default: 'Dialog' })
        private title!: string;
    }
</script>

<style scoped lang="scss">
    @import "./assets/sass/global.scss";

    .modal-mask {
        position: fixed;
        z-index: 9998;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background-color: rgba(0, 0, 0, 0.5);
        display: table;
        transition: opacity 0.3s ease;
    }

    .modal-wrapper {
        display: table-cell;
        vertical-align: middle;
    }

    .modal-container {
        width: 300px;
        margin: 0px auto;
        padding: 20px 30px;
        background-color: #fff;
        border-radius: 5px;
        box-shadow: 0 2px 8px rgba(0, 0, 0, 0.33);
        transition: all 0.3s ease;
    }

    .modal-header h3 {
        margin-bottom: 1rem;
    }

    .modal-content {
        margin: 10px 0;
    }

    .modal-footer {
        display: flex;
        gap: 10px;
    }

    .modal-enter {
        opacity: 0;
    }

    .modal-leave-active {
        opacity: 0;
    }

    .modal-enter .modal-container,
    .modal-leave-active .modal-container {
        -webkit-transform: scale(1.1);
        transform: scale(1.1);
    }

</style>
