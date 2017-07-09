import React from 'react';
import { shallow } from 'enzyme';
import Monochrome from '../app/Monochrome';
import * as utils from '../app/Utils';

describe('Monochrome', () => {
  let wrapper;

  beforeEach(done => {
    wrapper = shallow(<Monochrome />);
    expect(wrapper.exists()).toEqual(true);
    done();
  });

  it('displays the current color', () => {
    expect(wrapper.find('.current-color').exists()).toEqual(true);
  });

  it('displays a color picker when clicked', () => {
    expect(wrapper.find('.color-picker').exists()).toEqual(false);
    wrapper.find('.current-color').simulate('click');
    expect(wrapper.find('.color-picker').exists()).toEqual(true);
  });

  it('hides the color picker when clicked twice', () => {
    expect(wrapper.find('.color-picker').exists()).toEqual(false);
    wrapper.find('.current-color').simulate('click');
    wrapper.find('.current-color').simulate('click');
    expect(wrapper.find('.color-picker').exists()).toEqual(false);
  });

  xit('sets led colors when they change', () => {
    const spy = spyOn(utils, 'setColor');
    wrapper.props.children[0].props.handleChange({ r: 1, g: 2, b: 3 });
    expect(spy).toHaveBeenCalled();
  });
});
