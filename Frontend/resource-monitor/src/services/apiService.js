import { ref } from 'vue';

// Функція для отримання даних з сервера
export async function fetchComputerDataBasic(n, machineId) {
    try {
        const response = await fetch(`http://127.0.0.1:3333/basic_info/${n}/${machineId}`);
  
      if (!response.ok) {
        throw new Error("Failed to fetch data. Please check the computer ID and try again.");
      }
  
      const data = await response.json();
      return { data, errorMessage: null }; // Повертаємо дані, якщо запит пройшов успішно
    } catch (error) {
      return { data: null, errorMessage: error.message }; // Повертаємо помилку, якщо запит провалився
    }
  }
  
  // Функція для отримання даних з сервера
export async function fetchComputerDataCPU(n, machineId) {
    try {
        const response = await fetch(`http://127.0.0.1:3333/cpu/${n}/${machineId}`);
  
      if (!response.ok) {
        throw new Error("Failed to fetch data. Please check the computer ID and try again.");
      }
  
      const data = await response.json();
      return { data, errorMessage: null }; // Повертаємо дані, якщо запит пройшов успішно
    } catch (error) {
      return { data: null, errorMessage: error.message }; // Повертаємо помилку, якщо запит провалився
    }
  }
  
    // Функція для отримання даних з сервера
export async function fetchComputerDataMemory(n, machineId) {
    try {
        const response = await fetch(`http://127.0.0.1:3333/memory/${n}/${machineId}`);
  
      if (!response.ok) {
        throw new Error("Failed to fetch data. Please check the computer ID and try again.");
      }
  
      const data = await response.json();
      return { data, errorMessage: null }; // Повертаємо дані, якщо запит пройшов успішно
    } catch (error) {
      return { data: null, errorMessage: error.message }; // Повертаємо помилку, якщо запит провалився
    }
  }
  
      // Функція для отримання даних з сервера
export async function fetchComputerDisk(n, machineId) {
    try {
        const response = await fetch(`http://127.0.0.1:3333/disks/${n}/${machineId}`);
  
      if (!response.ok) {
        throw new Error("Failed to fetch data. Please check the computer ID and try again.");
      }
  
      const data = await response.json();
      return { data, errorMessage: null }; // Повертаємо дані, якщо запит пройшов успішно
    } catch (error) {
      return { data: null, errorMessage: error.message }; // Повертаємо помилку, якщо запит провалився
    }
  }

        // Функція для отримання даних з сервера
export async function fetchComputerNetwork(n, machineId) {
    try {
      const response = await fetch(`http://127.0.0.1:3333/network/${n}/${machineId}`);
  
      if (!response.ok) {
        throw new Error("Failed to fetch data. Please check the computer ID and try again.");
      }
  
      const data = await response.json();
      return { data, errorMessage: null }; // Повертаємо дані, якщо запит пройшов успішно
    } catch (error) {
      return { data: null, errorMessage: error.message }; // Повертаємо помилку, якщо запит провалився
    }
  }


  // Функція для запуску та зупинки запитів
  export function startFetchingData(fetchFunction, n, machineId, updateCallback) {    
    fetchFunction(n, machineId).then(({ data, errorMessage }) => {
      updateCallback(data, errorMessage);
    });
  }
  