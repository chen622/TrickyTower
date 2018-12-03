<template>
    <md-content class="md-layout md-alignment-center-center">
        <md-content class="md-layout-item md-size-100" style="text-align: center">
            <p class=" md-display-4" style="color: #448aff">Tricky Towers</p>
            <p class=" md-headline">This is a simple <a href="http://trickytowers.com/">tricky towers</a> clone
                using <a
                        href="http://brm.io/matter-js/">matter.js</a>.
                If you do not know the game: It is basically tetris with physics.</p>
            <span class=" md-display-2">Controls</span>
        </md-content>
        <div style="margin: 0 auto">
            <ul>
                <li class="md-subheading">Left/Right/Up/Down - control falling block</li>
                <li class="md-subheading">P - pause</li>
                <li class="md-subheading">Esc - Restart</li>
            </ul>
        </div>
        <div class="md-layout-item md-size-100 md-layout md-alignment-center-center">
            <md-button to="/room" class="md-accent md-layout-item md-size-30 md-medium-size-33 md-small-size-50 md-xsmall-size-66">TRY IT</md-button>
        </div>
    </md-content>
</template>

<script>
    // @ is an alias to /src

    export default {
        name: 'home',
    }
</script>
