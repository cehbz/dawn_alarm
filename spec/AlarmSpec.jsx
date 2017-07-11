import React from 'react';
import { shallow } from 'enzyme';
import Alarm from '../app/Alarm';

describe('Alarm', () => {
  let wrapper;

  beforeEach(done => {
    wrapper = shallow(<Alarm />);
    expect(wrapper.exists()).toEqual(true);
    done();
  });

  it('displays the current alarm times', () => {
    expect(wrapper.find('.sunday .alarm').exists()).toEqual(true);
    expect(wrapper.find('.monday .alarm').exists()).toEqual(true);
    expect(wrapper.find('.tuesday .alarm').exists()).toEqual(true);
    expect(wrapper.find('.wednesday .alarm').exists()).toEqual(true);
    expect(wrapper.find('.thursday .alarm').exists()).toEqual(true);
    expect(wrapper.find('.friday .alarm').exists()).toEqual(true);
    expect(wrapper.find('.saturday .alarm').exists()).toEqual(true);
  });
});
