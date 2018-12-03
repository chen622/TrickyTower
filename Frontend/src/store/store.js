import Vue from 'vue'
import Vuex from 'vuex'
import CircularJSON from 'circular-json'

Vue.use(Vuex)

const store = new Vuex.Store({
    state: {
        jsonObject: {
            function: -1,
            room: {
                id: null,
                name: -1
            },
            bodies: {}
        },
        webSocket: null,
        log: [],
        roomAmount: 0,
        room: [],
        socketId: null,
        msg1: {},
        msg2: {},
        role: null
    },
    mutations: {
        initWebSocket (state) {
            state.webSocket = new WebSocket('ws://127.0.0.1:8008')
            state.webSocket.onopen = function () {
                store.commit('log', 'Info: WebSocket connection opened.')
            }
            state.webSocket.onclose = function () {
                store.commit('log', 'Info: WebSocket closed.')
            }
            /*
            response json:{
                function:
                type: f1t1:创建房间成功;f1t2:所有房间;f2t1:游戏开始;f3:消息转发
                data:
            }
             */
            state.webSocket.onmessage = function (message) {
                // console.log('receive:' + message.data)
                var json = JSON.parse(message.data)
                switch (json.function) {
                    case 1: {
                        if (json.type == 1) {
                            store.commit('log', '创建房间成功')
                        } else if (json.type == 2) {
                            state.socketId = json.socketId
                            state.room = json.data
                        }
                        break
                    }
                    case 2: {
                        if (json.type == 1) {
                            state.room = -1
                        }
                        break
                    }
                    case 3: {
                        if (json.type == 0) {
                            state.msg1 = json.data
                        } else if (json.type == 1) {
                            state.msg2 = json.data
                        }
                        break
                    }
                }
            }
            state.webSocket.onerror = function (msg) {
                console.log(msg)
                // store.commit('initWebSocket')
            }
        },
        log (state, msg) {
            state.log.push(msg)
        },
        sendMsg (state, message) {
            // console.log("send" + message)
            state.webSocket.send(message)
        },
        createRoom (state, form) {
            state.jsonObject.function = 1
            state.jsonObject.room.name = form.name
            store.commit('sendMsg', JSON.stringify(state.jsonObject))
        },
        joinRoom (state, roomId) {
            state.jsonObject.function = 3
            state.jsonObject.room.id = roomId
            store.commit('sendMsg', JSON.stringify(state.jsonObject))
        },
        beginGame (state) {
            state.jsonObject.function = 4
            store.commit('sendMsg', JSON.stringify(state.jsonObject))
        },
        heartbeat (state, allBodies) {
            state.jsonObject.function = 5
            state.jsonObject.bodies = allBodies
            store.commit('sendMsg', CircularJSON.stringify(state.jsonObject))
        },
        setRole (state, role) {
            state.role = role
        }
    },
    actions: {}
})

export default store