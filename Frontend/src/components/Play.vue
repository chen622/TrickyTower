<template>
    <div style="width: 33%;text-align: center;margin: auto" :id="msg">
        <p class="md-display-2">{{msg}}</p>
    </div>
</template>

<script>
    import Matter from 'matter-js'

    export default {
        name: 'play',
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
            }]
        }),
        methods: {
            createTheWorld: function () {
                this.Engine = this.Engine.create()// create a renderer
                this.Runner = this.Runner.create()
                var ground = Matter.Bodies.rectangle(this.width / 2, this.height - 35, this.width / 2, 20, {
                    isStatic: true,
                    label: 'ground',
                    render: {fillStyle: '#8BC34A'}
                })

                Matter.World.add(this.Engine.world, [ground])// run the engine

                this.Render = Matter.Render.create({
                    element: document.getElementById(this.msg),
                    engine: this.Engine,
                    options: {
                        width: this.width,
                        height: this.height,
                        wireframes: false,
                    },
                })
                Matter.Render.run(this.Render)
                Matter.Runner.run(this.Runner, this.Engine)

                // this.Render.canvas.style.background = 'linear-gradient(90deg, transparent 50%, rgba(255,255,255,.5) 50%)'
                this.Render.canvas.style.backgroundColor = '#ccc'
                this.Render.canvas.style.backgroundSize = this.size * 2 + 'px'

                this.spawn()
            },
            evenOn: function () {
                var _this = this
                Matter.Events.on(_this.Engine, 'collisionStart', function (event) {
                    event.pairs.forEach(function (pair) {
                        if (_this.falling.parts.includes(pair.bodyA) ||
                            _this.falling.parts.includes(pair.bodyB)) {
                            _this.spawn()
                        }
                    })
                })

                Matter.Events.on(_this.Engine, 'beforeUpdate', function (event) {
                    if (_this.previousTimestamp && _this.control) {
                        var dt = event.timestamp - _this.previousTimestamp
                        var dy = dt / 1000 * _this.size * _this.speed
                        Matter.Body.setVelocity(_this.falling, {x: 0, y: dy})
                        Matter.Body.setPosition(_this.falling, {
                            x: _this.falling.position.x,
                            y: _this.falling.position.y + dy,
                        })
                    }
                    _this.previousTimestamp = event.timestamp
                })

                Matter.Events.on(_this.Engine, 'afterUpdate', function () {
                    var allBodies = Matter.Composite.allBodies(_this.Engine.world)
                    allBodies.forEach(function (body) {
                        if (!body.isStatic && body.position.y > _this.height) {
                            Matter.Composite.remove(_this.Engine.world, body, true)
                            if (body == _this.falling) {
                                _this.spawn()
                            }
                        }
                    })
                })

                document.addEventListener('keydown', function (event) {
                    if (event.key == 'ArrowLeft') {
                        event.preventDefault()
                        Matter.Body.setPosition(_this.falling, {
                            x: _this.falling.position.x - _this.size / 2,
                            y: _this.falling.position.y,
                        })
                    } else if (event.key == 'ArrowRight') {
                        event.preventDefault()
                        Matter.Body.setPosition(_this.falling, {
                            x: _this.falling.position.x + _this.size / 2,
                            y: _this.falling.position.y,
                        })
                    } else if (event.key == 'ArrowUp') {
                        event.preventDefault()
                        Matter.Body.rotate(_this.falling, -Math.PI / 2)
                    } else if (event.key == 'ArrowDown') {
                        event.preventDefault()
                        if (_this.control) {
                            _this.control = false
                            Matter.Body.setVelocity(_this.falling, {x: 0, y: _this.falling.velocity.y * 2})
                        }
                    } else if (event.key == 'Escape') {
                        event.preventDefault()
                        Matter.Composite.clear(_this.Engine.world, true)
                        _this.spawn()
                    } else if (event.key == 'p') {
                        event.preventDefault()
                        _this.Runner.enabled = !_this.Runner.enabled
                    }
                })
            },
            spawn: function () {
                let type = Math.floor(Math.random() * this.BLOCKS.length)
                var block = this.BLOCKS[type]
                let _this = this
                var parts = block.points.map(function (d) {
                    return Matter.Bodies.rectangle(
                        _this.width / 2 + d.x * _this.size,
                        d.y * _this.size,
                        _this.size,
                        _this.size,
                        {render: {fillStyle: block.color}})
                })
                this.falling = Matter.Body.create({
                    label: type + '',
                    density: 0.001,
                    friction: 0.1,
                    frictionStatic: 1,
                    restitution: 0,
                    parts: parts,
                })
                this.control = true
                Matter.World.add(this.Engine.world, [this.falling])
            },
            heartbeat: function () {
                var list = []
                Matter.Composite.allBodies(this.Engine.world).forEach(function (body) {
                    if (body.label != 'ground') {
                        let data = {}
                        data.angle = body.angle
                        data.position = body.position
                        data.label = body.label
                        data.id = body.id
                        list.push(data)
                    }
                })
                this.$store.commit('heartbeat', list)
            }

        },
        mounted: function () {
            this.size = this.$size
            this.width = document.documentElement.clientWidth / 4
            this.height = 30 * this.$size
            this.createTheWorld()
            this.evenOn()
            this.heartbeat()
            setInterval(this.heartbeat, 100);
        },
        props: [
            'msg'
        ]
    }
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>

</style>
