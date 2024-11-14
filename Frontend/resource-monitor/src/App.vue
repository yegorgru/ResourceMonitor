<template>
  <div id="app">
    <Navbar />
    <div class="main-container">
      <!-- Sidebar, optional -->
      <div class="sidebar"></div>

      <!-- Main content area -->
      <div class="content">
        <!-- Search bar below navbar -->
         <div style="display: flex; flex-direction: row;">
          <span class="list-icon" @click="this.isCmpInfo = false">💻</span>
          <div class="search-container">
            <input
              v-model="computerId"
              placeholder="Enter Computer ID"
              @keyup.enter="startFetchingComputerData"
            />
            <span class="search-icon" @click="startFetchingComputerData">🔍</span>
          </div>
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
     

        <!-- Displaying fetched data -->
        <ComputerStatus v-if="this.isCmpInfo && machineState" :data="machineState" />
        <p v-if="errorMessage" class="error">{{ errorMessage }}</p>
      </div>

      <!-- Sidebar, optional -->
      <div class="sidebar"></div>
    </div>

    <!-- Footer -->
    <footer class="footer">
      <p>&copy; 2024 Your Company. All rights reserved.</p>
    </footer>
  </div>
</template>

<script>
import Navbar from "./components/Navbar.vue";
import ComputerBlock from './components/ComputerBlock.vue';
import ComputerStatus from "./components/ComputerStatus.vue";

export default {
  components: { Navbar, ComputerBlock, ComputerStatus },
  data() {
    return {
      computerId: "",
      machineState: null,
      isCmpInfo: false,
      computers: [
        { id: '1', imageUrl: 'data:image/png;base64,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' },
        { id: '2', imageUrl: 'data:image/png;base64,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' },
      
      ],
    };
  },
  methods: {
    async fetchComputerData() {
      if (!this.computerId) {
        this.errorMessage = "Please enter a computer ID.";
        return;
      }
      
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
      this.computerId = computerId
      if (!this.computerId) {
        this.errorMessage = "Please enter a computer ID.";
        return;
      }
      
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
.main-container {
  display: flex;
  min-height: 81.9vh;
}

/* Sidebar styling */
.sidebar {
  flex-grow: 2;
  background-color: #f4f4f4;
  padding: 10px;
  border-radius: 5px;
}

/* Content area with Flexbox */
.content {
  flex-grow: 10;
  padding: 20px;
  background-color: #f9f9f9;
  border-radius: 5px;
}

.blocks-container {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
  /* justify-content: center; */
  padding-top: 20px;
  /* gap: 20px;  */
}

.search-container {
  position: relative;
  width: 100%; /* Full width of content */
  margin-top: 20px;
}

.search-container input {
  width: 100%;
  padding: 10px;
  padding-right: 35px; /* Space for search icon */
  font-size: 16px;
  border: 1px solid #ccc;
  border-radius: 5px;
  box-sizing: border-box;
}

.search-icon {
  position: absolute;
  right: 15px;
  color: #000000;
  cursor: pointer;
  top: 35%;
  transform: translateY(-50%);
}

.list-icon {
  margin: 30px;
  color: #000000;
  cursor: pointer;
}

.error {
  color: red;
  margin-top: 10px;
  font-size: 0.9em;
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
