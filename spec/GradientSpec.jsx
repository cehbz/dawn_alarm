import React from 'react';
import { shallow } from 'enzyme';
import Gradient from '../app/Gradient';

describe('Gradient', () => {
  let wrapper;

  beforeEach(done => {
    wrapper = shallow(<Gradient />);
    expect(wrapper.exists()).toEqual(true);
    done();
  });

  it('displays the current start and end colors', () => {
    expect(wrapper.find('.start-color').exists()).toEqual(true);
    expect(wrapper.find('.end-color').exists()).toEqual(true);
  });

  it('displays a color picker when start color is clicked', () => {
    expect(wrapper.find('.color-picker').exists()).toEqual(false);
    wrapper.find('.start-color').simulate('click');
    expect(wrapper.find('.color-picker').exists()).toEqual(true);
  });

  it('displays a color picker when end color is clicked', () => {
    expect(wrapper.find('.color-picker').exists()).toEqual(false);
    wrapper.find('.end-color').simulate('click');
    expect(wrapper.find('.color-picker').exists()).toEqual(true);
  });

  it('hides the color picker when clicked twice', () => {
    expect(wrapper.find('.color-picker').exists()).toEqual(false);
    wrapper.find('.start-color').simulate('click');
    wrapper.find('.start-color').simulate('click');
    expect(wrapper.find('.color-picker').exists()).toEqual(false);
  });
});
