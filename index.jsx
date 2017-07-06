/* eslint-env browser */

import React from 'react';
import ReactDOM from 'react-dom';
import { HashRouter as Router, Route, Link, Switch } from 'react-router-dom';
import Monochrome from './app/Monochrome';
import Gradient from './app/Gradient';
import Fader from './app/Fader';
import Alarm from './app/Alarm';
import Options from './app/Options';
import './assets/styles.scss';

ReactDOM.render(
  <Router>
    <div className="outerContainer">
      <header>
        <div className="fixed-width">
          <div className="menu">
            <Link className="menu-item" to="/">
              <img
                alt="logo"
                src="/assets/images/logo.png"
                id="logo"
                width="52px"
              />
            </Link>
            <Link className="menu-item" to="/monochrome">Monochrome</Link>
            <Link className="menu-item" to="/gradient">Gradient</Link>
            <Link className="menu-item" to="/fader">Fader</Link>
            <Link className="menu-item" to="/alarm">Alarm</Link>
            <Link className="menu-item" to="/options">Options</Link>
          </div>
        </div>
      </header>
      <div className="main">
        <Switch>
          <Route path="/monochrome" component={Monochrome} />
          <Route path="/gradient" component={Gradient} />
          <Route path="/fader" component={Fader} />
          <Route path="/alarm" component={Alarm} />
          <Route path="/options" component={Options} />
        </Switch>
      </div>
    </div>
  </Router>,
  document.getElementById('container')
);
