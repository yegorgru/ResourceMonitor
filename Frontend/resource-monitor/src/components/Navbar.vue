<template>
  <nav class="navbar">
    <div class="navbar-left">
      <a href="/" class="navbar-title">Resource Monitor</a>
    </div>
    <div class="navbar-right">
      <a href="/" class="navbar-link">Home</a>
      <a href="/dashboard" class="navbar-link">Dashboard</a>
      <a href="/settings" class="navbar-link">Settings</a>
    </div>
  </nav>
</template>

<script>
export default {
  name: 'Navbar',
};
</script>

<style scoped>
.navbar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 1.5rem 2rem;
  background-color: #ffffff;
  font-family: "Titillium Web", sans-serif;
  font-weight: 300;
  font-style: normal;
  box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2);
  position: sticky;
  top: 0;
  z-index: 1000; /* Щоб Navbar завжди був зверху */
}

.navbar-left .navbar-title {
  font-size: 1.5rem;
  font-weight: bold;
  color: #0000ff;
  text-decoration: none;
}

.navbar-right {
  display: flex;
  gap: 1.5rem;
  flex-wrap: wrap;
}

.navbar-link {
  font-size: 1.2rem;
  color: #0000ff;
  text-decoration: none;
  transition: color 0.3s;
}

.navbar-link:hover {
  color: #0099ff; /* Темніший відтінок синього при наведенні */
}
</style>
