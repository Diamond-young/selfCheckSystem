export interface ApiConfigResponse {
    content: string
    lastModified: string
  }
  
  export type ConfigStatus = 'initial' | 'loading' | 'loaded' | 'error'