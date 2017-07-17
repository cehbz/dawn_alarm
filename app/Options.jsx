import React, { Component } from 'react';
import * as d3color from 'd3-color';
import { interpolate, setColor, setColors, NUM_LEDS } from './Utils';
import { ShowColor } from './ShowColor';

export default class Options extends Component {
  state = {
    startColor: d3color.rgb('#f19'),
    endColor: d3color.rgb('#000'),
  };

  handleStartChange = color => {
    this.state.startColor = color.rgb();
    setColor(this.state.startColor);
  };

  handleEndChange = color => {
    this.state.endColor = color.rgb();
    if (
      this.state.endColor.r !== this.state.startColor.r ||
      this.state.endColor.g !== this.state.startColor.g ||
      this.state.endColor.b !== this.state.startColor.b
    ) {
      interpolate(this.state.startColor, this.state.endColor);
      return;
    }
    const colors = [];
    for (let i = 0; i < NUM_LEDS; i += 1) {
      colors[i] = this.state.endColor;
    }
    setColors(colors);
  };

  render() {
    return (
      <div>
        <ShowColor
          color={this.state.startColor}
          handleChange={this.handleStartChange}
        />
        <ShowColor
          color={this.state.endColor}
          handleChange={this.handleEndChange}
        />
      </div>
    );
  }
}
