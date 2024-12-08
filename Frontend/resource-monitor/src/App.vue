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
              <div class="search-n">
                <input
                  v-model="n"
                  type="number"
                  placeholder="Enter n"
                />
              </div>
              
          </div>
          <div style="display: flex; flex-direction: row; justify-content: center;">
            <button v-if="!this.isBasic"  class="none-active-button" @click="setBasic">Basic Info</button>
            <button v-if="this.isBasic" class="active-button" @click="setBasic">Basic Info</button>
            <button v-if="!this.isCPU" class="none-active-button" @click="setCPU">CPU Info</button>
            <button v-if="this.isCPU" class="active-button" @click="setCPU">CPU Info</button>
            <button v-if="!this.isMemory" class="none-active-button" @click="setMemory">Memory Info</button>
            <button v-if="this.isMemory" class="active-button" @click="setMemory">Memory Info</button>
            <button v-if="!this.isDisk" class="none-active-button" @click="setDisk">Disks Info</button>
            <button v-if="this.isDisk" class="active-button" @click="setDisk">Disks Info</button>
            <button  v-if="!this.isNetwork" class="none-active-button" @click="setNetwork">Network Info</button>
            <button  v-if="this.isNetwork" class="active-button" @click="setNetwork">Network Info</button>
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
          <ComputerStatusBasicInfo 
          v-if="this.isBasic && machineState!=null" 
          :machineState="machineState" 
          :interval="intervalRef"
          :n="n"
          :key="updateStatuses"  
          :errorMessage="errorMessage"
        />
        <ComputerStatusCPU 
          v-if="this.isCPU && machineState!=null" 
          :machineState="machineState" 
           :n="n"
          :interval="intervalRef"
          :key="updateStatuses"  
          :errorMessage="errorMessage"
        />
        <ComputerStatusMemory
          v-if="this.isMemory && machineState!=null" 
          :machineState="machineState" 
           :n="n"
          :interval="intervalRef"
          :key="updateStatuses"  
          :errorMessage="errorMessage"
        />
        <ComputerStatusDisk
          v-if="this.isDisk && machineState!=null" 
          :machineState="machineState" 
           :n="n"
          :interval="intervalRef"
          :key="updateStatuses"  
          :errorMessage="errorMessage"
        />
        <ComputerStatusNetwork
          v-if="this.isNetwork && machineState!=null" 
          :machineState="machineState" 
           :n="n"
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
import ComputerStatusBasicInfo from "./components/ComputerStatusBasicInfo.vue";
import ComputerStatusCPU from "./components/ComputerStatusCPU.vue";
import ComputerStatusMemory from "./components/ComputerStatusMemory.vue";
import ComputerStatusDisk from "./components/ComputerStatusDisk.vue";
import ComputerStatusNetwork from "./components/ComputerStatusNetwork.vue";

import { fetchComputerDataBasic, fetchComputerDataCPU, fetchComputerDataMemory, fetchComputerDisk, fetchComputerNetwork,  startFetchingData } from "@/services/apiService";

export default {
  
  components: { Navbar,   Footer, ComputerBlock, ComputerStatusBasicInfo, ComputerStatusCPU, ComputerStatusMemory, ComputerStatusDisk, ComputerStatusNetwork },
  data() {
    return {
      machineId: "",
      n: "",
      machineState: null,
      isBasic: false,
      isCPU: false,
      isMemory: false,
      isDisk: false,
      isNetwork: false,
      isCmpInfo: false,
      errorMessage:{ ref: "" },
      updateStatuses:  ref(false),
      machines: [
        { id: '1' },
        { id: '2' },
      ],
    };
  },
  methods: {
    startFetching(machineId){
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
      if (!this.n) {
        this.errorMessage.ref = "Please enter a count of notes";
        return;
      }
    
      this.updateStatuses = !this.updateStatuses
      if(this.isBasic){
        console.log(this.n, this.machineId)
        this.startFetchingBasic(this.n, this.machineId)
        this.isCmpInfo = true;
      }else if(this.isCPU){
        this.startFetchingCPU(this.n, this.machineId)
        this.isCmpInfo = true;
      }
      else if(this.isMemory){
        this.startFetchingMemory(this.n, this.machineId)
        this.isCmpInfo = true;
      }
      else if(this.isDisk){
        this.startFetchingDisks(this.n, this.machineId)
      }
      else if(this.isNetwork){
        this.startFetchingNetwork(this.n, this.machineId)
      }
    },
    startFetchingBasic(n, machineId) {
  
      startFetchingData(
        fetchComputerDataBasic,
        this.n,            
        this.machineId,
        this.updateMachineState,
      );
      this.isBasic = true
    },
    startFetchingCPU(n, machineId) {
      startFetchingData(
        fetchComputerDataCPU,   
        this.n,    
        this.machineId,
        this.updateMachineState,
      );
      this.isCPU = true
    },
    startFetchingMemory(n, machineId) { 
      startFetchingData(
        fetchComputerDataMemory,     
        this.n,  
        this.machineId,
        this.updateMachineState,
      );
      this.isMemory = true
    },
    startFetchingDisks(n, machineId) {
      startFetchingData(
        fetchComputerDisk,     
        this.n,     
        this.machineId,
        this.updateMachineState,
      );
      this.isDisk = true
    },
    startFetchingNetwork(n, machineId) {  
      startFetchingData(
        fetchComputerNetwork,       
        this.n,     
        this.machineId,
        this.updateMachineState,
      );
      this.isNetwork = true
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
    },
    setBasic(){
      if(!this.isBasic){
        this.isBasic = true;
      }else{
        this.isBasic = false;
      }
      this.isCPU = false;
      this.isMemory = false;
      this.isDisk = false;
      this.isNetwork = false;
    },
    setCPU(){
      if(!this.isCPU){
        this.isCPU = true;
      }else{
        this.isCPU = false;
      }
      this.isBasic = false;
      this.isMemory = false;
      this.isDisk = false;
      this.isNetwork = false;
    },
    setMemory(){
      if(!this.isMemory){
        this.isMemory = true;
      }else{
        this.isMemory = false;
      }
      this.isBasic = false;
      this.isCPU = false;
      this.isDisk = false;
      this.isNetwork = false;
    },
    setDisk(){
      if(!this.isDisk){
        this.isDisk = true;
      }else{
        this.isDisk = false;
      }
      this.isBasic = false;
      this.isCPU = false;
      this.isMemory = false;
      this.isNetwork = false;
    },
    setNetwork(){
      if(!this.isNetwork){
        this.isNetwork = true;
      }else{
        this.isNetwork = false;
      }
      this.isBasic = false;
      this.isCPU = false;
      this.isMemory = false;
      this.isDisk = false;
    },
  }
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

.search-n input{
  padding: 5px; 
  height: 40px; 
  margin-top: 20px; 
  font-size: 16px; 
  color: #0099ff; 
  border: 1px solid #0000ff; 
  border-radius: 5px; 
  box-sizing: border-box;
}

.search-n input::placeholder {
  color: #0099ff;
  opacity: 1;
}

.search-n input:focus {
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
.none-active-button, .active-button {
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

.active-button {
  background-color: #0099ff; /* Червоний колір для виділення кнопки */
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


.none-active-button:hover {
  background-color: #0077cc; /* Темніший відтінок для кнопки "Start" */
}

.active-button:hover {
  background-color: #0099ff; /* Трохи темніший червоний при наведенні */
  transform: scale(1.05); /* Легке збільшення при наведенні */
}

.active-button:active {
  background-color: #cc0000; /* Темно-червоний при натисканні */
  transform: scale(0.95); /* Легке зменшення при натисканні */
}

.none-active-button:active {
  background-color: #00e600; /* Темно-червоний при натисканні */
  transform: scale(0.95); /* Легке зменшення при натисканні */
}
</style>
