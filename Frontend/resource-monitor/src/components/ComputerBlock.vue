<template>
    <div class="computer-block" @click="handleClick(machineId)">
      <img src="@/assets/monitor.png" alt="Computer Image" class="computer-image" />
      <div class="computer-info">
        <h3>{{ machineId }}</h3>
      </div>
    </div>
  </template>
  
  <script>
  export default {
    props: {
      machineId: {
        type: String,
        required: true,
      },
    },
    methods: {
      handleClick(machineId) {
        this.$emit('fetchData', machineId);
      },
    },
  };
  </script>
  
  <style scoped>
  .computer-block {
    border: 1px solid #ccc;
    padding: 20px;
    margin: 15px;
    width: 200px;
    text-align: center;
    border-radius: 10px;
    cursor: pointer;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    transition: transform 0.3s ease, box-shadow 0.3s ease;
  }
  
  .computer-block:hover {
    transform: scale(1.05);
    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.2);
  }
  
  .computer-image {
    width: 100%;
    height: auto;
    border-radius: 8px;
  }
  
  .computer-info {
    margin-top: 10px;
  }
  
  .computer-info h3 {
    font-size: 1.2em;
    margin: 0;
    color: #333;
  }
  </style>
  