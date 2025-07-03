import { ref, reactive, computed } from 'vue'
import type { CheckItem, LogEntry, DetailItem, CheckStatus, OverallStatus } from '@/types/selfCheck'

export default function useSelfCheck() {
  // API基础路径
  const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080'
  
  // 自检项定义
  const checks = reactive<CheckItem[]>([
    {
      id: 1,
      name: 'SSH连接检测',
      status: 'initial',
      message: '',
      required: true,
      tips: '检查网络连接和板卡电源状态，确认SSH服务已开启。',
    },
    {
      id: 2,
      name: '内存占用检测',
      status: 'initial',
      message: '',
      required: true,
      tips: '关闭不需要的应用程序或服务释放内存资源。',
    },
    {
      id: 3,
      name: 'GPU频率设置',
      status: 'initial',
      message: '',
      required: true,
      tips: '检查GPU驱动状态，重新设置频率参数。',
    },
    {
      id: 4,
      name: '串口设备状态',
      status: 'initial',
      message: '',
      required: true,
      tips: '检查串口设备连接，确认串口权限设置正确。',
    },
    {
      id: 5,
      name: '相机设备状态',
      status: 'initial',
      message: '',
      required: true,
      tips: '检查相机连接，确认相机驱动已正常加载。',
    },
  ])

  // 详细诊断信息
  const details = reactive<DetailItem[]>([
    // ...保持不变...
  ])

  // 状态变量
  const logs = ref<LogEntry[]>([])
  const progress = ref(0)
  const currentCheckIndex = ref(-1)
  const isChecking = ref(false)
  const isFinished = ref(false)
  const showLogs = ref(false)
  const showDetails = ref(false)
  const statusText = ref('准备开始自检')

  // 图片展示相关状态
  const showImageGallery = ref(false)
  const images = ref<string[]>([])
  const selectedImage = ref<string | null>(null)
  const isFetchingImages = ref(false)
  const imageFetchError = ref<string | null>(null)

  // 计算属性
  const readyStatus = computed<OverallStatus>(() => {
    if (!isFinished.value) return 'initial'
    
    // 检查所有必需项是否通过
    const allRequiredPassed = checks.every(
      (check) => !check.required || check.status === 'success' || check.status === 'warning',
    )

    // 检查是否有任何错误项
    const hasErrors = checks.some((check) => check.status === 'error')

    if (hasErrors) return 'not-ready'
    if (!allRequiredPassed) return 'warning'
    return 'ready'
  })

  const readyStatusDisplay = computed(() => {
    switch (readyStatus.value) {
      case 'ready':
        return '准备就绪 - 可以飞行'
      case 'not-ready':
        return '未就绪 - 禁止飞行'
      case 'warning':
        return '警告 - 建议检查'
      default:
        return '未开始检测'
    }
  })

  const statusDescription = computed(() => {
    switch (readyStatus.value) {
      case 'ready':
        return '所有关键系统检测通过，设备满足飞行要求。'
      case 'not-ready':
        return '系统存在严重问题，无法满足飞行要求。请查看详细日志了解问题原因。'
      case 'warning':
        return '系统存在部分问题但仍可飞行，建议查看日志并进行问题排查。'
      default:
        return '请点击"开始自检"按钮，执行完整的系统状态检查。'
    }
  })

  // 辅助函数
  const statusDisplay = (status: CheckStatus): string => {
    switch (status) {
      case 'success':
        return '正常'
      case 'warning':
        return '警告'
      case 'error':
        return '错误'
      default:
        return '待检测'
    }
  }

  // 日志处理
  const addLog = (item: string, status: LogEntry['status'], message: string, tips = ''): void => {
    const timestamp = new Date().toLocaleTimeString()
    const statusDisplay = statusDisplayLog(status)

    logs.value.push({
      id: Date.now(),
      timestamp,
      item,
      status,
      statusDisplay,
      message,
      tips,
    })
  }

  const statusDisplayLog = (status: LogEntry['status']): string => {
    switch (status) {
      case 'success':
        return '正常'
      case 'warning':
        return '警告'
      case 'error':
        return '错误'
      case 'pending':
        return '检测中'
      default:
        return '未开始'
    }
  }

  // 实际调用API的检测函数 - 添加错误处理重试
  const executeCheck = async (check: CheckItem): Promise<void> => {
    let endpoint = ''
    const MAX_RETRIES = 2
    
    // 确定要调用的API端点
    switch (check.id) {
      case 1: // SSH连接检测
        endpoint = '/api/v1/check/ssh'
        break
      case 2: // 内存占用检测
        endpoint = '/api/v1/check/memory'
        break
      case 3: // GPU频点设置
        endpoint = '/api/v1/check/gpu'
        break
      case 4: // 串口设备状态
        endpoint = '/api/v1/check/serial'
        break
      case 5: // 相机设备状态
        endpoint = '/api/v1/check/camera'
        break
      default:
        throw new Error(`未知检测项ID: ${check.id}`)
    }
    
    let retries = 0;
    while (retries < MAX_RETRIES) {
      try {
        const response = await fetch(`${API_BASE_URL}${endpoint}`)
        
        if (!response.ok) {
          const errorText = await response.text()
          throw new Error(`HTTP错误! 状态: ${response.status}, ${errorText}`)
        }
        
        // 解析API响应
        const result = await response.json()
        
        // 更新检测状态
        check.status = result.status
        check.message = result.message

        // 添加日志
        addLog(check.name, check.status, result.message, check.tips)
        
        // 成功，跳出循环
        break;
      } catch (error: any) {
        retries++;
        
        // 如果达到最大重试次数，抛出错误
        if (retries >= MAX_RETRIES) {
          throw new Error(`检测失败(${retries}次尝试): ${error.message || '未知错误'}`)
        } else {
          // 重试前等待一小段时间
          await new Promise(resolve => setTimeout(resolve, 500));
        }
      }
    }
  }

  // 自检流程控制 - 处理每个检测项
  const performNextCheck = async (): Promise<void> => {
    currentCheckIndex.value++

    if (currentCheckIndex.value >= checks.length) {
      // 所有检测完成
      isChecking.value = false
      isFinished.value = true
      statusText.value = '自检完成'

      // 添加完成日志
      addLog('系统', 'success', '自检流程结束')
      
      // 自检完成后自动获取图片列表
      await fetchImages();
      return
    }

    const currentCheck = checks[currentCheckIndex.value]
    statusText.value = `正在检测: ${currentCheck.name}`
    currentCheck.status = 'pending'

    // 更新进度
    progress.value = Math.round((currentCheckIndex.value / checks.length) * 100)

    try {
      // 执行实际检测
      await executeCheck(currentCheck)
      
      // 继续下一个检测
      await performNextCheck()
    } catch (error: any) {  // 添加 any 类型以便访问 message 属性
      console.error('检测过程中发生错误:', error)
      
      // 标记当前检测为错误状态
      currentCheck.status = 'error'
      currentCheck.message = error.message || '未知错误'
      
      // 添加错误日志
      addLog(currentCheck.name, 'error', currentCheck.message, currentCheck.tips)
      
      // 继续下一个检测
      await performNextCheck()
    }
  }

  // 启动自检函数 
  const startSelfTest = async (): Promise<void> => {
    logs.value = []
    checks.forEach((check) => {
      check.status = 'initial'
      check.message = ''
    })
    progress.value = 0
    currentCheckIndex.value = -1
    isChecking.value = true
    isFinished.value = false
    imageFetchError.value = null

    addLog('系统', 'pending', '开始自检流程')
    
    try {
      // 开始实际检测流程
      await performNextCheck()
    } catch (error: any) {  // 添加 any 类型
      console.error('启动自检失败:', error)
      isChecking.value = false
      isFinished.value = true
      statusText.value = '自检启动失败'
      addLog('系统', 'error', `启动自检失败: ${error.message}`)
    }
  }

  // 日志下载 - 添加图片信息
  const downloadLog = (): void => {
    let logContent = `RK3588 板卡自检日志\n`
    logContent += `生成时间: ${new Date().toLocaleString()}\n\n`

    logs.value.forEach((log) => {
      logContent += `[${log.timestamp}] ${log.item}: ${log.statusDisplay} - ${log.message}\n`
      if (log.tips) {
        logContent += `\t建议: ${log.tips}\n`
      }
    })

    // 添加图片信息
    if (images.value.length > 0) {
      logContent += `\n检测到的图片:\n`
      images.value.forEach(image => {
        logContent += `  - ${image}\n`
      })
    } else {
      logContent += `\n未检测到图片\n`
    }

    logContent += `\n最终状态: ${readyStatusDisplay.value}\n`
    logContent += `检测完成时间: ${new Date().toLocaleTimeString()}\n`

    // 创建下载
    const blob = new Blob([logContent], { type: 'text/plain;charset=utf-8' })
    const url = URL.createObjectURL(blob)
    const a = document.createElement('a')
    a.href = url
    a.download = `rk3588_check_log_${new Date().getTime()}.log`
    document.body.appendChild(a)
    a.click()
    document.body.removeChild(a)
    URL.revokeObjectURL(url)
  }

  //=================图片展示功能===================//
  // 获取图片列表 - 添加错误处理
  const fetchImages = async (): Promise<void> => {
    isFetchingImages.value = true;
    imageFetchError.value = null;
    
    try {
      const response = await fetch(`${API_BASE_URL}/api/v1/image`);
      if (!response.ok) {
        const errorText = await response.text();
        throw new Error(`获取图片列表失败(${response.status}): ${response.statusText}\n${errorText}`);
      }
      
      const data = await response.json();
      images.value = data.images || [];
      
      if (images.value.length === 0) {
        imageFetchError.value = '服务器上未检测到图片';
      } else {
        addLog('图片库', 'success', `获取到 ${images.value.length} 张图片`);
      }
    } catch (error: any) {
      console.error('获取图片列表失败:', error);
      const errorMsg = error.message || '未知错误';
      imageFetchError.value = `无法获取图片列表: ${errorMsg}`;
      addLog('图片库', 'error', errorMsg);
    } finally {
      isFetchingImages.value = false;
    }
  };

  // 打开图片库 - 总是刷新列表
  const openImageGallery = async (): Promise<void> => {
    showImageGallery.value = true;
    await fetchImages(); // 总是重新获取最新列表
  };

  // 关闭图片库
  const closeImageGallery = (): void => {
    showImageGallery.value = false;
    selectedImage.value = null;
   
  };

  // 查看单张图片
  const viewImage = (imageName: string): void => {
    // 确保文件名是安全的
    const sanitizedImageName = imageName.replace(/[^a-zA-Z0-9_.-]/g, '');
    selectedImage.value = `${API_BASE_URL}/api/v1/image/${encodeURIComponent(sanitizedImageName)}`;
  };

  // 下载图片 - 添加文件名处理
  const downloadImage = (imageName: string): void => {
    // 确保文件名是安全的
    const sanitizedImageName = imageName.replace(/[^a-zA-Z0-9_.-]/g, '');
    const downloadUrl = `${API_BASE_URL}/api/v1/image/${encodeURIComponent(sanitizedImageName)}`;
    
    const link = document.createElement('a');
    link.href = downloadUrl;
    link.download = sanitizedImageName; // 设置下载的文件名
    link.style.display = 'none';
    
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    
    addLog('图片库', 'success', `已下载图片: ${sanitizedImageName}`);
  };

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
    statusDisplay,
    startSelfTest,
    downloadLog,
    
    // 图片展示相关
    showImageGallery,
    images,
    selectedImage,
    isFetchingImages,
    imageFetchError, // 新增的错误状态
    openImageGallery,
    closeImageGallery,
    viewImage,
    downloadImage,
    fetchImages
  }
}