<template>
    <div class="computer-block" @click="handleClick(computerId)">
      <img :src="imageUrl" alt="Computer Image" class="computer-image" />
      <div class="computer-info">
        <h3>{{ computerId }}</h3>
      </div>
    </div>
  </template>
  
  <script>
  export default {
    props: {
      computerId: {
        type: String,
        required: true,
      },
      imageUrl: {
        type: String,
        default: 'data:image/png;base64,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', // Place your default image here
      },
    },
    methods: {
      handleClick(computerId) {
        this.$emit('fetchData', computerId); // Emit event to fetch data for the selected computer
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
  