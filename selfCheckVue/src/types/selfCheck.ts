// 自检项类型
export interface CheckItem {
  id: number;
  name: string;
  status: CheckStatus;
  message: string;
  required: boolean;
  tips: string;
}

// 日志条目类型
export interface LogEntry {
  id: number;
  timestamp: string;
  item: string;
  status: CheckStatus;
  statusDisplay: string;
  message: string;
  tips?: string;
}

// 详细诊断信息类型
export interface DetailItem {
  id: number;
  title: string;
  icon: string;
  content: string;
  tips: string;
}

// 自检状态枚举
export type CheckStatus =
  | 'initial'
  | 'pending'
  | 'success'
  | 'warning'
  | 'error';

// 总体状态
export type OverallStatus =
  | 'initial'
  | 'ready'
  | 'not-ready'
  | 'warning';

//图片库状态
export interface ImageGalleryState {
  showImageGallery: boolean;
  images: string[];
  selectedImage: string | null;
  isFetchingImages: boolean;
}