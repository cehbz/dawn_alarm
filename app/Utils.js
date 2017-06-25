import axios from 'axios';

const BASE_URL = 'http://dawn.local';

function getColor() {
  const url = `${BASE_URL}/color`;
  return axios.get(url, {}).then(response => response.data);
}

export { BASE_URL, getColor };
