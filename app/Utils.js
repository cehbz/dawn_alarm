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
  const url = `${BASE_URL}/color`;
  return axios.post(url, color).then(response => response.data).catch(error => {
    console.log(
      `setColor: error in axios.post('${url}', ${JSON.stringify(color)})`
    );
    console.log(error);
  });
}

export { BASE_URL, getColor, setColor };
