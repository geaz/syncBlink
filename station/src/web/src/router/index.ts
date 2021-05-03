import Vue from "vue"
import VueRouter, { RouteConfig } from "vue-router"
import Scripts from "../views/Scripts.vue"
import Mesh from "../views/Mesh.vue"
import Settings from "../views/Settings.vue"

Vue.use(VueRouter);

const routes: Array<RouteConfig> = [
  {
    path: "/",
    name: "Settings",
    component: Settings
  },
  {
    path: "/mesh",
    name: "Mesh",
    component: Mesh
  },
  {
    path: "/scripts",
    name: "Scripts",
    component: Scripts
  }
];

const router = new VueRouter({
  mode: "hash",
  base: process.env.BASE_URL,
  routes
});

export default router;
