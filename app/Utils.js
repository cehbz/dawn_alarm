import { scaleLinear } from 'd3-scale';
import axios from 'axios';

const BASE_URL = 'http://dawn.local';

function getColor() {
  const url = `${BASE_URL}/color`;
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
        `setColor: error in axios.post('${url}', ${JSON.stringify(colors)})`
      );
      console.log(error);
      /* eslint-enable no-console */
    });
}

function setColor(color) {
  const colors = [];
  for (let i = 0; i < 30; i += 1) {
    colors[i] = color;
  }
  setColors(colors);
}

const colorToRGB16 = c => ({ r: c.r * 256, g: c.g * 256, b: c.b * 256 });
const RGB16 = c => ({
  r: Math.round(c.r),
  g: Math.round(c.g),
  b: Math.round(c.b),
});

function setGradient(startColor, endColor) {
  const colorScale = scaleLinear()
    .domain([0, 29])
    .range([colorToRGB16(startColor), colorToRGB16(endColor)]);
  const colors = [];
  for (let i = 0; i < 30; i += 1) {
    colors[i] = RGB16(colorScale(i));
  }
  setColors(colors);
}

function interpolate(startColor, endColor) {
  const url = `${BASE_URL}/interpolate`;
  // this deliberately treats startColor and endColor as if they were RGB16
  // even though they're RGB8 - effectively reducing their intensity by 256
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

export { BASE_URL, getColor, setColor, setColors, setGradient, interpolate };
