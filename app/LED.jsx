import React, { Component } from 'react';
import * as d3color from 'd3-color';

const colorProp = {
  isRequired: (props, propName, componentName) => {
    const v = props[propName];
    if (v == null) {
      return new Error(
        `The property ${propName} is required in ${componentName} but its value is ${v}`
      );
    }
    if (!(v instanceof d3color.color)) {
      return new Error(
        `the property ${propName} in ${componentName}  must be a d3-color but it is ${v}`
      );
    }
    return null;
  },
};

export default class LED extends Component {
  static propTypes = {
    color: colorProp.isRequired,
  };

  state = {
    color: this.props.color,
  };

  componentWillReceiveProps = nextProps => {
    if (nextProps.color === this.props.color) return;
    const c = d3color.rgb(nextProps.color); // TODO let this just be a color, not require rgb
    this.setState({ color: c });
  };

  render() {
    return (
      <div className="led">
        <div className="led-emitter" style={{ background: this.state.color }} />
      </div>
    );
  }
}
