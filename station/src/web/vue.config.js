module.exports = {
    productionSourceMap: false,
    outputDir: "../firmware/data/public",
    chainWebpack: config => {
        config
            .plugin('html')
            .tap(args => {
                args[0].title = 'syncBlink'
                return args
            })
    },
    devServer: {
        proxy: {
            '/api': {
                target: 'http://192.168.178.57',
                ws: true,
                changeOrigin: true
            }
        }
    }
}