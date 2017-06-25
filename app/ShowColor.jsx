import React, { Component } from 'react';
import reactCSS from 'reactcss';
import { getColor } from './Utils';

export default class ContractsShow extends Component {
  constructor() {
    super();

    let state = {
      color: {
        r: '241',
        g: '112',
        b: '19',
        a: '1',
      },
    };
  }

  componentWillMount() {
    this.getColor();
  }

  getColor() {
    getColor().then(color => {
      this.setState({ color });
    });
  }

  render() {
    const styles = reactCSS({
      default: {
        ledemitter: {
          width: '36px',
          height: '36px',
          borderRadius: '18px',
          background: `rgba(${this.state.color.r}, ${this.state.color.g}, ${this
            .state.color.b}, ${this.state.color.a})`,
        },
        ledframe: {
          padding: '5px',
          background: '#fff',
          borderRadius: '1px',
          boxShadow: '0 0 0 1px rgba(0,0,0,.1)',
          display: 'inline-block',
          cursor: 'pointer',
        },
        popover: {
          position: 'absolute',
          zIndex: '2',
        },
        cover: {
          position: 'fixed',
          top: '0px',
          right: '0px',
          bottom: '0px',
          left: '0px',
        },
      },
    });

    return (
      <div>
        <div style={styles.ledframe}>
          <div style={styles.ledemitter} />
        </div>
      </div>
    );
  }
}
