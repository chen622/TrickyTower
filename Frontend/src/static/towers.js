var Matter = require('matter-js');

var TILE_SIZE = 30;
var WIDTH = 10 * TILE_SIZE;
var HEIGHT = 30 * TILE_SIZE;
var SPEED = 3

var BLOCKS = [{
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
}];

var engine = Matter.Engine.create();
var runner = Matter.Runner.create();

var ground = Matter.Bodies.rectangle(WIDTH / 2, HEIGHT + 50, WIDTH, 100, {isStatic: true});
Matter.World.add(engine.world, [ground]);

var falling;
var control = true;

var spawn = function() {
    var block = BLOCKS[Math.floor(Math.random() * BLOCKS.length)];
    var parts = block.points.map(function(d) {
        return Matter.Bodies.rectangle(
            WIDTH / 2 + d.x * TILE_SIZE,
            d.y * TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE,
            {render: {fillStyle: block.color}});
    });
    falling = Matter.Body.create({
        density: 0.001,
        friction: 0.1,
        restitution: 0,
        parts: parts,
    });
    control = true;
    Matter.World.add(engine.world, [falling]);
};

Matter.Events.on(engine, 'collisionStart', function(event) {
    event.pairs.forEach(function(pair) {
        if (falling.parts.includes(pair.bodyA) ||
            falling.parts.includes(pair.bodyB)) {
            spawn();
        }
    });
});

var previousTimestamp;
Matter.Events.on(engine, 'beforeUpdate', function(event) {
    if (previousTimestamp && control) {
        var dt = event.timestamp - previousTimestamp;
        var dy = dt / 1000 * TILE_SIZE * SPEED;
        Matter.Body.setVelocity(falling, {x: 0, y: dy});
        Matter.Body.setPosition(falling, {
            x: falling.position.x,
            y: falling.position.y + dy,
        });
    }
    previousTimestamp = event.timestamp;
});

Matter.Events.on(engine, 'afterUpdate', function() {
    var allBodies = Matter.Composite.allBodies(engine.world);
    allBodies.forEach(function(body) {
        if (!body.isStatic && body.position.y > HEIGHT) {
            Matter.Composite.remove(engine.world, body, true);
            if (body == falling) {
                spawn();
            }
        }
    });
});

document.addEventListener('keydown', function(event) {
    if (event.key == 'ArrowLeft') {
        event.preventDefault();
        Matter.Body.setPosition(falling, {
            x: falling.position.x - TILE_SIZE / 2,
            y: falling.position.y,
        });
    } else if (event.key == 'ArrowRight') {
        event.preventDefault();
        Matter.Body.setPosition(falling, {
            x: falling.position.x + TILE_SIZE / 2,
            y: falling.position.y,
        });
    } else if (event.key == 'ArrowUp') {
        event.preventDefault();
        Matter.Body.rotate(falling, -Math.PI / 2);
    } else if (event.key == 'ArrowDown') {
        event.preventDefault();
        if (control) {
            control = false;
            Matter.Body.setVelocity(falling, {x: 0, y: falling.velocity.y * 2});
        }
    } else if (event.key == 'Escape') {
        event.preventDefault();
        Matter.Composite.clear(engine.world, true);
        spawn();
    } else if (event.key == 'p') {
        event.preventDefault();
        runner.enabled = !runner.enabled;
    }
});

document.addEventListener('DOMContentLoaded', function() {
    var render = Matter.Render.create({
        element: document.body,
        engine: engine,
        options: {
            width: WIDTH,
            height: HEIGHT,
            wireframes: false,
        },
    });
    Matter.Render.run(render);
    Matter.Runner.run(runner, engine);

    render.canvas.style.background = 'linear-gradient(90deg, transparent 50%, rgba(255,255,255,.5) 50%)';
    render.canvas.style.backgroundSize = TILE_SIZE * 2 + 'px';

    spawn();
}, 100);
