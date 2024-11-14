<template>
  <nav class="navbar">
    <h2><a href="/">Resource Monitor</a></h2>
    <ul class="navbar-links">
      <li><a href="#">Home</a></li>
      <li><a href="#">Dashboard</a></li>
      <li><a href="#">Settings</a></li>
    </ul>
  </nav>
</template>

<script>
export default {
  name: "Navbar",
};
</script>

<style scoped>
.navbar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 20px;
  background-color: #333;
  color: #fff;
}

.navbar h2 {
  margin: 0;
}

.navbar-links {
  list-style: none;
  display: flex;
  gap: 15px;
}

.navbar-links a {
  color: #fff;
  text-decoration: none;
}

.navbar-links a:hover {
  text-decoration: underline;
}
</style>
