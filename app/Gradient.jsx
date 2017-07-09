import React, { Component } from 'react';
import { SketchPicker } from 'react-color';
import * as d3color from 'd3-color';
import { getColors, setGradient } from './Utils';
import LED from './LED';

const NUM_LEDS = 30;

export default class Gradient extends Component {
  state = {
    startColor: d3color.rgb('#f19'),
    displayStartColorPicker: false,
    endColor: d3color.rgb('#000'),
    displayEndColorPicker: false,
  };

  componentWillMount = () => {
    getColors().then(c => {
      if (typeof c === 'undefined') {
        return;
      }
      if (c.length !== NUM_LEDS) {
        // eslint-disable-next-line no-console
        console.log(
          'getColors expected to see array of',
          NUM_LEDS,
          'values but got ',
          c
        );
        return;
      }
      this.setState({
        startColor: d3color.rgb(c[0].r, c[0].g, c[0].b),
        endColor: d3color.rgb(
          c[NUM_LEDS - 1].r,
          c[NUM_LEDS - 1].g,
          c[NUM_LEDS - 1].b
        ),
      });
    });
  };

  handleStartClick = () => {
    this.setState({
      displayStartColorPicker: !this.state.displayStartColorPicker,
    });
  };

  handleEndClick = () => {
    this.setState({ displayEndColorPicker: !this.state.displayEndColorPicker });
  };

  handleStartClose = () => {
    this.setState({ displayStartColorPicker: false });
  };

  handleEndClose = () => {
    this.setState({ displayEndColorPicker: false });
  };

  handleStartChange = color => {
    if (typeof color === 'undefined') {
      return;
    }
    const c = color.rgb;
    this.setState({ startColor: d3color.rgb(c.r, c.g, c.b) });
    setGradient(this.state.startColor, this.state.endColor);
  };

  handleEndChange = color => {
    if (typeof color === 'undefined') {
      return;
    }
    const c = color.rgb;
    this.setState({ endColor: d3color.rgb(c.r, c.g, c.b) });
    setGradient(this.state.startColor, this.state.endColor);
  };

  render = () =>
    <div>
      <button className="start-color" onClick={this.handleStartClick}>
        <LED color={this.state.startColor} />
      </button>
      {this.state.displayStartColorPicker
        ? <div className="popover">
            <button className="cover" onClick={this.handleStartClose} />
            <div className="color-picker">
              <SketchPicker
                color={this.state.startColor}
                onChangeComplete={this.handleStartChange}
              />
            </div>
          </div>
        : null}
      <button className="end-color" onClick={this.handleEndClick}>
        <LED color={this.state.endColor} />
      </button>
      {this.state.displayEndColorPicker
        ? <div className="popover">
            <button className="cover" onClick={this.handleEndClose} />
            <div className="color-picker">
              <SketchPicker
                color={this.state.endColor}
                onChangeComplete={this.handleEndChange}
              />
            </div>
          </div>
        : null}
    </div>;
}
