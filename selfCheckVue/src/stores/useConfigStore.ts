import { defineStore } from 'pinia'
import { ref } from 'vue'
import axios from 'axios'

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080'

export const useConfigStore = defineStore('config', () => {
  const configData = ref<Record<string, any>>({})
  const isLoading = ref(false)
  const errorMessage = ref<string | null>(null)
  
  // 获取结构化配置
  const fetchConfig = async () => {
    isLoading.value = true
    errorMessage.value = null
    try {
      const response = await axios.get(`${API_BASE_URL}/api/v1/config/json`)
      configData.value = response.data
    } catch (error) {
      errorMessage.value = '获取配置失败'
      console.error('获取配置失败:', error)
    } finally {
      isLoading.value = false
    }
  }
  
  // 更新单个配置项
  const updateConfigItem = async (key: string, value: any) => {
    isLoading.value = true
    errorMessage.value = null
    try {
      await axios.post(`${API_BASE_URL}/api/v1/config/update`, {
        key,
        value
      })
      // 更新后刷新配置
      await fetchConfig()
      return true
    } catch (error) {
      errorMessage.value = '更新配置失败'
      console.error('更新配置失败:', error)
      return false
    } finally {
      isLoading.value = false
    }
  }
  
  // 保存整个配置
  const saveFullConfig = async (config: Record<string, any>) => {
    isLoading.value = true
    errorMessage.value = null
    try {
      // 将JSON对象转换为YAML字符串
      let yamlContent = ''
      function convertToYaml(obj: any, indent = 0) {
        for (const key in obj) {
          if (typeof obj[key] === 'object') {
            yamlContent += `${' '.repeat(indent)}${key}:\n`
            convertToYaml(obj[key], indent + 2)
          } else {
            yamlContent += `${' '.repeat(indent)}${key}: ${obj[key]}\n`
          }
        }
      }
      
      convertToYaml(config)
      
      // 发送到后端
      await axios.post(`${API_BASE_URL}/api/v1/config`, yamlContent, {
        headers: {
          'Content-Type': 'text/yaml'
        }
      })
      
      // 重新获取配置确保一致
      await fetchConfig()
      return true
    } catch (error) {
      errorMessage.value = '保存配置失败'
      console.error('保存配置失败:', error)
      return false
    } finally {
      isLoading.value = false
    }
  }
  
  return {
    configData,
    isLoading,
    errorMessage,
    fetchConfig,
    updateConfigItem,
    saveFullConfig
  }
})