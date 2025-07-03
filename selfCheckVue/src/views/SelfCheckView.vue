<template>
    <div class="container">
      <header>
        <h1>RK3588 板卡自检系统</h1>
        <p class="subtitle">飞行前板卡状态检查与诊断</p>
      </header>
  
      <div class="status-card">
        <ProgressBar :progress="progress" :statusText="statusText" />
  
        <CheckItem
          v-for="(check, index) in checks"
          :key="check.id"
          :check="check"
          :isActive="currentCheckIndex === index"
        />
      </div>
  
      <ResultPanel
        :readyStatus="readyStatus"
        :statusDescription="statusDescription"
        :readyStatusDisplay="readyStatusDisplay"
      />
  
      <div class="action-buttons">
        <button class="btn btn-primary" :disabled="isChecking" @click="startSelfTest">
          <i class="fas fa-play"></i> {{ isChecking ? '检测中...' : '开始自检' }}
        </button>
        <button class="btn btn-success" @click="showLogs = true" :disabled="!isFinished">
          <i class="fas fa-file-alt"></i> 查看日志
        </button>
        <button class="btn btn-image" @click="openImageGallery" :disabled="isChecking">
          <i class="fas fa-images"></i> 查看图片
        </button>
        <button class="btn btn-detail" @click="showDetails = !showDetails">
          <i class="fas" :class="showDetails ? 'fa-angle-up' : 'fa-angle-down'"></i>
          {{ showDetails ? '隐藏详情' : '显示详情' }}
        </button>
      </div>
  
      <!-- 图片不可用提示 -->
      <div v-if="imageFetchError && !showImageGallery" class="image-warning">
        <i class="fas fa-exclamation-circle"></i> {{ imageFetchError }}
      </div>
  
      <div v-if="showDetails" class="status-card" style="margin-top: 20px">
        <h3 style="margin-bottom: 15px; color: var(--primary)">详细诊断信息</h3>
        <div v-for="detail in details" :key="detail.id" class="check-detail-info">
          <div style="font-weight: bold; margin-bottom: 8px; display: flex; align-items: center">
            <i :class="detail.icon" style="margin-right: 10px; width: 20px"></i>
            {{ detail.title }}
          </div>
          <div style="padding-left: 30px; margin-bottom: 15px; color: var(--text-secondary)">
            <p>{{ detail.content }}</p>
            <div v-if="detail.tips" class="log-tips">
              <div class="tips-title">故障排除建议</div>
              <p>{{ detail.tips }}</p>
            </div>
          </div>
        </div>
      </div>
  
      <LogModal
        v-if="showLogs"
        :logs="logs"
        :readyStatusDisplay="readyStatusDisplay"
        :images="images"
        @close="showLogs = false"
        @download="downloadLog"
      />
      
      <!-- 图片展示组件 -->
      <ImageGallery
        v-if="showImageGallery"
        :state="{
          showImageGallery,
          images,
          selectedImage,
          isFetchingImages,
          imageFetchError
        }"
        :fetchImages="fetchImages"
        :view="viewImage"
        :download="downloadImage"
        :close="closeImageGallery"
      />
    </div>
  </template>
  
  <script lang="ts">
  import { defineComponent } from 'vue'
  import CheckItem from '@/components/CheckItem.vue'
  import ProgressBar from '@/components/ProgressBar.vue'
  import ResultPanel from '@/components/ResultPanel.vue'
  import LogModal from '@/components/LogModal.vue'
  import ImageGallery from '@/components/ImageGallery.vue' 
  import useSelfCheck from '@/composables/useSelfCheck'
  
  export default defineComponent({
    name: 'App',
    components: {
      CheckItem,
      ProgressBar,
      ResultPanel,
      LogModal,
      ImageGallery,
    },
    setup() {
      const {
        checks,
        details,
        logs,
        progress,
        currentCheckIndex,
        isChecking,
        isFinished,
        showLogs,
        showDetails,
        statusText,
        readyStatus,
        readyStatusDisplay,
        statusDescription,
        startSelfTest,
        downloadLog,
        
        // 图片展示相关
        showImageGallery,
        images,
        selectedImage,
        isFetchingImages,
        imageFetchError,
        openImageGallery,
        closeImageGallery,
        viewImage,
        downloadImage,
        fetchImages
      } = useSelfCheck()
  
      return {
        checks,
        details,
        logs,
        progress,
        currentCheckIndex,
        isChecking,
        isFinished,
        showLogs,
        showDetails,
        statusText,
        readyStatus,
        readyStatusDisplay,
        statusDescription,
        startSelfTest,
        downloadLog,
        
        // 图片展示相关
        showImageGallery,
        images,
        selectedImage,
        isFetchingImages,
        imageFetchError,
        openImageGallery,
        closeImageGallery,
        viewImage,
        downloadImage,
        fetchImages
      }
    },
  })
  </script>
  
  <style>
  /* 全局样式 */
  .container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 20px;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    color: #333;
  }
  
  header {
    text-align: center;
    margin-bottom: 30px;
  }
  
  header h1 {
    font-size: 2.2rem;
    color: #2c3e50;
    margin-bottom: 10px;
  }
  
  .subtitle {
    color: #7f8c8d;
    font-size: 1.2rem;
  }
  
  .status-card {
    background: white;
    border-radius: 10px;
    padding: 25px;
    margin-bottom: 25px;
    box-shadow: 0 4px 15px rgba(0, 0, 0, 0.08);
  }
  
  .action-buttons {
    display: flex;
    gap: 15px;
    margin: 30px 0;
    flex-wrap: wrap;
  }
  
  .btn {
    padding: 12px 25px;
    border-radius: 8px;
    font-size: 1.1rem;
    font-weight: 500;
    cursor: pointer;
    display: flex;
    align-items: center;
    gap: 10px;
    transition: all 0.2s;
  }
  
  .btn-primary {
    background: #3498db;
    color: white;
    border: none;
  }
  
  .btn-primary:hover:not(:disabled) {
    background: #2980b9;
    transform: translateY(-2px);
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  }
  
  .btn-success {
    background: #2ecc71;
    color: white;
    border: none;
  }
  
  .btn-success:hover:not(:disabled) {
    background: #27ae60;
    transform: translateY(-2px);
  }
  
  .btn-image {
    background: #9b59b6;
    color: white;
    border: none;
  }
  
  .btn-image:hover:not(:disabled) {
    background: #8e44ad;
    transform: translateY(-2px);
  }
  
  .btn-detail {
    background: #f1f2f6;
    color: #2c3e50;
    border: 1px solid #dfe4ea;
  }
  
  .btn-detail:hover:not(:disabled) {
    background: #dfe4ea;
  }
  
  .btn:disabled {
    opacity: 0.7;
    cursor: not-allowed;
  }
  
  /* 图片错误提示样式 */
  .image-warning {
    background: #fff8e1;
    border-left: 4px solid #ffc107;
    padding: 10px 15px;
    margin-top: 10px;
    border-radius: 4px;
    display: flex;
    align-items: center;
    animation: fadeIn 0.3s;
  }
  
  .image-warning i {
    color: #ffc107;
    margin-right: 10px;
  }
  
  @keyframes fadeIn {
    from { opacity: 0; transform: translateY(-10px); }
    to { opacity: 1; transform: translateY(0); }
  }
  
  /* 详细诊断信息样式 */
  .check-detail-info {
    margin-bottom: 20px;
    padding-bottom: 15px;
    border-bottom: 1px solid #eee;
  }
  
  .check-detail-info:last-child {
    border-bottom: none;
  }
  
  .log-tips {
    background: #f9f9f9;
    border-left: 3px solid #4da6ff;
    padding: 10px;
    margin-top: 10px;
    border-radius: 0 4px 4px 0;
  }
  
  .tips-title {
    font-weight: bold;
    margin-bottom: 5px;
    color: #2980b9;
  }
  </style>