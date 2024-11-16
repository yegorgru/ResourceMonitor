<template>
  <div class="machine-info">
    <h2>{{ data.name }}</h2>
    <h1>Time: {{ data.time }}</h1>
    
    <div>
      <h3>CPU</h3>
      <canvas ref="cpuChart"></canvas>
      <p>Usage: {{ data.cpu["usage %"] }}%</p>
    </div>
    
    <div>
      <h3>Memory</h3>
      <canvas ref="memoryChart"></canvas>
      <p>Usage: {{ data.memory["usage %"] }}%</p>
      <p>Total: {{ data.memory["total GB"] }} GB</p>
      <p>Used: {{ data.memory["used GB"] }} GB</p>
    </div>
    
    <div>
      <h3>Disk</h3>
      <canvas ref="diskChart"></canvas>
      <p>Usage: {{ data.disk["usage %"] }}%</p>
      <p>Total: {{ data.disk["total GB"] }} GB</p>
      <p>Used: {{ data.disk["used GB"] }} GB</p>
    </div>
  </div>
</template>

<script>
import Chart from 'chart.js/auto';

export default {
  props: ["data"],
  data() {
    return {
      cpuChart: null,
      memoryChart: null,
      diskChart: null,
    };
  },
  mounted() {
    this.createChart('cpuChart', this.data.cpu["usage %"]);
    this.createChart('memoryChart', this.data.memory["usage %"]);
    this.createChart('diskChart', this.data.disk["usage %"]);
  },
  watch: {
    data(newData)  {
        this.updateChart(this['cpuChart'] , newData.cpu["usage %"]);
        // this.updateChart(this.memoryChart, newData.memory["usage %"]);
        // this.updateChart(this.diskChart, newData.disk["usage %"]);
      }
  },
  methods: {
    createChart(ref, initialUsage) {
      if (!this.$refs[ref]) return;
      console.log(initialUsage)
      this[ref] = new Chart(this.$refs[ref].getContext("2d"), {
        type: 'line',
        data: {
          labels: Array(60).fill(''),
          datasets: [{
            label: 'Usage %',
            backgroundColor: 'rgba(0, 123, 255, 0.2)',
            borderColor: 'rgba(0, 123, 255, 1)',
            borderWidth: 2,
            pointRadius: 0,  // Прибираємо точки даних
            data: Array(5).fill(initialUsage),
            fill: true,
          }],
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          scales: {
            x: {
              display: false,  // Прибираємо ось `x`
            },
            y: {
              beginAtZero: true,
              max: 100,
            },
          },
          plugins: {
            legend: {
              display: false,  // Прибираємо легенду
            },
          },
          animation: false, // Вимикаємо анімацію для плавного оновлення
        },
      });
    },
    updateChart(chart, newUsage) {
      console.log(chart)
      console.log(newUsage)
      if(chart){
        chart.data.datasets[0].data = Array(5).fill(newUsage);
        chart.update('none');
      }
    },
  },
};
</script>

<style scoped>
.machine-info {
  display: flex;
  flex-direction: row;
  height: 200px;
  border: 1px solid #000000;
  padding: 20px;
  color: #000;
  margin-top: 20px;
  margin-left: auto;
  margin-right: auto;
  text-align: left;
  gap: 20px;
}
.machine-info p {
  margin-left: 15px;
}
canvas {
  width: 100%;
  height: 100px;
}
</style>
<!-- .machine-info {
  display: flex;
  flex-direction: row;
  height: 200px;
  border: 1px solid #000000;
  padding: 20px;
  color: #000;
  margin-top: 20px;
  margin-left: auto;
  margin-right: auto;
  text-align: left;
  gap: 20px;
} -->