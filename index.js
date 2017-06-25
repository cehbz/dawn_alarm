/* eslint-env browser */

import React from 'react';
import ReactDOM from 'react-dom';
import ShowColor from './app/ShowColor';
import './assets/styles.scss';

ReactDOM.render(
  <div className="outerContainer">
    <header>
      <div className="fixed-width">
        <img src="/assets/images/logo.png" id="logo" width="52px" />
      </div>
    </header>
    <div className="main">
      <ShowColor />
    </div>
  </div>,
  document.getElementById('container')
);
