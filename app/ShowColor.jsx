import React, { Component } from 'react';
import PropTypes from 'prop-types';
import { SketchPicker } from 'react-color';
import tinycolor from 'tinycolor2';

export default class ShowColor extends Component {
  static propTypes = {
    color: PropTypes.shape({
      toHexString: PropTypes.func.isRequired,
    }).isRequired,
    handleChange: PropTypes.func.isRequired,
  };

  static defaultProps = {
    color: tinycolor('#f19'),
    handleChange: () => {},
  };

  state = {
    displayColorPicker: false,
    color: this.props.color,
  };

  handleClick = () => {
    this.setState({ displayColorPicker: !this.state.displayColorPicker });
  };

  handleClose = () => {
    this.setState({ displayColorPicker: false });
  };

  handleChange = color => {
    const c = tinycolor(color.rgb);
    this.setState({ color: c });
    this.props.handleChange(c);
  };

  render() {
    return (
      <div>
        <button className="ledframe" onClick={this.handleClick}>
          <div
            className="ledemitter"
            style={{ background: this.state.color.toHexString() }}
          />
        </button>
        {this.state.displayColorPicker
          ? <div className="popover">
              <button className="cover" onClick={this.handleClose} />
              <SketchPicker
                color={this.state.color}
                onChangeComplete={this.handleChange}
              />
            </div>
          : null}
      </div>
    );
  }
}
