import { ref } from 'vue'
import axios from 'axios'
import _ from 'lodash'

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080'

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

  // 加载配置
  const loadConfig = async (configName: string = currentConfig.value) => {
    loading.value = true
    error.value = ''
    configData.value = null
    
    try {
      // 请求JSON格式的配置
      const response = await axios.get(`${API_BASE_URL}/api/v1/config/${configName}/json`)
      
      if (response.data?.config) {
        currentConfig.value = configName
        let config = response.data.config
        
        // 特殊处理：obj_trans 配置的点号分隔键名转换为嵌套对象
        if (configName === 'obj_trans') {
          config = convertDotNotationToNested(config);
          
          // 确保必要的嵌套结构存在
          if (!config.std) config.std = {};
          if (!config.std.Camera) config.std.Camera = {};
          if (!config.std.target) config.std.target = {};
        }
        
        // 特殊处理：相机配置
        if (configName === 'main' && config.camera) {
          config.camera = ensureCameraStructure(config.camera);
        }
        
        configData.value = config;
        successMessage.value = `${configName} 配置加载成功`
        setTimeout(() => successMessage.value = '', 3000)
        
        changedItems.value = []
      } else {
        error.value = response.data?.error || '加载配置失败'
      }
    } catch (err: any) {
      error.value = '加载配置失败: ' + (err.message || '未知错误')
      
      // 如果是特殊配置，尝试初始化空结构
      if (configName === 'obj_trans') {
        configData.value = {
          std: {
            Camera: {},
            target: {}
          },
          zmq_addr_recv_det_res: '',
          zmq_addr_send_obj_loc_req: '',
          zmq_addr_send_loc_res: '',
          zmq_addr_recv_plane_res: ''
        }
      } else if (configName === 'main') {
        configData.value = {
          camera: {
            fx: 0,
            fy: 0,
            ux: 0,
            uy: 0,
            k1: 0,
            k2: 0,
            p1: 0,
            p2: 0
          }
        }
      }
    } finally {
      loading.value = false
    }
  }

  // 确保相机配置结构正确
  const ensureCameraStructure = (camera: any) => {
    const defaultCamera = {
      fx: 0,
      fy: 0,
      ux: 0,
      uy: 0,
      k1: 0,
      k2: 0,
      p1: 0,
      p2: 0
    };
    
    return {...defaultCamera, ...camera};
  }

  // 处理配置变更
  const handleChange = (path: string, value: any) => {
    // 使用完整路径
    const existing = changedItems.value.find(item => item.path === path);
    if (existing) {
      existing.value = value;
    } else {
      changedItems.value.push({ path, value });
    }
    
    // 使用 lodash 设置嵌套属性值
    _.set(configData.value, path, value);
  }

  // 保存配置
  const saveAll = async () => {
    saving.value = true;
    error.value = '';
    successMessage.value = '';
    
    try {
        // 构建更改项的对象
        const changes: Record<string, any> = {};
        
        // 处理相机配置的特殊情况
        if (currentConfig.value === 'main' && changedItems.value.some(item => item.path.startsWith('camera'))) {
          // 收集所有相机相关的更改
          const cameraChanges: Record<string, any> = {};
          
          changedItems.value.forEach(item => {
            if (item.path.startsWith('camera.')) {
              const prop = item.path.split('.')[1];
              cameraChanges[prop] = item.value;
            } else {
              _.set(changes, item.path, item.value);
            }
          });
          
          // 将相机更改作为整体发送
          changes.camera = cameraChanges;
        } else {
          // 其他配置正常处理
          changedItems.value.forEach(item => {
            _.set(changes, item.path, item.value);
          });
        }
        
        // 特殊处理：obj_trans 配置需要将嵌套对象转换回点号分隔键名
        let configToSave: any = changes;
        
        if (currentConfig.value === 'obj_trans') {
            configToSave = convertNestedToDotNotation(changes);
        }
        
        // 发送到后端
        const response = await axios.post(
            `${API_BASE_URL}/api/v1/config/${currentConfig.value}`,
            configToSave
        );
        
        if (response.data?.status === "success") {
            changedItems.value = [];
            successMessage.value = `${currentConfig.value} 配置已保存: ${response.data.message}`;
            setTimeout(() => successMessage.value = '', 5000);
            
            // 重新加载配置以确保前端状态更新
            loadConfig(currentConfig.value);
        } else {
            error.value = '保存失败: ' + (response.data?.message || '未知错误');
        }
    } catch (err: any) {
        error.value = '保存配置失败: ' + (err.message || '未知错误');
    } finally {
        saving.value = false;
    }
  }

  // 点号分隔键名转换为嵌套对象
  const convertDotNotationToNested = (config: any) => {
    const result: any = {};
    
    for (const [key, value] of Object.entries(config)) {
      if (key.includes('.')) {
        const parts = key.split('.');
        let current = result;
        
        for (let i = 0; i < parts.length; i++) {
          const part = parts[i];
          
          if (i === parts.length - 1) {
            current[part] = value;
          } else {
            if (!current[part]) {
              current[part] = {};
            }
            current = current[part];
          }
        }
      } else {
        result[key] = value;
      }
    }
    
    return result;
  }

  // 嵌套对象转换为点号分隔键名
  const convertNestedToDotNotation = (config: any) => {
    const result: Record<string, any> = {};
    
    const flatten = (obj: any, prefix = '') => {
      Object.keys(obj).forEach(key => {
        const value = obj[key];
        const fullPath = prefix ? `${prefix}.${key}` : key;
        
        if (value && typeof value === 'object' && !Array.isArray(value)) {
          flatten(value, fullPath);
        } else {
          result[fullPath] = value;
        }
      });
    };
    
    flatten(config);
    return result;
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