import axios from 'axios';

const baseURL = 'http://localhost:8090/rtree'

const api = axios.create({
  baseURL,
  timeout: 10000,
});

export default api;
