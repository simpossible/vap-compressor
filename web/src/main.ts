import './assets/main.css'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import { createApp } from 'vue'
import App from './App.vue'

const vue_app = createApp(App);
vue_app.use(ElementPlus);
vue_app.mount('#app');
