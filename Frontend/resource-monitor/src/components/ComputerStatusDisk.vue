<template>
    <div> 
      <div style="display:flex; justify-content: space-between; align-items: flex-end; padding-left: 20px; padding-bottom:10px; padding-right:20px; border-bottom: 2px solid #0000ff;">
        <h1>{{ this.machineState.ip }}</h1>
      </div>
      <div class="chart-container">
      <div>
          <h3>Disk Usage %</h3>
          <canvas height="300vh" id="disk_usage"></canvas>
        </div>
        <div>
          <h3>Disk Usaged GB</h3>
          <canvas height="300vh" id="disk_used"></canvas>
        </div>
        <div>
          <h3>Disk Free GB</h3>
          <canvas height="300vh" id="disk_free"></canvas>
        </div>
        <div>
          <h3>Disk Read Count</h3>
          <canvas height="300vh" id="disk_read_count"></canvas>
        </div>
        <div>
          <h3>Disk Write Count</h3>
          <canvas height="300vh" id="disk_write_count"></canvas>
        </div>
        <div>
          <h3>Disk Read Bytes GB</h3>
          <canvas height="300vh" id="disk_read_bytes"></canvas>
        </div>
        <div>
          <h3>Disk Write Bytes GB</h3>
          <canvas height="300vh" id="disk_write_bytes"></canvas>
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
        diskUsageChart: null,
        diskUsedChart: null,
        diskFreeChart: null,
        diskReadCountChart: null,
        diskWriteCountChart: null,
        diskReadBytesChart: null,
        diskWriteBytesChart: null,
      }
    },
    mounted(){
      const diskUsageArray = this.machineState.rows.map(rows => rows["disk usage"]["usage %"]);
      const diskUsedArray = this.machineState.rows.map(rows => rows["disk usage"]["used GB"]);
      const diskFreeArray = this.machineState.rows.map(rows => rows["disk usage"]["free GB"]);
      const diskReadCountArray = this.machineState.rows.map(rows => rows["disk i/o"]["read_count"]);
      const diskWriteCountArray = this.machineState.rows.map(rows => rows["disk i/o"]["write_count"]);
      const diskReadBytesArray = this.machineState.rows.map(rows => rows["disk i/o"]["read_bytes GB"]);
      const diskWriteBytesArray = this.machineState.rows.map(rows => rows["disk i/o"]["write_bytes GB"]);
      const diskUsage  = new Chart(document.getElementById('disk_usage'), {
          type: 'line',
          data:{
            labels: Array(diskUsageArray.length).fill(''),
            datasets: [
              {
                label: 'Disk Usage %',
                data: diskUsageArray,
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
      const diskUsed  = new Chart(document.getElementById('disk_used'), {
          type: 'line',
          data:{
            labels: Array(diskUsedArray.length).fill(''),
            datasets: [
              {
                label: 'Disk Usaged GB',
                data: diskUsedArray,
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
      const diskFree  = new Chart(document.getElementById('disk_free'), {
          type: 'line',
          data:{
            labels: Array(diskFreeArray.length).fill(''),
            datasets: [
              {
                label: 'Disk Free GB',
                data: diskFreeArray,
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
      const diskReadCount  = new Chart(document.getElementById('disk_read_count'), {
          type: 'line',
          data:{
            labels: Array(diskReadCountArray.length).fill(''),
            datasets: [
              {
                label: 'Disk Read Count',
                data: diskReadCountArray,
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
      const diskWriteCount  = new Chart(document.getElementById('disk_write_count'), {
          type: 'line',
          data:{
            labels: Array(diskWriteCountArray.length).fill(''),
            datasets: [
              {
                label: 'Disk Write Count',
                data: diskWriteCountArray,
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
      const diskReadBytes  = new Chart(document.getElementById('disk_read_bytes'), {
          type: 'line',
          data:{
            labels: Array(diskReadBytesArray.length).fill(''),
            datasets: [
              {
                label: 'Disk Read Bytes GB',
                data: diskReadBytesArray,
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
      const diskWriteBytes  = new Chart(document.getElementById('disk_write_bytes'), {
          type: 'line',
          data:{
            labels: Array(diskWriteBytesArray.length).fill(''),
            datasets: [
              {
                label: 'Disk Write Bytes GB',
                data: diskWriteBytesArray,
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
      this.diskUsageChart = markRaw(diskUsage)
      this.diskUsedChart = markRaw(diskUsed)
      this.diskFreeChart = markRaw(diskFree)
      this.diskReadCount = markRaw(diskReadCount)
      this.diskWriteCountChart = markRaw(diskWriteCount)
      this.diskReadBytesChart = markRaw(diskReadBytes)
      this.diskWriteBytesChart = markRaw(diskWriteBytes)
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