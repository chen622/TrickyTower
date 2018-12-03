import Vue from 'vue'
import App from './App.vue'
import axios from 'axios'
import store from './store/store'
import router from './router/router'
import VueMaterial from 'vue-material'
import 'vue-material/dist/vue-material.min.css'
import 'vue-material/dist/theme/default.css'
import CircularJSON from 'circular-json'
Vue.use(VueMaterial)
Vue.config.productionTip = false
Vue.prototype.$size = 22
Vue.prototype.$axios = axios
Vue.prototype.$CircularJSON = CircularJSON


new Vue({
    store,
    router,
    render: h => h(App)
}).$mount('#app')
