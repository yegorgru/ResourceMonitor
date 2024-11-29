<template>
  <div id="app">
    <Navbar />
    <main class="main-content">
      <router-view />
      <div class="grid-container">
        <div class="left-space"></div>
        <div class="content">
          <div style="display: flex; flex-direction: row;">
              <span class="list-icon" @click="changeInfo">💻</span>
              <div class="search-container">
                <input
                  v-model="computerId"
                  type="number"
                  placeholder="Enter Computer ID"
                  @keyup.enter="startFetchingComputerData"
                  :class="{'input-error ': this.errorMessage}"
                />
                <span class="search-icon" @click="startFetchingComputerData">🔍</span>
              </div>
          </div>
          <div style="display:flex; justify-content: center;">
            <div v-if="this.errorMessage" class="error">{{ this.errorMessage }}</div>
          </div>
         <div v-if="!isCmpInfo" class="blocks-container">
            <ComputerBlock 
              @fetch-data="startFetchingComputerBlockData"
              v-for="computer in computers" 
              :key="computer.id" 
              :computerId="computer.id"
              :imageUrl="computer.imageUrl"
            />
          </div>
          <ComputerStatus v-if="this.isCmpInfo && machineState!=null" :data="machineState" :interval="interval"/>
        </div>
        <div class="right-space"></div>
      </div>
    </main>

  <Footer />
  </div>
</template>

<script>
import Navbar from "./components/Navbar.vue";
import Footer from './components/Footer.vue';

import ComputerBlock from './components/ComputerBlock.vue';
import ComputerStatus from "./components/ComputerStatus.vue";

export default {
  components: { Navbar,   Footer, ComputerBlock, ComputerStatus },
  data() {
    return {
      computerId: "",
      machineState: null,
      isCmpInfo: false,
      errorMessage: "",
      interval: null,
      computers: [
        { id: '1' },
        { id: '2' },
      ],
    };
  },
  methods: {
    async fetchComputerData() {
      try {
        const response = await fetch(`http://127.0.0.1:5000/api/data/${this.computerId}`);
        
        if (!response.ok) {
          throw new Error("Failed to fetch data. Please check the computer ID and try again.");
        }

        const data = await response.json();
        this.machineState = data;
        this.errorMessage = "";
        this.isCmpInfo = true
      } catch (error) {
        this.errorMessage = error.message;
        this.machineState = null;
      }
    },
    async fetchComputerBlockData(computerId) {
      try {
        const response = await fetch(`http://127.0.0.1:5000/api/data/${this.computerId}`);
        
        if (!response.ok) {
          throw new Error("Failed to fetch data. Please check the computer ID and try again.");
        }

        const data = await response.json();
        this.machineState = data;
        this.errorMessage = "";
        this.isCmpInfo = true
      } catch (error) {
        this.errorMessage = error.message;
        this.machineState = null;
      }
    },
    startFetchingComputerData() {
      console.log(this.computerId)
      if (this.computerId === "") {
        this.errorMessage = "Please enter a computer ID.";
        console.log(this.errorMessage)
        return;
      }
      

      // Зупинити існуючий інтервал, якщо він вже працює
      if (this.fetchInterval) {
        clearInterval(this.fetchInterval);
      }

      // Почати новий інтервал для запиту кожні 5 секунд
      this.fetchInterval = setInterval(() => {
        this.fetchComputerData();
      }, 5000);

      // Одразу виконати перший запит
      this.fetchComputerData();
    },
    startFetchingComputerBlockData(computerId) {
      this.computerId = computerId
      console.log(this.computerId)
      if (this.computerId === "") {
        this.errorMessage = "Please enter a computer ID.";
        console.log(this.errorMessage)
        return;
      }
      

      // Зупинити існуючий інтервал, якщо він вже працює
      if (this.fetchInterval) {
        clearInterval(this.fetchInterval);
      }

      // Почати новий інтервал для запиту кожні 5 секунд
      this.fetchInterval = setInterval(() => {
        this.fetchComputerBlockData(computerId);
      }, 5000);

      // Одразу виконати перший запит
      this.fetchComputerBlockData(computerId);
    },
    stopFetchingData() {
      // Зупинити інтервал при необхідності
      if (this.fetchInterval) {
        clearInterval(this.fetchInterval);
        this.fetchInterval = null;
      }
    },
    changeInfo(){
      if (this.isCmpInfo){
        this.isCmpInfo = !this.isCmpInfo
        location.reload()
      }
    }
  },
  beforeDestroy() {
       // Очищення інтервалу при виході з компонента
       this.stopFetchingData();
  },
};
</script>

<style>
@import './assets/main.css';

/* Main container setup with Flexbox */
#app {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
}

.main-content {
  flex: 1;
  display: flex;
  justify-content: center;
  align-items: center;
  background-color: #ffffff;
  color: #0000ff;
}

.content-placeholder {
  text-align: center;
  font-size: 1.5rem;
}

.grid-container {
  display: grid;
  grid-template-columns: 1fr 4fr 1fr;
  width: 100%; 
  height: 100%;
}

.content {
  background-color: #ffffff;
  color: #0000ff;
  border-radius: 8px;
  box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.1);
  text-align: center;
}

.left-space,
.right-space {
  background-color: transparent; /* Порожній простір */
}

.blocks-container {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
  padding-top: 20px;
}

.search-container {
  position: relative;
  width: 100%; /* Full width of content */
  margin: 20px;
}

.search-container input {
  width: 100%;
  padding: 10px;
  font-size: 16px;
  color: #0099ff;
  border: 1px solid #0000ff;
  border-radius: 5px;
  box-sizing: border-box;
}

.search-container input::placeholder {
  color: #0099ff;
  opacity: 1;
}

.search-container input:focus {
  border-color: #0099ff;
  box-shadow: 0 0 8px rgba(0, 153, 255, 0.5); 
  outline: none; 
}

.search-icon {
  position: absolute;
  right: 15px;
  color: #0000ff;
  cursor: pointer;
  top: 50%;
  transform: translateY(-50%);
  font-size: 1.25em;
}

.list-icon {
  margin-top: 20px;
  margin-left: 25px;
  color: #0000ff;
  cursor: pointer;
  font-size: 1.5em;
}

.error {
  color: red;
  width: 50%;
  border: 1px solid #0000ff; 
  border-radius: 10px;
  padding: 10px;
  margin-top: 10px;
  font-size: 0.9em;
}

.input-error {
  border: 1px solid red; 
}

.footer {
  background-color: #333;
  color: white;
  text-align: center;
  padding: 10px;
  min-height: 50px;
  margin-top: auto; /* Pushes the footer to the bottom */
}
</style>
