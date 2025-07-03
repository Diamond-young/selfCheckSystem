<template>
  <div
    class="check-item"
    :class="[check.status, { active: isActive }]"
  >
    <div class="status-icon">
      <i v-if="check.status === 'pending'" class="fas fa-circle-notch fa-spin"></i>
      <i v-else-if="check.status === 'success'" class="fas fa-check"></i>
      <i v-else-if="check.status === 'warning'" class="fas fa-exclamation-triangle"></i>
      <i v-else-if="check.status === 'error'" class="fas fa-times"></i>
      <i v-else class="far fa-circle"></i>
    </div>
    <div class="check-content">
      <div class="check-name">{{ check.name }}</div>
      <div v-if="check.status !== 'pending' && check.status !== 'initial'" class="check-detail">
        {{ check.message }}
      </div>
    </div>
    <div class="check-status" :class="'status-' + check.status">
      {{ statusDisplay(check.status) }}
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from 'vue';
import { CheckStatus } from '@/types/selfCheck';

export default defineComponent({
  props: {
    check: {
      type: Object as PropType<{
        id: number;
        name: string;
        status: CheckStatus;
        message: string;
        required: boolean;
        tips: string;
      }>,
      required: true
    },
    isActive: {
      type: Boolean,
      default: false
    }
  },
  methods: {
    statusDisplay(status: CheckStatus): string {
      switch(status) {
        case 'success': return '正常';
        case 'warning': return '警告';
        case 'error': return '错误';
        case 'pending': return '检测中';
        default: return '待检测';
      }
    }
  }
});
</script>

<style scoped>
/* 保留原始样式 */
</style>
