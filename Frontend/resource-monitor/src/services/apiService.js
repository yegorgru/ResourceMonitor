import { ref } from 'vue';

// Функція для отримання даних з сервера
export async function fetchComputerData(machineId) {
    try {
      const response = await fetch(`http://127.0.0.1:8080/${machineId}`);
  
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
  export function startFetchingData(fetchFunction, intervalRef, intervalTime = 5000, machineId, updateCallback) {
    if (intervalRef.ref) {
        clearInterval(intervalRef.ref);
    }

    intervalRef.ref = setInterval(async () => {
      const { data, errorMessage } = await fetchFunction(machineId);
      updateCallback(data, errorMessage); // Оновлюємо стани у компоненті
    }, intervalTime);
    
    fetchFunction(machineId).then(({ data, errorMessage }) => {
      updateCallback(data, errorMessage);
    });
  }
  
  // Функція для зупинки інтервалу
  export function stopFetchingData(intervalRef) {
    if (intervalRef) {
        clearInterval(intervalRef);
        intervalRef = null;
      }
  }
  