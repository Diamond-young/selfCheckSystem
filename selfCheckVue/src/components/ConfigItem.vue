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

const emit = defineEmits(['update:modelValue', 'change', 'save'])

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

// 点击保存按钮时触发保存事件
const onSave = () => {
  emit('save')
}
</script>

<style scoped>
.config-item {
  display: flex;
  flex-wrap: wrap;
  margin-bottom: 15px;
  padding: 10px;
  border-radius: 6px;
  background: #f8f9fa;
  transition: background 0.2s;
}

.config-item:hover {
  background: #e9ecef;
}

.item-label {
  flex: 0 0 200px;
  font-weight: 500;
  color: #495057;
  padding: 8px 0;
}

.item-control {
  flex: 1;
  min-width: 300px;
}

.input-control, .select-control {
  width: 100%;
  padding: 8px 12px;
  border: 1px solid #ced4da;
  border-radius: 4px;
  font-size: 1rem;
}

.select-control {
  background: white;
  cursor: pointer;
}

.item-path {
  flex: 0 0 100%;
  font-size: 0.85rem;
  color: #6c757d;
  margin-top: 5px;
  padding-top: 5px;
  border-top: 1px dashed #dee2e6;
}

/* 保存按钮样式 */
.save-btn {
  background-color: #4caf50;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 6px 12px;
  cursor: pointer;
  transition: background-color 0.2s;
  font-size: 0.9rem;
  margin-left: 15px;
}

.save-btn:hover {
  background-color: #388e3c;
}

.save-btn i {
  margin-right: 5px;
}
</style>