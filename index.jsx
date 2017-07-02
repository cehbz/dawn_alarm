/* eslint-env browser */

import * as d3color from 'd3-color';
import React from 'react';
import ReactDOM from 'react-dom';
import ShowColor from './app/ShowColor';
import { getColor, interpolate, setColor, setColors } from './app/Utils';
import './assets/styles.scss';

const rgb16toColor = c => d3color.rgb(c.r / 256, c.g / 256, c.b / 256);
const colorToRGB16 = c => d3color.rgb(c.r * 256, c.g * 256, c.b * 256);

let startColor = d3color.color('#f19');
let endColor = d3color.color('#000');

const handleStartChange = color => {
  startColor = color.rgb();
  setColor(colorToRGB16(startColor));
};

const handleEndChange = color => {
  endColor = color.rgb();
  if (
    endColor.r !== startColor.r ||
    endColor.g !== startColor.g ||
    endColor.b !== startColor.b
  ) {
    interpolate(colorToRGB16(startColor), colorToRGB16(endColor));
    return;
  }
  const colors = [];
  for (let i = 0; i < 30; i += 1) {
    colors[i] = colorToRGB16(endColor);
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
    startColor = rgb16toColor(c);
    endColor = startColor;
    render();
  }
});
