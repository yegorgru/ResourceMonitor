<template>
    <div> 
      <div style="display:flex; justify-content: space-between; align-items: flex-end; padding-left: 20px; padding-bottom:10px; padding-right:20px; border-bottom: 2px solid #0000ff;">
        <h1>{{ this.machineState.ip }}</h1>
      </div>
      <div class="chart-container">
      <div>
          <h3>Packets Sent</h3>
          <canvas height="300vh" id="packets_sent"></canvas>
        </div>
        <div>
          <h3>Packets Receive</h3>
          <canvas height="300vh" id="packets_receiv"></canvas>
        </div>
        <div>
          <h3>Bytes Sent</h3>
          <canvas height="300vh" id="bytes_sent"></canvas>
        </div>
        <div>
          <h3>Bytes Receiv</h3>
          <canvas height="300vh" id="bytes_receiv"></canvas>
        </div>
        <div>
          <h3>Errors In</h3>
          <canvas height="300vh" id="errors_in"></canvas>
        </div>
        <div>
          <h3>Errors Out</h3>
          <canvas height="300vh" id="errors_out"></canvas>
        </div>
        <div>
          <h3>Pack Drop In</h3>
          <canvas height="300vh" id="pack_drop_in"></canvas>
        </div>
        <div>
          <h3>Pack Drop Out</h3>
          <canvas height="300vh" id="pack_drop_out"></canvas>
        </div>
        <div>
          <h3>Connections</h3>
          <canvas height="300vh" id="connections"></canvas>
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
        packetsSentChart: null,
        packetsReceiveChart: null,
        bytesSentChart: null,
        bytesReceiveChart: null,
        bytesErrorInChart: null,
        bytesErrorOutChart: null,
        bytesPackDropInChart: null,
        bytesPackDropOutChart: null,
        connectionsChart: null,
      }
    },
    mounted(){
      const packetsSentArray = this.machineState.rows.map(rows => rows["i/o"]["packets_sent"]);
      const packetsReceiveArray = this.machineState.rows.map(rows => rows["i/o"]["packets_recv"]);
      const bytesSentArray = this.machineState.rows.map(rows =>  rows["i/o"]["bytes_sent"]);
      const  bytesReceiveArray = this.machineState.rows.map(rows => rows["i/o"]["bytes_recv"]);
      const  bytesErrorInArray = this.machineState.rows.map(rows => rows["i/o"]["errors in"]);
      const  bytesErrorOutArray = this.machineState.rows.map(rows =>  rows["i/o"]["errors out"]);
      const  bytesPackDropInArray = this.machineState.rows.map(rows => rows["i/o"]["pack drop in"]);
      const  bytesPackDropOutArray = this.machineState.rows.map(rows => rows["i/o"]["pack drop out"]);
      const  connectionsArray = this.machineState.rows.map(rows => rows["connections"]);
      const packetsSent  = new Chart(document.getElementById('packets_sent'), {
          type: 'line',
          data:{
            labels: Array(packetsSentArray.length).fill(''),
            datasets: [
              {
                label: 'Packets Sent',
                data: packetsSentArray,
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
      const packetsReceive  = new Chart(document.getElementById('packets_receiv'), {
          type: 'line',
          data:{
            labels: Array(packetsReceiveArray.length).fill(''),
            datasets: [
              {
                label: 'Packets Receive',
                data: packetsReceiveArray,
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
      const bytesSent  = new Chart(document.getElementById('bytes_sent'), {
          type: 'line',
          data:{
            labels: Array(bytesSentArray.length).fill(''),
            datasets: [
              {
                label: 'Bytes Sent',
                data: bytesSentArray,
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
      const bytesReceive  = new Chart(document.getElementById('bytes_receiv'), {
          type: 'line',
          data:{
            labels: Array(bytesReceiveArray.length).fill(''),
            datasets: [
              {
                label: 'Bytes Receive',
                data: bytesReceiveArray,
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
      const bytesErrorIn  = new Chart(document.getElementById('errors_in'), {
          type: 'line',
          data:{
            labels: Array(bytesErrorInArray.length).fill(''),
            datasets: [
              {
                label: 'Errors In',
                data: bytesErrorInArray,
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
      const bytesErrorOut  = new Chart(document.getElementById('errors_out'), {
          type: 'line',
          data:{
            labels: Array(bytesErrorOutArray.length).fill(''),
            datasets: [
              {
                label: 'Errors Out',
                data: bytesErrorOutArray,
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
      const bytesPackDropIn  = new Chart(document.getElementById('pack_drop_in'), {
          type: 'line',
          data:{
            labels: Array(bytesPackDropInArray.length).fill(''),
            datasets: [
              {
                label: 'Pack Drop In',
                data: bytesPackDropInArray,
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
      const bytesPackDropOut  = new Chart(document.getElementById('pack_drop_out'), {
          type: 'line',
          data:{
            labels: Array(bytesPackDropOutArray.length).fill(''),
            datasets: [
              {
                label: 'Pack Drop Out',
                data: bytesPackDropOutArray,
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
      const connections  = new Chart(document.getElementById('connections'), {
          type: 'line',
          data:{
            labels: Array(connectionsArray.length).fill(''),
            datasets: [
              {
                label: 'Connections',
                data: connectionsArray,
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
      this.packetsSentChart = markRaw(packetsSent)
      this.packetsReceiveChart = markRaw(packetsReceive)
      this.bytesSentChart = markRaw(bytesSent)
      this.bytesReceiveChart = markRaw(bytesReceive)
      this.bytesErrorInChart = markRaw(bytesErrorIn)
      this.bytesErrorOutChart = markRaw(bytesErrorOut)
      this.bytesPackDropInChart = markRaw(bytesPackDropIn)
      this.bytesPackDropOutChart = markRaw(bytesPackDropOut)
      this.connectionsChart = markRaw(connections)
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