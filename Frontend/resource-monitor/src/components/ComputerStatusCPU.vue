<template>
    <div> 
      <div style="display:flex; justify-content: space-between; align-items: flex-end; padding-left: 20px; padding-bottom:10px; padding-right:20px; border-bottom: 2px solid #0000ff;">
        <h1>{{ this.machineState.ip }}</h1>
      </div>
      <div class="chart-container">
      <div>
          <h3>Number of CPU User</h3>
          <canvas height="300vh" id="cpu_user"></canvas>
        </div>
        <div>
          <h3>CPU System</h3>
          <canvas height="300vh" id="cpu_system"></canvas>
        </div>
        <div>
          <h3>CPU Idle</h3>
          <canvas height="300vh" id="cpu_idle"></canvas>
        </div>
        <div>
          <h3>CPU Usage</h3>
          <canvas height="300vh" id="cpu_usage"></canvas>
        </div>
        <div>
          <h3>Current frequency Mhz</h3>
          <canvas height="300vh" id="freq_curr_Mhz"></canvas>
        </div>
        <div>
          <h3>Min frequency Mhz</h3>
          <canvas height="300vh" id="freq_min_Mhz"></canvas>
        </div>
        <div>
          <h3>Max frequency Mhz</h3>
          <canvas height="300vh" id="freq_max_Mhz"></canvas>
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
        cpuUserChart: null,
        cpuSystemChart: null,
        cpuIdleChart: null,
        cpuUsageChart: null,
        freqCurrMhzChart: null,
        freqMinMhzChart: null,
        freqMaxMhzChart: null,
      }
    },
    mounted(){
      const cpuUserArray = this.machineState.rows.map(rows => rows.cpu_times.cpu_user);
      const cpuSystemArray = this.machineState.rows.map(rows => rows.cpu_times.cpu_system);
      const cpuIdleArray = this.machineState.rows.map(rows => rows.cpu_times.cpu_idle);
      const cpuUsageArray = this.machineState.rows.map(rows => rows["cpu_usage %"]);
      const freqCurrMhzArray = this.machineState.rows.map(rows => rows.cpu_freq["freq_curr Mhz"]);
      const freqMinMhzArray = this.machineState.rows.map(rows => rows.cpu_freq["freq_min Mhz"]);
      const freqMaxMhzArray = this.machineState.rows.map(rows => rows.cpu_freq["freq_max Mhz"]);
      const cpuUser  = new Chart(document.getElementById('cpu_user'), {
          type: 'line',
          data:{
            labels: Array(cpuUserArray.length).fill(''),
            datasets: [
              {
                label: 'Number of CPU User',
                data: cpuUserArray,
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
      const cpuSystem = new Chart(document.getElementById('cpu_system'), {
        type: 'line',
          data:{
            labels: Array(cpuSystemArray.length).fill(''),
            datasets: [
              {
                label: 'CPU System',
                data: cpuSystemArray,
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
      const cpuIdle = new Chart(document.getElementById('cpu_idle'), {
        type: 'line',
          data:{
            labels: Array(cpuIdleArray.length).fill(''),
            datasets: [
              {
                label: 'CPU Idle',
                data: cpuIdleArray,
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
      const cpuUsage = new Chart(document.getElementById('cpu_usage'), {
        type: 'line',
          data:{
            labels: Array(cpuUsageArray.length).fill(''),
            datasets: [
              {
                label: 'CPU Usage',
                data: cpuUsageArray,
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
      const freqCurrMhz = new Chart(document.getElementById('freq_curr_Mhz'), {
        type: 'line',
          data:{
            labels: Array(freqCurrMhzArray.length).fill(''),
            datasets: [
              {
                label: 'Current frequency Mhz',
                data: freqCurrMhzArray,
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
      const freqMinMhz = new Chart(document.getElementById('freq_min_Mhz'), {
        type: 'line',
          data:{
            labels: Array(freqMinMhzArray.length).fill(''),
            datasets: [
              {
                label: 'Min frequency Mhz',
                data: freqMinMhzArray,
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
      const freqMaxMhz = new Chart(document.getElementById('freq_max_Mhz'), {
        type: 'line',
          data:{
            labels: Array(freqMaxMhzArray.length).fill(''),
            datasets: [
              {
                label: 'Max frequency Mhz',
                data: freqMaxMhzArray,
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
      this.cpuUserChart = markRaw(cpuUser)
      this.cpuSystemChart = markRaw(cpuSystem)
      this.cpuIdleChart = markRaw(cpuIdle)
      this.cpuUsageChart = markRaw(cpuUsage)
      this.freqCurrMhzChart = markRaw(freqCurrMhz)
      this.freqMinMhzChart = markRaw(freqMinMhz)
      this.freqMaxMhzChart = markRaw(freqMaxMhz)
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