/* eslint-env browser */

// import d3 from 'd3-scale';
import React from 'react';
import ReactDOM from 'react-dom';
import tinycolor from 'tinycolor2';
import ShowColor from './app/ShowColor';
import { getColor, setColor, setColors } from './app/Utils';
import './assets/styles.scss';

const colorToRGB16 = color => ({
  r: color.r * 256,
  g: color.g * 256,
  b: color.b * 256,
});

let startColor = tinycolor('#f19');
let endColor = tinycolor('#000');
getColor().then(c => {
  if (typeof c !== 'undefined') {
    startColor = tinycolor(c);
    endColor = startColor;
  }
});

const handleStartChange = color => {
  startColor = color;
  const c16 = colorToRGB16(color.toRgb());
  setColor(c16);
};

const handleEndChange = color => {
  endColor = color;
  const colorScale = d3.scale
    .linear()
    .domain([0, 29])
    .range([startColor.toHexString(), endColor.toHexString()]);
  const colors = [];
  for (let i = 0; i < 30; i += 1) {
    colors[i] = colorToRGB16(tinycolor(colorScale(i)).toRgb());
  }
  setColors(colors);
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
