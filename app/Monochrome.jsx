import * as d3color from 'd3-color';
import React, { Component } from 'react';
import { SketchPicker } from 'react-color';

import LED from './LED';
import { getColor, setColor } from './Utils';

export default class Monochrome extends Component {
  static propTypes = {};

  state = {
    color: d3color.rgb('#f19'),
    displayColorPicker: false,
  };

  componentWillMount = () => {
    getColor().then(c => {
      if (typeof c !== 'undefined') {
        this.setState({ color: d3color.rgb(c.r, c.g, c.b) });
      }
    });
  };

  handleClick = () => {
    this.setState({ displayColorPicker: !this.state.displayColorPicker });
  };

  handleClose = () => {
    this.setState({ displayColorPicker: false });
  };

  handleChange = color => {
    const c = color.rgb;
    this.setState({ color: d3color.rgb(c.r, c.g, c.b) });
    setColor(this.state.color);
  };

  render = () =>
    <div className="monochrome">
      <button className="current-color" onClick={this.handleClick}>
        <LED color={this.state.color} />
      </button>
      {this.state.displayColorPicker
        ? <div className="popover">
            <button className="cover" onClick={this.handleClose} />
            <div className="color-picker">
              <SketchPicker
                color={this.state.color}
                onChangeComplete={this.handleChange}
              />
            </div>
          </div>
        : null}
    </div>;
}
