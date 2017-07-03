/* eslint-env browser */

import * as d3color from 'd3-color';
import React from 'react';
import ReactDOM from 'react-dom';
import ShowColor from './app/ShowColor';
import { getColor, interpolate, setColor, setColors } from './app/Utils';
import './assets/styles.scss';

let startColor = d3color.color('#f19');
let endColor = d3color.color('#000');

const handleStartChange = color => {
  startColor = color.rgb();
  setColor(startColor);
};

const handleEndChange = color => {
  endColor = color.rgb();
  if (
    endColor.r !== startColor.r ||
    endColor.g !== startColor.g ||
    endColor.b !== startColor.b
  ) {
    interpolate(startColor, endColor);
    return;
  }
  const colors = [];
  for (let i = 0; i < 30; i += 1) {
    colors[i] = endColor;
  }
  setColors(colors);
};

const render = () => {
  ReactDOM.render(
    <div className="outerContainer">
      <header>
        <div className="fixed-width">
          <img
            alt="logo"
            src="/assets/images/logo.png"
            id="logo"
            width="52px"
          />
        </div>
      </header>
      <div className="main">
        <ShowColor color={startColor} handleChange={handleStartChange} />
        <ShowColor color={endColor} handleChange={handleEndChange} />
      </div>
    </div>,
    document.getElementById('container')
  );
};

render();

getColor().then(c => {
  if (typeof c !== 'undefined') {
    startColor = c;
    endColor = startColor;
    render();
  }
});
