module.exports = {
    devServer: {
        proxy: 'http://127.0.1.1:9090'
    },

    css: {
      loaderOptions: {
        sass: {
          data: '@import "@/scss/settings.scss";'
        }
      }
    }
}
