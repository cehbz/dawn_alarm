import React, { Component } from 'react';
import moment from 'moment';
import 'rc-time-picker/assets/index.css';
import TimePicker from 'rc-time-picker';

import { getAlarms, setAlarms } from './Utils';

export default class AlarmClock extends Component {
  static format = 'h:mm a';
  static defaultAlarmTime = moment().hour(6).minute(0);

  static stateChanged(oldState, newState) {
    if (Object.keys(oldState).length !== Object.keys(newState).length) {
      return true;
    }
    return Object.keys(oldState).reduce(
      (acc, key) => acc || oldState[key] !== newState[key],
      false
    );
  }

  state = {
    sunday: AlarmClock.defaultAlarmTime,
    monday: AlarmClock.defaultAlarmTime,
    tuesday: AlarmClock.defaultAlarmTime,
    wednesday: AlarmClock.defaultAlarmTime,
    thursday: AlarmClock.defaultAlarmTime,
    friday: AlarmClock.defaultAlarmTime,
    saturday: AlarmClock.defaultAlarmTime,
  };

  componentWillMount = () => {
    getAlarms().then(alarms => {
      const s = {};
      Object.keys(alarms).forEach(key => {
        s[key] = moment(alarms[key], 'HH:mm');
      });
      this.setState(s);
    });
  };

  componentDidUpdate = (_, prevState) => {
    if (!AlarmClock.stateChanged(prevState, this.state)) {
      return;
    }
    setAlarms(this.state);
  };

  onChange = (day, time) => {
    const s = {};
    if (time === null) {
      s[day] = '';
    } else {
      s[day] = moment(time);
    }
    this.setState(s);
  };

  render() {
    const dayAlarm = day =>
      <div className="day" key={day}>
        <div className="day-name">
          {`${day.charAt(0).toUpperCase()}${day.slice(1)}`}
        </div>
        <TimePicker
          showSecond={false}
          defaultValue={AlarmClock.defaultAlarmTime}
          value={this.state[day]}
          className="alarm"
          onChange={time => this.onChange(day, time)}
          format={AlarmClock.format}
          use12Hours
        />
      </div>;

    return (
      <div className="alarms">
        {Object.keys(this.state).map(dayAlarm)}
      </div>
    );
  }
}
