import React, { Component } from 'react';
import { SketchPicker } from 'react-color';
import tinycolor from 'tinycolor2';

import { getColor, setColor } from './Utils';

let rgb16toColor = color => {
  return tinycolor({ r: color.r >> 8, g: color.g >> 8, b: color.b >> 8 });
};

let colorToRGB16 = color => {
  return { r: color.r << 8, g: color.g << 8, b: color.b << 8 };
};

export default class ShowColor extends Component {
  state = {
    color: '#f19f',
  };

  getColor = () => {
    getColor().then(color => {
      if (typeof color != 'undefined') {
        this.setState({ color: rgb16toColor(color).toHexString() });
      }
    });
  };

  componentWillMount = () => {
    this.getColor();
  };

  handleChangeComplete = color => {
    let c = colorToRGB16(color.rgb);
    setColor(c);
    this.setState({ color: color.hex });
  };

  render() {
    return (
      <div>
        <div className="ledframe">
          <div
            className="ledemitter"
            style={{ background: this.state.color }}
          />
        </div>
        <SketchPicker
          color={this.state.color}
          onChangeComplete={this.handleChangeComplete}
        />
        <pre>
          Color: {JSON.stringify(this.state.color)}
        </pre>
      </div>
    );
  }
}
