module.exports = config => {
  config.set({
    frameworks: ['jasmine'],

    browsers: ['ChromeHeadless'],

    files: [
      'node_modules/babel-polyfill/dist/polyfill.js',
      'app/**/*.js',
      'app/**/*.jsx',
      {
        pattern: 'spec/SpecHelper.js',
        watched: false,
        served: true,
        included: true,
      },
      {
        pattern: 'spec/**/*Spec.js',
        watched: false,
        served: true,
        included: true,
      },
      {
        pattern: 'spec/**/*Spec.jsx',
        watched: false,
        served: true,
        included: true,
      },
    ],

    plugins: [
      'karma-chrome-launcher',
      'karma-eslint',
      'karma-jasmine',
      'karma-phantomjs-launcher',
      'karma-sourcemap-loader',
      'karma-webpack',
    ],

    preprocessors: {
      '{app,spec}/**/*.{js,jsx}': ['eslint', 'webpack', 'sourcemap'],
    },

    webpack: {
      module: {
        loaders: [
          {
            test: /(\.js$|\.jsx$)/,
            exclude: /\/node_modules\//,
            loader: 'babel-loader',
          },
        ],
      },
      resolve: { extensions: ['.js', '.jsx'] },
      externals: {
        cheerio: 'window',
        'react/addons': true,
        'react/lib/ExecutionEnvironment': true,
        'react/lib/ReactContext': true,
      },
    },
  });
};
