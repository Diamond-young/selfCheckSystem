<template>
  <div class="check-progress">
    <div class="progress-header">
      <div class="progress-text">{{ statusText }}</div>
      <div class="progress-percent">{{ progress }}%</div>
    </div>
    <div class="progress-bar">
      <div
        class="progress-fill"
        :style="{ width: progress + '%' }"
      ></div>
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, PropType } from 'vue';

export default defineComponent({
  props: {
    progress: {
      type: Number,
      required: true
    },
    statusText: {
      type: String as PropType<string>,
      required: true
    }
  }
});
</script>

<style scoped>
/* 保留原始样式 */
</style>
