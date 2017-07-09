import { scaleLinear } from 'd3-scale';
import { rgb } from 'd3-color';
import axios from 'axios';

const BASE_URL = 'http://dawn.local';

function getColor() {
  const url = `${BASE_URL}/color`;
  return axios
    .get(url, {})
    .then(response => response.data)
    .catch(() => undefined);
}

function getColors() {
  const url = `${BASE_URL}/colors`;
  return axios
    .get(url, {})
    .then(response => response.data)
    .catch(() => undefined);
}

function setColors(colors) {
  const url = `${BASE_URL}/colors`;
  return axios
    .post(url, colors)
    .then(response => response.data)
    .catch(error => {
      /* eslint-disable no-console */
      console.log(
        `setColors: error in axios.post('${url}', ${JSON.stringify(colors)})`
      );
      console.log(error);
      /* eslint-enable no-console */
    });
}

function setColor(color) {
  const url = `${BASE_URL}/color`;
  return axios.post(url, color).then(response => response.data).catch(error => {
    /* eslint-disable no-console */
    console.log(
      `setColor: error in axios.post('${url}', ${JSON.stringify(color)})`
    );
    console.log(error);
    /* eslint-enable no-console */
  });
}

function setGradient(startColor, endColor) {
  console.log('setGradient startColor', startColor, 'endColor', endColor);
  const colorScale = scaleLinear()
    .domain([0, 29])
    .range([startColor, endColor]);
  const colors = [];
  for (let i = 0; i < 30; i += 1) {
    const c = rgb(colorScale(i));
    colors[i] = { r: c.r, g: c.g, b: c.b };
  }
  setColors(colors);
}

function interpolate(startColor, endColor) {
  const url = `${BASE_URL}/interpolate`;
  const body = {
    start: { r: startColor.r, g: startColor.g, b: startColor.b },
    end: { r: endColor.r, g: endColor.g, b: endColor.b },
  };
  return axios.post(url, body).then(response => response.data).catch(error => {
    /* eslint-disable no-console */
    console.log(
      `setColor: error in axios.post('${url}', ${JSON.stringify(body)})`
    );
    console.log(error);
    /* eslint-enable no-console */
  });
}

export {
  BASE_URL,
  getColor,
  getColors,
  setColor,
  setColors,
  setGradient,
  interpolate,
};
