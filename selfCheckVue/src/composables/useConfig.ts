import { ref } from 'vue'
import axios from 'axios'
import _ from 'lodash'

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080'

// 深度合并对象（保留原始值）
const deepMerge = (target: any, source: any) => {
  if (_.isPlainObject(target) && _.isPlainObject(source)) {
    const merged = {...target}
    
    for (const key in source) {
      if (!merged.hasOwnProperty(key)) {
        // 新键，直接添加
        merged[key] = source[key]
      } else {
        // 递归合并嵌套对象
        merged[key] = deepMerge(merged[key], source[key])
      }
    }
    
    return merged
  } else if (source === undefined) {
    // 源未定义，保留目标
    return target
  } else {
    // 不是对象或数组，直接覆盖
    return source
  }
}

export function useConfig() {
  // 状态变量
  const configData = ref<any>(null)
  const loading = ref(false)
  const saving = ref(false)
  const error = ref('')
  const successMessage = ref('')
  const changedItems = ref<{path: string, value: any}[]>([])
  const configFiles = ref<string[]>([])
  const currentConfig = ref('main') // 默认为main配置

  // 获取配置文件列表
  const loadConfigFiles = async () => {
    try {
      const response = await axios.get(`${API_BASE_URL}/api/v1/config/files`)
      if (response.data?.files) {
        configFiles.value = response.data.files
      }
    } catch (err: any) {
      error.value = '加载配置文件列表失败: ' + (err.message || '未知错误')
    }
  }

  // 创建一个新的、结构完整的配置对象
  const createDefaultConfig = (configName: string) => {
    if (configName === 'obj_trans') {
      return {
        std: {
          Camera: {
            lng: 0,
            lat: 0,
            h: 0,
            x: 0,
            y: 0,
            z: 0
          },
          target: {
            lng: 0,
            lat: 0
          }
        },
        zmq_addr_recv_det_res: '',
        zmq_addr_send_obj_loc_req: '',
        zmq_addr_send_loc_res: '',
        zmq_addr_recv_plane_res: ''
      }
    } else if (configName === 'main') {
      return {
        camera: {
          fx: 0,
          fy: 0,
          ux: 0,
          uy: 0,
          k1: 0,
          k2: 0,
          p1: 0,
          p2: 0
        },
        vo: {
          vo_match_model_file1: '',
          vo_match_model_file2: '',
          grid_size: 0,
          min_init_point_num: 0,
          kpt_type: 0,
          use_R_from_E: 0,
          parallax_threshold: 0,
          reprojection_threshold: 0,
          match_method: 0,
          zmq_show_map_address: '',
          use_plane_mode: 0,
          max_optical_flow_distance: 0,
          min_required_tracked_mpt_num: 0,
          max_mpt_num: 0
        }
      }
    } else {
      return {}
    }
  }

  // 加载配置
  const loadConfig = async (configName: string = currentConfig.value) => {
    loading.value = true
    error.value = ''
    
    // 1. 先创建一个空对象而不是null
    const defaultConfig = createDefaultConfig(configName)
    configData.value = _.cloneDeep(defaultConfig)
    
    try {
      // 请求JSON格式的配置
      const response = await axios.get(`${API_BASE_URL}/api/v1/config/${configName}/json`)
      
      if (response.data?.config) {
        currentConfig.value = configName
        let loadedConfig = response.data.config
        
        console.log(`[API Response] ${configName}配置:`, loadedConfig)
        
        // 对于obj_trans特殊处理点号分隔键名
        if (configName === 'obj_trans') {
          // 检查是否存在点号分隔键名
          const hasDotKeys = Object.keys(loadedConfig).some(key => key.includes('.'))
          
          if (hasDotKeys) {
            // 创建新的嵌套结构
            const nestedConfig: Record<string, any> = {}
            
            // 处理每个键
            for (const [key, value] of Object.entries(loadedConfig)) {
              if (key.includes('.')) {
                const parts = key.split('.')
                let current = nestedConfig
                
                // 遍历除了最后一部分的所有部分
                for (let i = 0; i < parts.length - 1; i++) {
                  const part = parts[i]
                  
                  if (!current[part]) {
                    current[part] = {}
                  }
                  
                  current = current[part]
                }
                
                // 设置最后一部分的值
                current[parts[parts.length - 1]] = value
              } else {
                // 直接设置非点号键
                nestedConfig[key] = value
              }
            }
            
            loadedConfig = nestedConfig
            console.log(`[Processed] 转换后的${configName}配置:`, loadedConfig)
          }
        }
        
        // 安全地合并配置（深度合并，保留原始值）
        const finalConfig = deepMerge(defaultConfig, loadedConfig)
        configData.value = finalConfig
        console.log(`[Merged] 最终${configName}配置:`, finalConfig)
        
        successMessage.value = `${configName} 配置加载成功`
        setTimeout(() => successMessage.value = '', 3000)
        
        changedItems.value = []
      } else {
        error.value = response.data?.error || '加载配置失败'
      }
    } catch (err: any) {
      console.error('加载配置失败:', err)
      error.value = '加载配置失败: ' + (err.message || '未知错误')
      
      // 显示默认配置结构
      configData.value = defaultConfig
    } finally {
      loading.value = false
    }
  }

  // 处理配置变更
  const handleChange = (path: string, value: any) => {
    console.log(`配置变更: ${path} =`, value)
    
    // 更新变更列表
    const existingIndex = changedItems.value.findIndex(item => item.path === path)
    if (existingIndex > -1) {
      changedItems.value[existingIndex].value = value
    } else {
      changedItems.value.push({ path, value })
    }
    
    // 深层次更新配置数据
    const newData = _.cloneDeep(configData.value)
    
    // 分割路径并设置值
    const parts = path.split('.')
    let current = newData
    
    for (let i = 0; i < parts.length - 1; i++) {
      const part = parts[i]
      
      if (!current[part]) {
        current[part] = {}
      }
      
      current = current[part]
    }
    
    // 设置实际值
    current[parts[parts.length - 1]] = value
    
    // 更新响应式对象
    configData.value = newData
  }

  // 保存配置
  const saveAll = async () => {
    if (!changedItems.value.length) {
      successMessage.value = '没有更改需要保存'
      setTimeout(() => successMessage.value = '', 2000)
      return
    }
    
    saving.value = true
    error.value = ''
    successMessage.value = ''
    
    try {
      console.log('需要保存的变更:', changedItems.value)
      
      // 构建更新对象（扁平化路径）
      const updateData: Record<string, any> = {}
      
      // 处理所有更改项
      changedItems.value.forEach(item => {
        updateData[item.path] = item.value
      })
      
      // 对于obj_trans配置，添加标准结构的所有路径
      if (currentConfig.value === 'obj_trans') {
        // 确保所有可能的路径都存在
        const standardPaths = [
          'std.Camera.lng', 'std.Camera.lat', 'std.Camera.h', 
          'std.Camera.x', 'std.Camera.y', 'std.Camera.z',
          'std.target.lng', 'std.target.lat'
        ]
        
        standardPaths.forEach(path => {
          if (!updateData.hasOwnProperty(path)) {
            // 如果路径不存在，添加当前值
            const value = _.get(configData.value, path)
            if (value !== undefined) {
              updateData[path] = value
            }
          }
        })
      }
      
      // 转换为API需要的格式（对于obj_trans，使用点号路径）
      let formattedData
      
      if (currentConfig.value === 'obj_trans') {
        // 对于obj_trans，保持点号路径
        formattedData = {}
        
        // 直接使用点号分隔的键值对
        Object.entries(updateData).forEach(([key, value]) => {
          // 确保路径使用点号
          formattedData[key] = value
        })
      } else {
        // 其他配置使用嵌套结构
        formattedData = {}
        
        Object.entries(updateData).forEach(([key, value]) => {
          // 分割路径并构建嵌套对象
          const parts = key.split('.')
          let current = formattedData
          
          for (let i = 0; i < parts.length - 1; i++) {
            const part = parts[i]
            
            if (!current[part]) {
              current[part] = {}
            }
            
            current = current[part]
          }
          
          current[parts[parts.length - 1]] = value
        })
      }
      
      console.log('发送到API的配置更新:', formattedData)
      
      // 发送到后端
      const response = await axios.post(
        `${API_BASE_URL}/api/v1/config/${currentConfig.value}`,
        formattedData
      )
      
      console.log('API响应:', response.data)
      
      if (response.data?.status === "success") {
        changedItems.value = []
        successMessage.value = `${currentConfig.value} 配置已保存: ${response.data.message}`
        setTimeout(() => successMessage.value = '', 5000)
        
        // 重新加载配置以确保前端状态更新
        loadConfig(currentConfig.value)
      } else {
        error.value = '保存失败: ' + (response.data?.message || '未知错误')
      }
    } catch (err: any) {
      console.error('保存配置错误:', err)
      error.value = '保存配置失败: ' + (err.message || '未知错误')
    } finally {
      saving.value = false
    }
  }

  return {
    configData,
    loading,
    saving,
    error,
    successMessage,
    changedItems,
    configFiles,
    currentConfig,
    loadConfigFiles,
    loadConfig,
    handleChange,
    saveAll
  }
}