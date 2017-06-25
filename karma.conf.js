module.exports = function(config) {
  config.set({
    frameworks: ['jasmine'],

    browsers: ['PhantomJS'],

    files: [
      'node_modules/babel-polyfill/dist/polyfill.js',
      'app/**/*.js',
      'spec/SpecHelper.js',
      'spec/**/*Spec.js'
    ],

    plugins: [
      'karma-jasmine',
      'karma-phantomjs-launcher',
      'karma-sourcemap-loader',
      'karma-webpack'
    ],

    preprocessors: {
      'app/**/*.js': ['webpack', 'sourcemap'],
      'spec/**/*.js': ['webpack', 'sourcemap']
    },

    webpack: {
      module: {
        loaders: [
          {
            test: /\.js$/,
            exclude: /\/node_modules\//,
            loader: 'babel-loader',
          }
        ]
      },
      externals: {
        'cheerio': 'window',
        'react/addons': true,
        'react/lib/ExecutionEnvironment': true,
        'react/lib/ReactContext': true
      }
    }
  })
}
