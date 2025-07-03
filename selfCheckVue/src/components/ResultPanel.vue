<template>
  <div class="result-panel">
    <div class="result-header">
      <div class="result-title">飞行准备状态</div>
      <div
        class="overall-status"
        :class="'status-' + readyStatus"
      >
        {{ readyStatusDisplay }}
      </div>
    </div>
    <div class="result-content">
      {{ statusDescription }}
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from 'vue';
import { OverallStatus } from '@/types/selfCheck';

export default defineComponent({
  props: {
    readyStatus: {
      type: String as PropType<OverallStatus>,
      required: true
    },
    statusDescription: {
      type: String,
      required: true
    },
    readyStatusDisplay: {
      type: String,
      required: true
    }
  }
});
</script>

<style scoped>
/* 保留原始样式 */
</style>
