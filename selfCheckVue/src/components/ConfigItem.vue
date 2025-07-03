<template>
    <div class="config-item">
      <label class="item-label">{{ label }}</label>
      <div class="item-control">
        <!-- 根据类型渲染不同的输入控件 -->
        <input 
          v-if="type === 'text' || type === 'number'"
          :type="type"
          v-model="internalValue"
          :step="step"
          @input="handleInput"
          class="input-control"
        />
        
        <select 
          v-else-if="type === 'select'"
          v-model="internalValue"
          @change="handleChange"
          class="select-control"
        >
          <option 
            v-for="option in options" 
            :key="option.value" 
            :value="option.value"
          >
            {{ option.text }}
          </option>
        </select>
        
        <input 
          v-else
          type="text"
          v-model="internalValue"
          @input="handleInput"
          class="input-control"
        />
      </div>
      <div class="item-path">配置路径: {{ path }}</div>
    </div>
  </template>
  
  <script setup lang="ts">
  import { ref, watch } from 'vue'
  
  const props = defineProps({
    label: {
      type: String,
      required: true
    },
    path: {
      type: String,
      required: true
    },
    modelValue: {
      type: [String, Number, Boolean, Object, Array],
      required: true
    },
    type: {
      type: String,
      default: 'text'
    },
    step: {
      type: Number,
      default: undefined
    },
    options: {
      type: Array as () => Array<{value: any, text: string}>,
      default: () => []
    }
  })
  
  const emit = defineEmits(['update:modelValue', 'change'])
  
  // 内部值
  const internalValue = ref(props.modelValue)
  
  // 监听外部值变化
  watch(() => props.modelValue, (newVal) => {
    internalValue.value = newVal
  })
  
  // 处理输入事件
  const handleInput = () => {
    emit('update:modelValue', internalValue.value)
    emit('change', props.path, internalValue.value)
  }
  
  // 处理选择变更事件
  const handleChange = () => {
    emit('update:modelValue', internalValue.value)
    emit('change', props.path, internalValue.value)
  }
  </script>
  
  <style scoped>
  .config-item {
    margin-bottom: 10px;
    padding: 10px;
    border-radius: 8px;
    background-color: #f9f9f9;
    border: 1px solid #eee;
  }
  
  .item-label {
    font-weight: bold;
    display: block;
    margin-bottom: 5px;
    color: #333;
  }
  
  .item-path {
    font-size: 0.85rem;
    color: #666;
    margin-top: 3px;
  }
  
  .input-control, .select-control {
    width: 100%;
    padding: 8px 10px;
    border: 1px solid #ddd;
    border-radius: 4px;
    font-size: 1rem;
  }
  
  .select-control {
    cursor: pointer;
    background-color: white;
  }
  </style>