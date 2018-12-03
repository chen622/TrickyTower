import Vue from 'vue'
import Router from 'vue-router'
import Home from '@/views/Home.vue'
import Room from '@/views/Room.vue'
import Game from '@/views/Game.vue'

Vue.use(Router)

export default new Router({
    routes: [
        {
            path: '/',
            name: 'home',
            component: Home
        },
        {
            path: '/room',
            name: 'room',
            component: Room
        },
        {
            path: '/game',
            name: 'game',
            component: Game
        }
    ]
})
