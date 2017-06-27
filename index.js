/* eslint-env browser */

import React from 'react';
import ReactDOM from 'react-dom';
import tinycolor from 'tinycolor2';
import ShowColor from './app/ShowColor';
import { getColor, setColor, setColors } from './app/Utils';
import './assets/styles.scss';

let rgb16toColor = color => {
  return tinycolor({ r: color.r >> 8, g: color.g >> 8, b: color.b >> 8 });
};

let colorToRGB16 = color => {
  return { r: color.r << 8, g: color.g << 8, b: color.b << 8 };
};

let startColor = tinycolor('#f19');
let endColor = tinycolor('#000');
getColor().then(c => {
  if (typeof c != 'undefined') {
    startColor = endColor = tinycolor(c);
    console.log('getColor: ', startColor);
  }
});

let handleStartChange = color => {
  console.log('handleStartChange: color: ', color);
  startColor = color;
  let c16 = colorToRGB16(color.toRgb());
  console.log('handleStartChange: c16: ', c16);
  setColor(c16);
};

let handleEndChange = color => {
  console.log('handleEndChange: color: ', color);
  endColor = color;
  let c16 = colorToRGB16(color.toRgb());
  console.log('handleEndChange: c16: ', c16);
  var colorScale = d3.scale
    .linear()
    .domain([0, 29])
    .range([startColor.toHexString(), endColor.toHexString()]);
  console.log('start: ', startColor.toHexString());
  console.log('end: ', endColor.toHexString());
  let colors = [];
  for (let i = 0; i < 30; i++) {
    colors[i] = colorToRGB16(tinycolor(colorScale(i)).toRgb());
  }
  console.log('colors: ', colors);
  setColors(colors);
};

ReactDOM.render(
  <div className="outerContainer">
    <header>
      <div className="fixed-width">
        <img src="/assets/images/logo.png" id="logo" width="52px" />
      </div>
    </header>
    <div className="main">
      <ShowColor color={startColor} handleChange={handleStartChange} />
      <ShowColor color={endColor} handleChange={handleEndChange} />
    </div>
  </div>,
  document.getElementById('container')
);
