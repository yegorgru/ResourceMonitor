<template>
    <div class="machine-info">
      <h2>{{ data.name }}</h2>
      <h1>Time: {{ data.time }}</h1>
      <div>
        <h3>CPU</h3>
        <p>Usage: {{ data.cpu["usage %"] }}%</p>
      </div>
      <div>
        <h3>Memory</h3>
        <p>Usage: {{ data.memory["usage %"] }}%</p>
        <p>Total: {{ data.memory["total GB"] }} GB</p>
        <p>Used: {{ data.memory["used GB"] }} GB</p>
      </div>
      <div>
        <h3>Disk</h3>
        <p>Usage: {{ data.disk["usage %"] }}%</p>
        <p>Total: {{ data.disk["total GB"] }} GB</p>
        <p>Used: {{ data.disk["used GB"] }} GB</p>
      </div>
    </div>
  </template>
  
  <script>
  export default {
    props: ["data"],
  };
  </script>
  
  <style scoped>
  .machine-info {
    border: 1px solid #000000;
    padding: 20px;
    color: #000;
    margin-top: 20px;
    max-width: 400px;
    margin-left: auto;
    margin-right: auto;
    text-align: left;
  }
  .machine-info p {
    margin-left: 15px;
  }
  </style>