<template>
    <transition
      @enter="enter"
      @after-enter="afterEnter"
      @leave="leave"
    >
      <slot />
    </transition>
  </template>
  
  <script setup lang="ts">
  import { onMounted } from 'vue'
  
  const enter = (el: HTMLElement) => {
    el.style.height = 'auto'
    const height = getComputedStyle(el).height
    el.style.height = '0'
    
    // 触发重绘
    getComputedStyle(el).height
    
    requestAnimationFrame(() => {
      el.style.height = height
      el.style.overflow = 'hidden'
    })
  }
  
  const afterEnter = (el: HTMLElement) => {
    el.style.height = ''
    el.style.overflow = ''
  }
  
  const leave = (el: HTMLElement) => {
    el.style.height = getComputedStyle(el).height
    el.style.overflow = 'hidden'
    
    // 触发重绘
    getComputedStyle(el).height
    
    requestAnimationFrame(() => {
      el.style.height = '0'
    })
  }
  </script>
  
  <style scoped>
  .v-enter-active,
  .v-leave-active {
    transition: height 0.3s ease;
  }
  </style>