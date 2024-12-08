<template>
  <div> 
    <div style="display:flex; justify-content: space-between; align-items: flex-end; padding-left: 20px; padding-bottom:10px; padding-right:20px; border-bottom: 2px solid #0000ff;">
      <h1>{{ this.machineState.machineIp }}</h1>
    </div>
    <div class="chart-container">
    <div>
        <h3>CPU Count</h3>
        <canvas height="300vh" id="numcpus"></canvas>
      </div>
      <div>
        <h3>Total Virtual Memory GB</h3>
        <canvas height="300vh" id="total_virt_mem_GB"></canvas>
      </div>
      <div>
        <h3>Total Swap Memory GB</h3>
        <canvas height="300vh" id="total_swap_mem_GB"></canvas>
      </div>
      <div>
        <h3>Disk Partitions Count</h3>
        <canvas height="300vh" id="numdisks"></canvas>
      </div>
      <div>
        <h3>Total Disk GB </h3>
        <canvas height="300vh" id="total_C_disk_GB"></canvas>
      </div>
    </div>
 </div>

</template>

<script>
import Chart from 'chart.js/auto'
import { markRaw } from 'vue'
import { fetchComputerDataBasic } from "@/services/apiService";

export default {
  props: ["machineState", "interval", "n", "errorMessage"],
  data() {
    return{
      numcpusChart: null,
      total_virt_mem_GBChart: null,
      total_swap_mem_GBChart: null,
      numdisksChart: null,
      total_C_disk_GBChart: null
    }
  },
  mounted(){
    const numcpusArray = this.machineState.rows.map(rows => rows.numcpus).reverse();
    const total_virt_mem_GBArray = this.machineState.rows.map(rows => rows["total virt mem GB"]).reverse();
    const total_swap_mem_GBArray = this.machineState.rows.map(rows => rows["total swap mem GB"]).reverse();
    const numdisksArray = this.machineState.rows.map(rows => rows["numdisks"]).reverse();
    const total_C_disk_GBArray = this.machineState.rows.map(rows => rows["total_C_disk GB"]).reverse();
    console.log(numdisksArray)
    console.log(total_C_disk_GBArray)
    const numcpus  = new Chart(document.getElementById('numcpus'), {
        type: 'line',
        data:{
          labels: Array(numcpusArray.length).fill(''),
          datasets: [
            {
              label: 'CPU Count',
              data: numcpusArray,
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
    const total_virt_mem_GB = new Chart(document.getElementById('total_virt_mem_GB'), {
      type: 'line',
        data:{
          labels: Array(total_virt_mem_GBArray.length).fill(''),
          datasets: [
            {
              label: 'Total virtual memory GB',
              data: total_virt_mem_GBArray,
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
    const total_swap_mem_GB = new Chart(document.getElementById('total_swap_mem_GB'), {
      type: 'line',
        data:{
          labels: Array(total_swap_mem_GBArray.length).fill(''),
          datasets: [
            {
              label: 'Total swap memory GB',
              data: total_swap_mem_GBArray,
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
    const numdisks = new Chart(document.getElementById('numdisks'), {
      type: 'line',
        data:{
          labels: Array(numdisksArray.length).fill(''),
          datasets: [
            {
              label: 'Disk Partitions Count',
              data: numdisksArray,
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
    const total_C_disk_GB = new Chart(document.getElementById('total_C_disk_GB'), {
      type: 'line',
        data:{
          labels: Array(total_C_disk_GBArray.length).fill(''),
          datasets: [
            {
              label: 'Total Disk GB',
              data: total_C_disk_GBArray,
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
    this.numcpusChart = markRaw(numcpus)
    this.total_virt_mem_GBChart = markRaw(total_virt_mem_GB)
    this.total_swap_mem_GBChart = markRaw(total_swap_mem_GB)
    this.numdisksChart = markRaw(numdisks)
    this.total_C_disk_GBChart = markRaw(total_C_disk_GB)
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