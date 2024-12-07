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
                  v-model="machineId"
                  type="number"
                  placeholder="Enter Computer ID"
                  @keyup.enter="startFetching"
                  :class="{'input-error ': this.errorMessage.ref}"
                />
                <span class="search-icon" @click="startFetching">🔍</span>
              </div>
          </div>
          <div style="display:flex; justify-content: center;">
            <div v-if="this.errorMessage.ref" class="error">{{ this.errorMessage.ref }}</div>
          </div>
         <div v-if="!isCmpInfo" class="blocks-container">
            <ComputerBlock 
              @fetch-data="startFetching"
              v-for="machine in machines" 
              :key="machine.id" 
              :machineId="machine.id"
              :imageUrl="machine.imageUrl"
            />
          </div>
          <ComputerStatus 
          v-if="this.isCmpInfo && machineState!=null" 
          :machineState="machineState" 
          :interval="intervalRef"
          :key="updateStatuses"  
          :errorMessage="errorMessage"
        />
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
import { ref } from 'vue';
import ComputerBlock from './components/ComputerBlock.vue';
import ComputerStatus from "./components/ComputerStatus.vue";
import { fetchComputerData, startFetchingData, stopFetchingData } from "@/services/apiService";

export default {
  
  components: { Navbar,   Footer, ComputerBlock, ComputerStatus },
  data() {
    return {
      machineId: "",
      machineState: null,
      isCmpInfo: false,
      errorMessage:{ ref: "" },
      intervalRef:{ ref: null },
      updateStatuses:  ref(false),
      machines: [
        { id: '1' },
        { id: '2' },
      ],
    };
  },
  methods: {
    startFetching(machineId) {
      if (typeof(machineId)==="string"){
        console.log(typeof(machineId))
        this.machineId = machineId
      } 
      if (!machineId instanceof PointerEvent){
        this.machineId = machineId
      } 
      if (!this.machineId) {
        this.errorMessage.ref = "Please enter a computer ID.";
        return;
      }
    
      this.updateStatuses = !this.updateStatuses
    
      startFetchingData(
        fetchComputerData,
        this.intervalRef, 
        5000,              
        this.machineId,
        this.updateMachineState,
      );
      this.isCmpInfo = true
    },
    stopFetching() {
      stopFetchingData(this.intervalRef);
    },
    updateMachineState(data, errorMessage) {
      if (data) {
        this.machineState = data;
        this.errorMessage.ref = "";
      } else {
        this.machineState = null;
        this.errorMessage.ref = errorMessage;
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
       this.stopFetching();
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
