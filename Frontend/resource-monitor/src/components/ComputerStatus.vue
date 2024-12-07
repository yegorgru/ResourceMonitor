<template>
  <div> 
    <div style="display:flex; justify-content: space-between; align-items: flex-end; padding-left: 20px; padding-bottom:10px; padding-right:20px; border-bottom: 2px solid #0000ff;">
      <h1>{{ this.machineState.name }}</h1>
      <div>
        <button v-if="this.isStop" class="start-button" @click="startFetching">▶️ Start monitoring</button>
        <button v-if="!this.isStop" class="stop-button" @click="this.stopFetchingData">🛑 Stop monitoring</button>
      </div>
    </div>
    <div class="chart-container">
    <div>
        <h3>CPU Usage</h3>
        <canvas height="300vh" id="cpuChart"></canvas>
        <p>Usage: {{ this.machineState.cpu["usage %"] }}%</p>
      </div>
      <div>
        <h3>Memory Usage</h3>
        <canvas id="memoryChart"></canvas>
        <p>Usage: {{ this.machineState.memory["usage %"] }}%</p>
      </div>
      <div>
        <h3>Disk Usage</h3>
        <canvas id="diskChart"></canvas>
        <p>Usage: {{ this.machineState.disk["usage %"] }}%</p>
      </div>
    </div>
 </div>

</template>

<script>
import Chart from 'chart.js/auto'
import { markRaw } from 'vue'
import { fetchComputerData, startFetchingData } from "@/services/apiService";

export default {
  props: ["machineState", "interval", "errorMessage"],
  data() {
    return{
      cpuChart: null,
      diskChart: null,
      memoryChart: null,
      isStop: false
    }
  },
  mounted(){
    const cpu  = new Chart(document.getElementById('cpuChart'), {
        type: 'line',
        data:{
          labels: Array(60).fill(''),
          datasets: [
            {
              label: 'CPU Usage (%)',
              data: Array(2).fill(this.machineState.cpu["usage %"]),
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
    const memory = new Chart(document.getElementById('memoryChart'), {
        type: 'pie',
        data:{
          labels: ['Used', 'Free'],
          datasets: [
            {
              label: 'Memory Usage',
              data: [this.machineState.memory["used GB"], this.machineState.memory["total GB"]], // Спочатку значення на 100% вільно
              backgroundColor: ['#0000ff', '#0099ff'],
              hoverOffset: 4,
            }
          ],
          options:{
            responsive: true,
            plugins: {
              legend: {
                position: 'top',
              },
            },
          }
      },
    })
    const disk = new Chart(document.getElementById('diskChart'), {
        type: 'pie',
        data:{
          labels: ['Used', 'Free'],
          datasets: [
            {
              label: 'Disk Usage',
              data: [this.machineState.disk["used GB"], this.machineState.disk["total GB"]], // Спочатку значення на 100% вільно
              backgroundColor: ['#0000ff', '#0099ff'],
              hoverOffset: 4,
            }
          ],
          options:{
            responsive: true,
            plugins: {
              legend: {
                position: 'top',
              },
              render: 'value',
            },
          }
      },
    })
    this.cpuChart = markRaw(cpu)
    this.memoryChart = markRaw(memory)
    this.diskChart = markRaw(disk)
    
    if (this.chartInterval) {
        clearInterval(this.chartInterval);
    }
    this.startCharts();
  },  
  methods: {
    startCharts(){
      this.chartInterval = setInterval(() => {
        if (this.machineState == null){
          return
        }
        const newCpuValue = this.machineState.cpu["usage %"]
       
        if (this.cpuChart.data.datasets[0].data.length === 60) {
          for (let i = 0; i < 2; i++) {
            this.cpuChart.data.datasets[0].data.shift()
          }
        }
        for (let i = 0; i < 2; i++) {
          this.cpuChart.data.datasets[0].data.push(newCpuValue)
        }

        // Оновлення даних для Memory
        this.memoryChart.data.datasets[0].data = [this.machineState.memory["used GB"], this.machineState.memory["total GB"]];

        // Оновлення даних для Disk
        this.diskChart.data.datasets[0].data = [this.machineState.disk["used GB"], this.machineState.disk["total GB"]];

        // Оновлення графіків
        this.cpuChart.update();
        this.memoryChart.update();
        this.diskChart.update();
      }, 5000);
    },
    startFetching() {
      startFetchingData(
          fetchComputerData,
          this.interval, 
          5000,              
          this.machineState.id,
          this.updateMachineState,
        );
        this.startCharts();
        this.isStop = false
  },
  updateMachineState(data, errorMessage) {
      if (data) {
        this.machineState.cpu["usage %"] = data.cpu["usage %"];
        this.machineState.memory["used GB"] = data.memory["used GB"];
        this.machineState.disk["used GB"] = data.disk["used GB"];
        this.machineState.memory["total GB"] = data.memory["total GB"];
        this.machineState.disk["total GB"] = data.disk["total GB"];
        this.machineState.memory["usage %"] = data.memory["usage %"];
        this.machineState.disk["usage %"] = data.disk["usage %"];
        this.errorMessage.ref = "";
      } else {
        this.machineState = null;
        this.errorMessage.ref = errorMessage;
      }
    },
  stopFetchingData() {
      // Зупинити інтервал при необхідності
      if (this.interval.ref) {
        clearInterval(this.interval.ref);
      }
      if (this.chartInterval) {
        clearInterval(this.chartInterval);
      }
      this.isStop = true
  },
},
beforeDestroy() {
    // Очищення інтервалу при виході з компонента
    this.stopFetchingData();
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