import axios from 'axios';

const BASE_URL = 'http://dawn.local';

function getColor() {
  const url = `${BASE_URL}/color`;
  return axios
    .get(url, {})
    .then(response => response.data)
    .catch(() => undefined);
}

function setColor(color) {
  const colors = [];
  for (let i = 0; i < 30; i++) {
    colors[i] = color;
  }
  setColors(colors);
}

function setColors(colors) {
  const url = `${BASE_URL}/colors`;
  return axios
    .post(url, colors)
    .then(response => response.data)
    .catch(error => {
      console.log(
        `setColor: error in axios.post('${url}', ${JSON.stringify(colors)})`,
      );
      console.log(error);
    });
}

export { BASE_URL, getColor, setColor, setColors };
