<template>
    <div class="config-view">
      <h1 class="page-title">
        <i class="fas fa-cogs"></i> 系统配置管理
      </h1>
      
      <div class="config-description">
        <p>在此编辑系统的YAML配置文件。修改配置后，请保存更改以应用新设置。</p>
        <p class="warning"><i class="fas fa-exclamation-triangle"></i> 注意：不正确的配置可能导致系统故障，请在更改前确认配置格式正确。</p>
      </div>
      
      <ConfigEditor />
    </div>
  </template>
  
  <script setup lang="ts">
  import ConfigEditor from '@/components/ConfigEditor.vue'
  </script>
  
  <style scoped>
  .config-view {
    padding: 20px;
    max-width: 1200px;
    margin: 0 auto;
  }
  
  .page-title {
    font-size: 1.8rem;
    color: #2c3e50;
    margin-bottom: 20px;
    display: flex;
    align-items: center;
  }
  
  .page-title i {
    margin-right: 15px;
    color: #3498db;
  }
  
  .config-description {
    background: white;
    border-radius: 8px;
    padding: 20px;
    margin-bottom: 25px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    line-height: 1.8;
    border-left: 4px solid #3498db;
  }
  
  .warning {
    background: #fff8e1;
    padding: 10px 15px;
    border-radius: 4px;
    border-left: 3px solid #ffc107;
    margin-top: 15px;
  }
  </style>