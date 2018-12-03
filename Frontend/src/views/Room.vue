<template>
    <md-content class="md-layout md-alignment-center-center">
        <template v-if="this.$store.state.room.length!=0">
            <md-card v-if="joinRoom!=null||hostRoom!=null" class="md-primary md-layout-item md-size-60"
                     style="background-color: black">
                <md-card-header>
                    <div class="md-title">{{room.name}}</div>
                    <div class="md-subhead">{{room.amount}}/3</div>
                </md-card-header>
                <md-dialog-content>
                    俄罗斯方块多没意思，来玩局昆特牌吧！<span style="margin-left: 10px">——女术士狩猎者</span>
                </md-dialog-content>
                <md-card-actions>
                    <md-button class="md-accent" @click="begin"><b>{{hostRoom==null?"等待开始":"开始"}}</b>
                    </md-button>
                </md-card-actions>
                <md-progress-bar class="md-accent" md-mode="indeterminate" v-if="hostRoom==null"></md-progress-bar>
            </md-card>
            <div class="md-layout-item md-size-90 md-layout md-alignment-center-right">
                <md-button class="md-accent md-raised" :disabled="joinRoom!=null||hostRoom!=null"
                           @click="showDialog=true">创建房间
                </md-button>
            </div>
            <md-content class="md-layout md-gutter md-layout-item md-size-90 md-alignment-center-space-around">
                <md-card
                        :class="item.amount==3?'md-accent md-layout-item md-size-30':'md-primary md-layout-item md-size-30'"
                        md-with-hover
                        v-for="item in this.$store.state.room" :key="item.id">
                    <md-ripple>
                        <md-card-header>
                            <div class="md-title">{{item.name}}</div>
                            <div class="md-subhead">{{item.amount}}/3</div>
                        </md-card-header>

                        <md-card-content>
                            俄罗斯方块多没意思，来玩局昆特牌吧！<br/> ——女术士狩猎者
                        </md-card-content>
                        <md-card-actions>
                            <md-button :disabled="joinRoom!=null||hostRoom!=null||item.amount==3"
                                       @click="joinRoomAction(item.id)"><b>JOIN</b></md-button>
                        </md-card-actions>
                    </md-ripple>
                </md-card>
            </md-content>
        </template>
        <md-empty-state v-else md-icon="devices_other"
                        md-label="没用可用房间"
                        md-description="好像没有人玩我的游戏...不如你来做第一个吧！:)">
            <md-button class="md-accent md-raised" @click="showDialog=true">创建房间</md-button>
        </md-empty-state>

        <md-dialog :md-active.sync="showDialog">
            <md-dialog-title>创建房间</md-dialog-title>
            <md-dialog-content style="padding: 30px">
                <md-field>
                    <label>房间名称</label>
                    <md-input v-model="form.name" :disabled="sending"/>
                    <span class="md-error" v-if="error">名称不可为空</span>
                </md-field>
            </md-dialog-content>
            <md-dialog-actions>
                <md-button class="md-primary" :disabled="sending" @click="showDialog = false">取消</md-button>
                <md-button class="md-primary" :disabled="sending" @click="createRoom">创建</md-button>
            </md-dialog-actions>
            <md-progress-bar class="md-accent" md-mode="indeterminate" v-if="sending"/>
        </md-dialog>
    </md-content>
</template>

<script>
    export default {
        name: "Room",
        data: () => ({
            msg: 'Hello!',
            showDialog: false,
            sending: false,
            hostRoom: null,
            joinRoom: null,
            room: null,
            form: {
                name: '撕逼小组'
            },
            error: false
        }),
        methods: {
            createRoom: function () {
                this.sending = true
                let _this = this
                if (this.form.name == null || this.form.name == '') {
                    this.error = true
                    this.sending = false
                    return
                }
                this.$store.commit('createRoom', this.form)
                setTimeout(function () {
                    _this.sending = false
                    _this.showDialog = false
                }, 1000)
            },
            joinRoomAction: function (roomId) {
                this.$store.commit('joinRoom', roomId)
            },
            begin: function () {
                if (this.hostRoom != null && this.room.amount == 3) {
                    this.$store.commit('beginGame')
                }
            }
        },
        mounted: function () {
            this.$store.commit('initWebSocket')
        },
        computed: {
            getRoom () {
                return this.$store.state.room;
            }
        },
        watch: {
            getRoom (newVal) {
                console.log(1)
                if (newVal == -1) {
                    console.log('游戏开始')
                    this.$router.push('/game')
                }
                for (let i = 0; i < newVal.length; i++) {
                    if (newVal[i].host == this.$store.state.socketId) {
                        this.hostRoom = newVal[i]
                        this.joinRoom = null
                        this.room = newVal[i]
                        this.$store.commit('setRole', '房主')
                        return
                    } else if (newVal[i].player2 == this.$store.state.socketId) {
                        this.joinRoom = newVal[i]
                        this.hostRoom = null
                        this.room = newVal[i]
                        this.$store.commit('setRole', '2号玩家')
                        return
                    } else if (newVal[i].player3 == this.$store.state.socketId) {
                        this.joinRoom = newVal[i]
                        this.room = newVal[i]
                        this.hostRoom = null
                        this.$store.commit('setRole', '3号玩家')
                        return
                    }
                }
                console.log(2)
                this.joinRoom = null
                this.hostRoom = null
                this.room = null
            }
        }
    }
</script>

<style scoped>
    .md-card {
        margin: 10px;
    }
</style>