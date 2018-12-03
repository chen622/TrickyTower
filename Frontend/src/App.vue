<template>
  <md-content id="app" class="md-scrollbar">
    <router-view/>
    <!--<div style="display: flex;min-width: 100%;">-->
    <!--<Play msg="player1"/>-->
    <!--<Watch msg="player2"/>-->
    <!--<Watch msg="player3"/>-->
    <!--</div>-->
  </md-content>
</template>

<script>



    export default {
        name: 'app',
        methods: {},
        mounted: function () {
            document.getElementsByTagName('html')[0].classList.add('md-scrollbar')
        }
    }
</script>
<style>
  @import "https://fonts.googleapis.com/css?family=Roboto:300,400,500,700,400italic|Material+Icons";
  #app {
    width: 100%;
    min-height: 100%;
    /*font-family: Roboto,'Avenir', Helvetica, Arial, sans-serif;*/
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    color: #2c3e50;
    padding-top: 60px;
    max-height: 100%;
    overflow: auto;
  }

  body {
    background-color: #fff;
    width: 100%;
    max-height: 100%;
    line-height: 1.5;
    color: #333;
    padding: 0 0
  }

  h1, h2, h3 {
    line-height: 1.2
  }

  html {
    background-color: #fff;
    max-height: 100%;
    min-height: 100%;
  }
</style>
