<template>
  <div class="modal">
    <div class="modal-content">
      <div class="modal-header">
        <div class="modal-title">自检日志记录</div>
        <button class="modal-close" @click="$emit('close')">&times;</button>
      </div>
      <div class="modal-body">
        <div
          v-for="log in logs"
          :key="log.id"
          class="log-item"
        >
          <div class="log-time">
            <i class="fas fa-clock"></i> {{ log.timestamp }}
          </div>
          <div class="log-content">
            <div class="log-name">{{ log.item }}：</div>
            <div class="log-status" :class="'log-' + log.status">
              {{ log.statusDisplay }}
            </div>
          </div>
          <div class="log-detail">{{ log.message }}</div>
          <div v-if="log.tips" class="log-tips">
            <div class="tips-title">故障排除</div>
            <div>{{ log.tips }}</div>
          </div>
        </div>
      </div>
      <div class="modal-footer">
        <button class="btn-download" @click="$emit('download')">
          <i class="fas fa-download"></i> 下载完整日志
        </button>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from 'vue';
import { LogEntry } from '@/types/selfCheck';

export default defineComponent({
  props: {
    logs: {
      type: Array as PropType<LogEntry[]>,
      required: true
    },
    readyStatusDisplay: {
      type: String,
      default: ''
    }
  },
  emits: ['close', 'download']
});
</script>

<style scoped>
/* 保留原始样式 */
</style>
