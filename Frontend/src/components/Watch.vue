<template>
    <div style="width: 33%;text-align: center;margin: auto" :id="msg">
        <p class="md-display-2">{{msg}}</p>
    </div>
</template>


<script>
    import Matter from 'matter-js'

    export default {
        name: "Watch",
        data: () => ({
            Engine: Matter.Engine,
            Render: Matter.Render,
            World: Matter.World,
            Bodies: Matter.Bodies,
            Runner: Matter.Runner,
            previousTimestamp: null,
            size: 30,
            width: 40 * 30,
            height: 30 * 30,
            speed: 1.5,
            falling: null,
            control: true,
            BLOCKS: [{
                label: 'I',
                color: 'cyan',
                points: [{x: -2, y: 0}, {x: -1, y: 0}, {x: 0, y: 0}, {x: 1, y: 0}],
            }, {
                label: 'J',
                color: 'blue',
                points: [{x: -2, y: 0}, {x: -1, y: 0}, {x: 0, y: 0}, {x: 0, y: 1}],
            }, {
                label: 'L',
                color: 'orange',
                points: [{x: -1, y: 0}, {x: 0, y: 0}, {x: 1, y: 0}, {x: -1, y: 1}],
            }, {
                label: 'O',
                color: 'yellow',
                points: [{x: -1, y: 0}, {x: 0, y: 0}, {x: -1, y: 1}, {x: 0, y: 1}],
            }, {
                label: 'S',
                color: 'green',
                points: [{x: 0, y: 0}, {x: 1, y: 0}, {x: -1, y: 1}, {x: 0, y: 1}],
            }, {
                label: 'T',
                color: 'purple',
                points: [{x: -1, y: 0}, {x: 0, y: 0}, {x: 1, y: 0}, {x: 0, y: 1}],
            }, {
                label: 'Z',
                color: 'red',
                points: [{x: -1, y: 0}, {x: 0, y: 0}, {x: 0, y: 1}, {x: 1, y: 1}],
            }],
            newVals: []
        }),
        methods: {
            createTheWorld: function () {
                this.Engine = this.Engine.create();// create a renderer
                this.Runner = this.Runner.create();
                var ground = Matter.Bodies.rectangle(this.width / 2, this.height - 35, this.width / 2, 20, {
                    isStatic: true,
                    label: 'ground',
                    render: {fillStyle: '#8BC34A'}
                });

                Matter.World.add(this.Engine.world, [ground]);// run the engine

                this.Render = Matter.Render.create({
                    element: document.getElementById(this.msg),
                    engine: this.Engine,
                    options: {
                        width: this.width,
                        height: this.height,
                        wireframes: false,
                    },
                });
                Matter.Render.run(this.Render);
                Matter.Runner.run(this.Runner, this.Engine);

                // this.Render.canvas.style.background = 'linear-gradient(90deg, transparent 50%, rgba(255,255,255,.5) 50%)'
                this.Render.canvas.style.backgroundColor = '#ccc'
                this.Render.canvas.style.backgroundSize = this.size * 2 + 'px'

                this.spawn();
            },
            evenOn: function () {
                var _this = this
                // Matter.Events.on(_this.Engine, 'collisionStart', function (event) {
                //     event.pairs.forEach(function (pair) {
                //         if (_this.falling.parts.includes(pair.bodyA) ||
                //             _this.falling.parts.includes(pair.bodyB)) {
                //             _this.spawn();
                //         }
                //     });
                // });

                Matter.Events.on(_this.Engine, 'beforeUpdate', function () {
                    var allBodies = Matter.Composite.allBodies(_this.Engine.world);
                    allBodies.forEach(function (oldVal) {
                        let isInNewVal = false
                        for (let i = 0; i < _this.newVals.length; i++) {
                            if (_this.newVals[i].id == oldVal.id) {
                                isInNewVal = true
                                break
                            }
                        }
                        if (!isInNewVal && oldVal.label != 'ground') {
                            Matter.Composite.remove(_this.Engine.world, oldVal, true)
                        }
                    })

                });

                Matter.Events.on(_this.Engine, 'afterUpdate', function () {
                    var allBodies = Matter.Composite.allBodies(_this.Engine.world);
                    _this.newVals.forEach(function (newVal) {
                        let isInOldVal = false
                        for (let i = 0; i < allBodies.length; i++) {
                            if (allBodies[i].id == newVal.id) {
                                isInOldVal = true
                                // Matter.Sleeping.set(allBodies[i],false)
                                Matter.Body.setPosition(allBodies[i], {x: newVal.position.x, y: newVal.position.y})
                                Matter.Body.setAngle(allBodies[i], newVal.angle)
                                // Matter.Sleeping.set(allBodies[i],true)
                                break
                            }
                        }
                        if (!isInOldVal) {
                            var block = _this.BLOCKS[new Number(newVal.label)];
                            var parts = block.points.map(function (d) {
                                return Matter.Bodies.rectangle(
                                    _this.width / 2 + d.x * _this.size,
                                    d.y * _this.size,
                                    _this.size,
                                    _this.size,
                                    {render: {fillStyle: block.color}});
                            });
                            _this.falling = Matter.Body.create({
                                // isSleeping: true,
                                id: newVal.id,
                                density: 0.001,
                                friction: 0.1,
                                restitution: 0,
                                parts: parts,
                            });
                            _this.control = true;
                            Matter.World.add(_this.Engine.world, [_this.falling]);
                        }
                    })
                });
            },
            spawn: function () {
                // var block = this.BLOCKS[Math.floor(Math.random() * this.BLOCKS.length)];
                // let _this = this
                // var parts = block.points.map(function (d) {
                //     return Matter.Bodies.rectangle(
                //         _this.width / 2 + d.x * _this.size,
                //         d.y * _this.size,
                //         _this.size,
                //         _this.size,
                //         {render: {fillStyle: block.color}});
                // });
                // this.falling = Matter.Body.create({
                //     density: 0.001,
                //     friction: 0.1,
                //     restitution: 0,
                //     parts: parts,
                // });
                // this.control = true;
                // Matter.World.add(this.Engine.world, [this.falling]);
            }
        },
        mounted: function () {
            this.size = this.$size
            this.width = document.documentElement.clientWidth / 4
            this.height = 30 * this.$size
            this.createTheWorld()
            this.evenOn()
        },
        computed: {
            getMsg () {
                if (this.id == '1')
                    return this.$store.state.msg1
                else
                    return this.$store.state.msg2
            }
        },
        watch: {
            getMsg (newVals) {
                this.newVals = newVals
            }
        },
        props: [
            'msg', 'id'
        ]
    }
</script>

<style scoped>

</style>