<template>
   <div class="chart-container">
    <div>
        <h3>CPU Usage</h3>
        <canvas height="300vh" id="cpuChart"></canvas>
        <p>Usage: {{ this.data.cpu["usage %"] }}%</p>
      </div>
      <div>
        <h3>Memory Usage</h3>
        <canvas id="memoryChart"></canvas>
        <p>Usage: {{ this.data.memory["usage %"] }}%</p>
      </div>
      <div>
        <h3>Disk Usage</h3>
        <canvas id="diskChart"></canvas>
        <p>Usage: {{ this.data.disk["usage %"] }}%</p>
      </div>
    </div>
</template>

<script>
import Chart from 'chart.js/auto'
import { markRaw } from 'vue'
export default {
  props: ["data"],
  data() {
    return{
      cpuChart: null,
      diskChart: null,
      memoryChart: null,
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
              data: Array(2).fill(this.data.cpu["usage %"]),
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
              data: [this.data.memory["used GB"], this.data.memory["total GB"]], // Спочатку значення на 100% вільно
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
              data: [this.data.disk["used GB"], this.data.disk["total GB"]], // Спочатку значення на 100% вільно
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
    setInterval(() => {
        if (this.data == null){
          return
        }
        const newCpuValue = this.data.cpu["usage %"]
       
        if (this.cpuChart.data.datasets[0].data.length === 60) {
          for (let i = 0; i < 2; i++) {
            this.cpuChart.data.datasets[0].data.shift()
          }
        }
        for (let i = 0; i < 2; i++) {
          this.cpuChart.data.datasets[0].data.push(newCpuValue)
        }

        // Оновлення даних для Memory
        this.memoryChart.data.datasets[0].data = [this.data.memory["usage %"], 100 - this.data.memory["usage %"]];

        // Оновлення даних для Disk
        this.diskChart.data.datasets[0].data = [this.data.disk["usage %"], 100 - this.data.disk["usage %"]];

        // Оновлення графіків
        this.cpuChart.update();
        this.memoryChart.update();
        this.diskChart.update();
      }, 5000);
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
</style>