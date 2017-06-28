/* eslint-env browser */

import * as d3color from 'd3-color';
import * as d3scale from 'd3-scale';
import React from 'react';
import ReactDOM from 'react-dom';
import ShowColor from './app/ShowColor';
import { getColor, setColor, setColors } from './app/Utils';
import './assets/styles.scss';

const colorToRGB16 = c => ({ r: c.r * 256, g: c.g * 256, b: c.b * 256 });

const rgb16toColor = c => d3color.rgb(c.r / 256, c.g / 256, c.b / 256);

let startColor = d3color.color('#f19');
let endColor = d3color.color('#000');
getColor().then(c => {
  if (typeof c !== 'undefined') {
    startColor = rgb16toColor(c);
    endColor = startColor;
    console.log('getColor');
    console.log(c);
    console.log(startColor);
    console.log(endColor);
  }
});

const handleStartChange = color => {
  startColor = color;
  const c16 = colorToRGB16(startColor.rgb());
  setColor(c16);
};

const handleEndChange = color => {
  endColor = color;
  const colorScale = d3scale
    .scaleLinear()
    .domain([0, 29])
    .range([startColor, endColor]);
  const colors = [];
  for (let i = 0; i < 30; i += 1) {
    colors[i] = colorToRGB16(d3color.rgb(colorScale(i)));
  }
  setColors(colors);
  console.log(colors);
};

ReactDOM.render(
  <div className="outerContainer">
    <header>
      <div className="fixed-width">
        <img alt="logo" src="/assets/images/logo.png" id="logo" width="52px" />
      </div>
    </header>
    <div className="main">
      <ShowColor color={startColor} handleChange={handleStartChange} />
      <ShowColor color={endColor} handleChange={handleEndChange} />
    </div>
  </div>,
  document.getElementById('container')
);
