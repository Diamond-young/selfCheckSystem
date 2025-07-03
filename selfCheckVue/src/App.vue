<template>
  <div id="app">
    <div class="app-container">
      <!-- 侧边导航 -->
      <div class="sidebar">
        <div class="logo-area">
          <h1>RK3588 控制台</h1>
          <p>板卡管理与监控</p>
        </div>
        
        <div class="nav-links">
          <div 
            class="nav-item" 
            :class="{ active: activeView === 'selfcheck' }"
            @click="setActiveView('selfcheck')"
          >
            <i class="fas fa-shield-alt"></i> 系统自检
          </div>
          <div 
            class="nav-item" 
            :class="{ active: activeView === 'config' }"
            @click="setActiveView('config')"
          >
            <i class="fas fa-cogs"></i> 配置管理
          </div>
        </div>
      </div>
      
      <!-- 主内容区域 -->
      <div class="main-content">
        <SelfCheckView v-if="activeView === 'selfcheck'" />
        <ConfigView v-else-if="activeView === 'config'" />
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import SelfCheckView from '@/views/SelfCheckView.vue'
import ConfigView from '@/views/ConfigView.vue'

const activeView = ref<'selfcheck' | 'config'>('selfcheck')

const setActiveView = (view: 'selfcheck' | 'config') => {
  activeView.value = view
}
</script>

<style>
/* 添加全局样式 */
#app {
  display: flex;
  min-height: 150vh;
  min-width: 200vh;
}

.app-container {
  display: flex;
  flex: 1;
}

/* 侧边栏样式 */
.sidebar {
  width: 285px;
  background: #2c3e50;
  color: white;
  display: flex;
  flex-direction: column;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
}

.logo-area {
  padding: 25px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

.logo-area h1 {
  font-size: 1.5rem;
  font-weight: 600;
  margin-bottom: 5px;
}

.logo-area p {
  font-size: 0.9rem;
  opacity: 0.8;
}

.nav-links {
  padding: 20px 0;
  flex-grow: 1;
}

.nav-item {
  padding: 14px 25px;
  display: flex;
  align-items: center;
  cursor: pointer;
  transition: all 0.3s;
  border-left: 4px solid transparent;
}

.nav-item i {
  width: 50px;
  font-size: 1.2rem;
  margin-right: 15px;
  opacity: 0.7;
}

.nav-item:hover {
  background: rgba(255, 255, 255, 0.1);
}

.nav-item.active {
  background: rgba(255, 255, 255, 0.15);
  border-left-color: #3498db;
  font-weight: 500;
}

.nav-item.active i {
  opacity: 1;
  color: #3498db;
}

/* 主内容区域 */
.main-content {
  flex-grow: 1;
  padding:20px;
  overflow: auto;
  background: #f5f7fa;
}
</style>