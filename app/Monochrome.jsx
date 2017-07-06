import React, { Component } from 'react';
import * as d3color from 'd3-color';
import { getColor, setColor } from './Utils';
import ShowColor from './ShowColor';

export default class Monochrome extends Component {
  state = {
    color: d3color.color('#f19'),
  };

  componentWillMount = () => {
    getColor().then(c => {
      if (typeof c !== 'undefined') {
        this.state.color = c;
      }
    });
  };

  handleChange = color => {
    this.state.color = color.rgb();
    setColor(this.state.color);
  };

  render = () =>
    <div className="monochrome">
      <ShowColor color={this.state.color} handleChange={this.handleChange} />
    </div>;
}
