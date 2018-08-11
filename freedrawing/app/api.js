import axios from 'axios';

const baseURL = 'http://192.168.1.104:8090/rtree'

const api = axios.create({
  baseURL,
  timeout: 10000,
});

export default api;
