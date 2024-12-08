<template>
    <div> 
      <div style="display:flex; justify-content: space-between; align-items: flex-end; padding-left: 20px; padding-bottom:10px; padding-right:20px; border-bottom: 2px solid #0000ff;">
        <h1>{{ this.machineState.ip }}</h1>
      </div>
      <div class="chart-container">
      <div>
          <h3>Virtual Memory Usage %</h3>
          <canvas height="300vh" id="usage_virt"></canvas>
        </div>
        <div>
          <h3>Virtual Memory Used GB</h3>
          <canvas height="300vh" id="used_virt"></canvas>
        </div>
        <div>
          <h3>Virtual Memory Available GB</h3>
          <canvas height="300vh" id="available_virt"></canvas>
        </div>
        <div>
          <h3>Usage Swap Memory %</h3>
          <canvas height="300vh" id="usage_swap"></canvas>
        </div>
        <div>
          <h3>Used Swap Memory GB</h3>
          <canvas height="300vh" id="used_swap"></canvas>
        </div>
        <div>
          <h3>Free Swap Memory GB</h3>
          <canvas height="300vh" id="free_swap"></canvas>
        </div>
      </div>
   </div>
  
  </template>
  
  <script>
  import Chart from 'chart.js/auto'
  import { markRaw } from 'vue'
  
  export default {
    props: ["machineState", "interval", "n", "errorMessage"],
    data() {
      return{
        usageVirtChart: null,
        usedVirtChart: null,
        availableVirtChart: null,
        usageSwapChart: null,
        usedSwapChart: null,
        freeSwapChart: null,
      }
    },
    mounted(){
      const usageVirtArray = this.machineState.rows.map(rows => rows.virt_memory["usage virt %"]).reverse();
      const usedVirtArray = this.machineState.rows.map(rows => rows.virt_memory["used virt GB"]).reverse();
      const availableVirtArray = this.machineState.rows.map(rows => rows.virt_memory["available virt GB"]).reverse();
      const  usageSwapArray = this.machineState.rows.map(rows => rows.swap_memory["usage swap %"]).reverse();
      const  usedSwapArray = this.machineState.rows.map(rows => rows.swap_memory["used swap GB"]).reverse();
      const  freeSwapArray = this.machineState.rows.map(rows => rows.swap_memory["free swap GB"]).reverse();
      const usageVirt  = new Chart(document.getElementById('usage_virt'), {
          type: 'line',
          data:{
            labels: Array(usageVirtArray.length).fill(''),
            datasets: [
              {
                label: 'Virtual Memory Usage %',
                data: usageVirtArray,
                backgroundColor: 'rgba(0, 123, 255, 0.2)',
                borderColor: '#00f',
                borderWidth: 2,
                pointRadius: 0,  // Прибираємо точки даних
                fill: true,
              }
            ],
            options:{
              legend: {display: false},
              responsive: true,
              maintainAspectRatio: false,
              plugins: {
                legend: {
                  display: true,
                },
                tooltip: {
                  enabled: true,
                },
              },
              scales: {
                y: {
                    display: true,
                    min: 0,
                    max: 100,
                }
              }
            }
        },
      })
      const usedVirt  = new Chart(document.getElementById('used_virt'), {
          type: 'line',
          data:{
            labels: Array(usedVirtArray.length).fill(''),
            datasets: [
              {
                label: 'Virtual Memory Used GB',
                data: usedVirtArray,
                backgroundColor: 'rgba(0, 123, 255, 0.2)',
                borderColor: '#00f',
                borderWidth: 2,
                pointRadius: 0,  // Прибираємо точки даних
                fill: true,
              }
            ],
            options:{
              legend: {display: false},
              responsive: true,
              maintainAspectRatio: false,
              plugins: {
                legend: {
                  display: true,
                },
                tooltip: {
                  enabled: true,
                },
              },
              scales: {
                y: {
                    display: true,
                    min: 0,
                    max: 100,
                }
              }
            }
        },
      })
      const availableVirt  = new Chart(document.getElementById('available_virt'), {
          type: 'line',
          data:{
            labels: Array(availableVirtArray.length).fill(''),
            datasets: [
              {
                label: 'Virtual Memory Available GB',
                data: availableVirtArray,
                backgroundColor: 'rgba(0, 123, 255, 0.2)',
                borderColor: '#00f',
                borderWidth: 2,
                pointRadius: 0,  // Прибираємо точки даних
                fill: true,
              }
            ],
            options:{
              legend: {display: false},
              responsive: true,
              maintainAspectRatio: false,
              plugins: {
                legend: {
                  display: true,
                },
                tooltip: {
                  enabled: true,
                },
              },
              scales: {
                y: {
                    display: true,
                    min: 0,
                    max: 100,
                }
              }
            }
        },
      })
      const usageSwap  = new Chart(document.getElementById('usage_swap'), {
          type: 'line',
          data:{
            labels: Array(usageSwapArray.length).fill(''),
            datasets: [
              {
                label: 'Usage Swap Memory %',
                data: usageSwapArray,
                backgroundColor: 'rgba(0, 123, 255, 0.2)',
                borderColor: '#00f',
                borderWidth: 2,
                pointRadius: 0,  // Прибираємо точки даних
                fill: true,
              }
            ],
            options:{
              legend: {display: false},
              responsive: true,
              maintainAspectRatio: false,
              plugins: {
                legend: {
                  display: true,
                },
                tooltip: {
                  enabled: true,
                },
              },
              scales: {
                y: {
                    display: true,
                    min: 0,
                    max: 100,
                }
              }
            }
        },
      })
      const usedSwap  = new Chart(document.getElementById('used_swap'), {
          type: 'line',
          data:{
            labels: Array(usedSwapArray.length).fill(''),
            datasets: [
              {
                label: 'Used Swap Memory GB',
                data: usedSwapArray,
                backgroundColor: 'rgba(0, 123, 255, 0.2)',
                borderColor: '#00f',
                borderWidth: 2,
                pointRadius: 0,  // Прибираємо точки даних
                fill: true,
              }
            ],
            options:{
              legend: {display: false},
              responsive: true,
              maintainAspectRatio: false,
              plugins: {
                legend: {
                  display: true,
                },
                tooltip: {
                  enabled: true,
                },
              },
              scales: {
                y: {
                    display: true,
                    min: 0,
                    max: 100,
                }
              }
            }
        },
      })
      const freeSwap  = new Chart(document.getElementById('free_swap'), {
          type: 'line',
          data:{
            labels: Array(freeSwapArray.length).fill(''),
            datasets: [
              {
                label: 'Free Swap Memory GB',
                data: freeSwapArray,
                backgroundColor: 'rgba(0, 123, 255, 0.2)',
                borderColor: '#00f',
                borderWidth: 2,
                pointRadius: 0,  // Прибираємо точки даних
                fill: true,
              }
            ],
            options:{
              legend: {display: false},
              responsive: true,
              maintainAspectRatio: false,
              plugins: {
                legend: {
                  display: true,
                },
                tooltip: {
                  enabled: true,
                },
              },
              scales: {
                y: {
                    display: true,
                    min: 0,
                    max: 100,
                }
              }
            }
        },
      })
      this.usageVirtChart = markRaw(usageVirt)
      this.usedVirtChart = markRaw(usedVirt)
      this.availableVirtChart = markRaw(availableVirt)
      this.usageSwapChart = markRaw(usageSwap)
      this.usedSwapChart = markRaw(usedSwap)
      this.freeSwapChart = markRaw(freeSwap)
    },  
  };
  </script>
  
  <style scoped>
  .monitoring-dashboard {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
  }
  
  .chart-container {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    flex-wrap: wrap;
    margin: 20px 0;
    padding: 20px;
  }
  .start-button, .stop-button {
    background-color: #0000ff; /* Червоний колір для виділення кнопки */
    color: #ffffff; /* Білий колір тексту */
    font-size: 16px;
    font-family: 'Titillium Web', sans-serif;
    border: none;
    border-radius: 5px;
    padding: 10px 20px;
    margin: 5px;
    cursor: pointer;
    transition: background-color 0.3s ease, transform 0.2s;
    box-shadow: 0px 2px 5px rgba(0, 0, 0, 0.2); /* Тінь для кнопки */
  }
  
  .start-button:hover {
    background-color: #0077cc; /* Темніший відтінок для кнопки "Start" */
  }
  
  .stop-button:hover {
    background-color: #0099ff; /* Трохи темніший червоний при наведенні */
    transform: scale(1.05); /* Легке збільшення при наведенні */
  }
  
  .stop-button:active {
    background-color: #cc0000; /* Темно-червоний при натисканні */
    transform: scale(0.95); /* Легке зменшення при натисканні */
  }
  
  .start-button:active {
    background-color: #00e600; /* Темно-червоний при натисканні */
    transform: scale(0.95); /* Легке зменшення при натисканні */
  }
  
  
  </style>