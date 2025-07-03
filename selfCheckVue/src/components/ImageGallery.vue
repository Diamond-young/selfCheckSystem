<template>
  <div v-if="state.showImageGallery" class="image-modal">
    <div class="modal-overlay" @click="close"></div>
    
    <div class="modal-content">
      <div class="modal-header">
        <h2>系统图片库</h2>
        <button class="close-btn" @click="close">
          <i class="fas fa-times"></i>
        </button>
      </div>
      
      <div class="modal-body">
        <div v-if="state.imageFetchError" class极简注释>
          <i class="fas fa-exclamation-triangle"></i>
          {{ state.imageFetchError }}
        </div>
        
        <div v-if="state.isFetchingImages" class="loading">
          <div class="spinner"></div>
          <p>加载图片中...</p>
        </div>
        
        <div v-else-if="state.images.length === 0" class="empty">
          <i class="fas fa-image"></i>
          <p>没有可用的图片</p>
          <button class="btn" @click="fetchImages">
            <i class="fas fa-sync"></i> 重新加载
          </button>
        </div>
        
        <div v-else-if="!state.selectedImageName" class="image-grid">
          <div 
            v-for="(image, index) in state.images" 
            :key="index" 
            class="image-item"
            @click="selectImage(image)"
          >
            <div class="image-preview">
              <img :src="getImageThumbnail(image)" :alt="image" class="thumbnail">
            </div>
            <div class="image-name">{{ truncateFilename(image) }}</div>
            <button class="download-btn" @click.stop="downloadImage(image)">
              <i class="fas fa-download"></i>
            </button>
          </div>
        </div>
        
        <div v-else class="image-viewer">
          <button class="back-btn" @click="clearSelectedImage">
            <i class="fas fa-arrow-left"></i> 返回列表
          </button>
          
          <div class="full-image">
            <img :src="getImageUrl(state.selectedImageName)" alt="Full size image" class="full-image-preview">
          </div>
          
          <button class="download-full-btn" @click="downloadImage(state.selectedImageName)">
            <i class="fas fa-download"></i> 下载原图
          </button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { reactive, onMounted } from 'vue'

const state = reactive({
  showImageGallery: true,
  images: [] as string[],
  selectedImageName: null as string | null,
  isFetchingImages: false,
  imageFetchError: null as string | null
})

const API_BASE_URL = 'http://localhost:8080'

const getImageUrl = (imageName: string): string => {
  return `${API_BASE_URL}/api/v1/image/${encodeURIComponent(imageName)}`
}

const getImageThumbnail = (imageName: string): string => {
  return getImageUrl(imageName)
}

const truncateFilename = (filename: string, maxLength = 15): string => {
  if (filename.length <= maxLength) return filename
  
  const extensionIndex = filename.lastIndexOf('.')
  if (extensionIndex === -1) {
    return filename.substring(0, maxLength) + '...'
  }
  
  const name = filename.substring(0, extensionIndex)
  const extension = filename.substring(extensionIndex)
  
  if (name.length > maxLength) {
    return name.substring(0, maxLength) + '...' + extension
  }
  
  return filename
}

const downloadImage = (imageName: string): void => {
  const link = document.createElement('a')
  link.href = getImageUrl(imageName)
  link.download = imageName
  link.style.display = 'none'
  document.body.appendChild(link)
  link.click()
  document.body.removeChild(link)
}

const selectImage = (imageName: string) => {
  state.selectedImageName = imageName
}

const clearSelectedImage = () => {
  state.selectedImageName = null
}

const close = () => {
  state.showImageGallery = false

}

const fetchImages = async () => {
  state.isFetchingImages = true
  state.imageFetchError = null
  state.images = []
  
  try {
    const response = await fetch(`${API_BASE_URL}/api/v1/image`)
    if (!response.ok) {
      throw new Error(`API请求失败: ${response.status} ${response.statusText}`)
    }
    
    const data = await response.json()
    if (data.images && Array.isArray(data.images)) {
      state.images = data.images
    } else {
      throw new Error('返回的图片列表格式无效')
    }
  } catch (error) {
    console.error('获取图片列表失败:', error)
    state.imageFetchError = `无法加载图片: ${error.message || '未知错误'}`
  } finally {
    state.isFetchingImages = false
  }
}

onMounted(fetchImages)
</script>

<style scoped>
/* 这里放置之前提供的所有CSS样式 */
.image-modal {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
  background: rgba(0, 0, 0, 0.5);
}

.modal-overlay {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
}

.modal-content {
  position: relative;
  background: white;
  border-radius: 8px;
  width: 90%;
  max-width: 900px;
  max-height: 90vh;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 15px 20px;
  background: #2c3e50;
  color: white;
}

.modal-header h2 {
  margin: 0;
  font-size: 1.5rem;
}

.close-btn {
  background: transparent;
  border: none;
  font-size: 1.5rem;
  cursor: pointer;
  color: white;
  transition: color 0.2s;
}

.close-btn:hover {
  color: #ff6b6b;
}

.modal-body {
  flex: 1;
  overflow: auto;
  padding: 20px;
}

.error-message {
  background: #fff6f6;
  color: #ff6b6b;
  border: 1px solid #ffcccc;
  padding: 10px;
  border-radius: 5px;
  margin-bottom: 20px;
  text-align: center;
}

.error-message i {
  margin-right: 8px;
}

.loading {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 40px ;
}

.spinner {
  width: 40px;
  height: 40px;
  border: 4px solid rgba(0, 0, 0, 0.1);
  border-left-color: #3498db;
  border-radius: 50%;
  animation: spin 1s linear infinite;
  margin-bottom: 15px;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}

.empty {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 40px 0;
  color: #7f8c8d;
}

.empty i {
  font-size: 3rem;
  color: #bdc3c7;
  margin-bottom: 15px;
}

.btn {
  margin-top: 15px;
  padding: 8px 16px;
  background: #3498db;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  display: flex;
  align-items: center;
}

.btn i {
  margin-right: 8px;
}

.image-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
  gap: 20px;
  padding: 10px;
}

.image-item {
  position: relative;
  cursor: pointer;
  border-radius: 8px;
  overflow: hidden;
  box-shadow: 0 3px 10px rgba(0, 0, 0, 0.1);
  transition: all 0.3s ease;
}

.image-item:hover {
  transform: translateY(-5px);
  box-shadow: 0 10px 20px rgba(0, 0, 0, 0.15);
}

.image-preview {
  height: 120px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: #f8f9fa;
}

.thumbnail {
  max-width: 100%;
  max-height: 100%;
  object-fit: contain;
}

.image-name {
  padding: 10px;
  font-size: 0.85rem;
  text-align: center;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  background: white;
}

.download-btn {
  position: absolute;
  top: 10px;
  right: 10px;
  background: rgba(255, 255, 255, 0.7);
  color: #2c3e50;
  border: none;
  border-radius: 50%;
  width: 30px;
  height: 30px;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: all 0.2s;
  box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
}

.download-btn:hover {
  background: white;
  transform: scale(1.1);
}

.image-viewer {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 20px;
}

.back-btn {
  align-self: flex-start;
  margin-bottom: 20px;
  padding: 8px 16px;
  background: #7f8c8d;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  display: flex;
  align-items: center;
  transition: background 0.2s;
}

.back-btn:hover {
  background: #95a5a6;
}

.back-btn i {
  margin-right: 8px;
}

.full-image {
  max-height: 60vh;
  display: flex;
  justify-content: center;
  margin: 10px 0;
}

.full-image-preview {
  max-width: 100%;
  max-height: 100%;
  object-fit: contain;
  border-radius: 4px;
  box-shadow: 0 5px 15px rgba(0, 0, 0, 0.1);
}

.download-full-btn {
  margin-top: 20px;
  padding: 10px 20px;
  background: #27ae60;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  display: flex;
  align-items: center;
  transition: background 0.2s;
  font-size: 1.1rem;
}

.download-full-btn:hover {
  background: #2ecc71;
}

.download-full-btn i {
  margin-right: 8px;
}

/* 响应式设计 */
@media (max-width: 768px) {
  .image-grid {
    grid-template-columns: repeat(auto-fill, minmax(120px, 1fr));
  }
  
  .image-preview {
    height: 100px;
  }
}

@media (max-width: 480px) {
  .modal-content {
    width: 95%;
  }
  
  .image-grid {
    grid-template-columns: repeat(auto-fill, minmax(100px, 1fr));
    gap: 10px;
  }
}
</style>