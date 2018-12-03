<template>
    <div style="display: flex;min-width: 100%;">
        <Play :msg="'当前玩家('+this.$store.state.role+')'"/>
        <Watch :msg="this.$store.state.role=='房主'?'二号玩家':'房主'" id="1"/>
        <Watch :msg="this.$store.state.role=='3号玩家'?'二号玩家':'三号玩家'" id="2"/>
    </div>
</template>

<script>
    import Play from '@/components/Play.vue'
    import Watch from '@/components/Watch.vue'

    export default {
        name: "Game",
        components: {
            Play,
            Watch
        },
    }
</script>

<style scoped>

</style>