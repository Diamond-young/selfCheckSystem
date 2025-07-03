<template>
  <div class="config-view">
    <h1 class="page-title">
      <i class="fas fa-cogs"></i> 系统配置管理
    </h1>
    
    <div class="config-header">
      <div class="config-selector">
        <div class="selector-group">
          <label>当前配置：</label>
          <select v-model="currentConfig" @change="loadConfig(currentConfig)">
            <option v-for="file in configFiles" :key="file" :value="file">
              {{ file }}
            </option>
          </select>
        </div>
        
        <div class="config-controls">
          <button @click="loadConfig(currentConfig)" class="btn btn-primary" :disabled="loading">
            <i class="fas fa-sync" :class="{'fa-spin': loading}"></i>
            {{ loading ? '加载中...' : '刷新配置' }}
          </button>
          <button @click="saveAll" class="btn btn-success" :disabled="saving">
            <i class="fas fa-save" :class="{'fa-spin': saving}"></i>
            {{ saving ? '保存中...' : '保存所有更改' }}
          </button>
        </div>
      </div>
    </div>
    
    <div v-if="error" class="error-message">
      <i class="fas fa-exclamation-circle"></i> {{ error }}
    </div>
    
    <div v-if="successMessage" class="success-message">
      <i class="fas fa-check-circle"></i> {{ successMessage }}
    </div>
    
    <!-- 配置文件选择提示 -->
    <div v-if="!configData && !loading" class="no-config">
      <i class="fas fa-info-circle"></i> 请从上方选择配置并加载
    </div>
    
      <!-- main Config 配置部分 -->
  <div v-if="configData && currentConfig === 'main'" class="config-sections">
    <div class="config-section">
      <h2 class="section-title">飞行器定位配置</h2>
      
      <div class="config-group">
        <h3 class="group-title">相机参数</h3>
        
        <ConfigItem 
          label="焦距 X (fx)" 
          path="camera.fx"
          v-model="configData.camera.fx"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="焦距 Y (fy)" 
          path="camera.fy"
          v-model="configData.camera.fy"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="中心点 X (ux)" 
          path="camera.ux"
          v-model="configData.camera.ux"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="中心点 Y (uy)" 
          path="camera.uy"
          v-model="configData.camera.uy"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 k1" 
          path="camera.k1"
          v-model="configData.camera.k1"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 k2" 
          path="camera.k2"
          v-model="configData.camera.k2"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 p1" 
          path="camera.p1"
          v-model="configData.camera.p1"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 p2" 
          path="camera.p2"
          v-model="configData.camera.p2"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">视觉里程计 (VO)</h3>
        
        <ConfigItem 
          label="匹配模型文件 1" 
          path="vo.vo_match_model_file1"
          v-model="configData.vo.vo_match_model_file1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="匹配模型文件 2" 
          path="vo.vo_match_model_file2"
          v-model="configData.vo.vo_match_model_file2"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="网格尺寸" 
          path="vo.grid_size"
          v-model="configData.vo.grid_size"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="最小初始点数" 
          path="vo.min_init_point_num"
          v-model="configData.vo.min_init_point_num"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="关键点类型" 
          path="vo.kpt_type"
          v-model="configData.vo.kpt_type"
          type="select"
          :options="[
            {value: 0, text: '类型0'},
            {value: 1, text: '类型1'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="使用E矩阵旋转" 
          path="vo.use_R_from_E"
          v-model="configData.vo.use_R_from_E"
          type="select"
          :options="[
            {value: 0, text: '禁用'},
            {value: 1, text: '启用'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="视差阈值" 
          path="vo.parallax_threshold"
          v-model="configData.vo.parallax_threshold"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="重投影阈值" 
          path="vo.reprojection_threshold"
          v-model="configData.vo.reprojection_threshold"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="匹配方法" 
          path="vo.match_method"
          v-model="configData.vo.match_method"
          type="select"
          :options="[
            {value: 0, text: '方法0'},
            {value: 1, text: '方法1'},
            {value: 2, text: '方法2'},
            {value: 3, text: '方法3'},
            {value: 4, text: '方法4'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="ZMQ地图地址" 
          path="vo.zmq_show_map_address"
          v-model="configData.vo.zmq_show_map_address"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="使用平面模式" 
          path="vo.use_plane_mode"
          v-model="configData.vo.use_plane_mode"
          type="select"
          :options="[
            {value: 0, text: '禁用'},
            {value: 1, text: '启用'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="最大光流距离" 
          path="vo.max_optical_flow_distance"
          v-model="configData.vo.max_optical_flow_distance"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="最小跟踪点数" 
          path="vo.min_required_tracked_mpt_num"
          v-model="configData.vo.min_required_tracked_mpt_num"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="最大点数" 
          path="vo.max_mpt_num"
          v-model="configData.vo.max_mpt_num"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">网络设置</h3>
        
        <ConfigItem 
          label="调试目录" 
          path="debug_dir"
          v-model="configData.debug_dir"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="状态发布地址" 
          path="show_state_publish_address"
          v-model="configData.show_state_publish_address"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="图像传感器地址" 
          path="img_sensor_address"
          v-model="configData.img_sensor_address"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="外部姿态地址" 
          path="zmq_external_pose_address"
          v-model="configData.zmq_external_pose_address"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">标准差设置</h3>
        
        <ConfigItem 
          label="VO X" 
          path="std.vo_x"
          v-model="configData.std.vo_x"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="VO Y" 
          path="std.vo_y"
          v-model="configData.std.vo_y"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="VO Z" 
          path="std.vo_z"
          v-model="configData.std.vo_z"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="VO RX" 
          path="std.vo_rx"
          v-model="configData.std.vo_rx"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="VO RY" 
          path="std.vo_ry"
          v-model="configData.std.vo_ry"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="VO RZ" 
          path="std.vo_rz"
          v-model="configData.std.vo_rz"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="外部 X" 
          path="std.external_x"
          v-model="configData.std.external_x"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="外部 Y" 
          path="std.external_y"
          v-model="configData.std.external_y"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="外部 Z" 
          path="std.external_z"
          v-model="configData.std.external_z"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="外部 RX" 
          path="std.external_rx"
          v-model="configData.std.external_rx"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="外部 RY" 
          path="std.external_ry"
          v-model="configData.std.external_ry"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="外部 RZ" 
          path="std.external_rz"
          v-model="configData.std.external_rz"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="速度 X" 
          path="std.speed_x"
          v-model="configData.std.speed_x"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="速度 Y" 
          path="std.speed_y"
          v-model="configData.std.speed_y"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="速度 Z" 
          path="std.speed_z"
          v-model="configData.std.speed_z"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="速度 RX" 
          path="std.speed_rx"
          v-model="configData.std.speed_rx"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="速度 RY" 
          path="std.speed_ry"
          v-model="configData.std.speed_ry"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="速度 RZ" 
          path="std.speed_rz"
          v-model="configData.std.speed_rz"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">高级设置</h3>
        
        <ConfigItem 
          label="最小注册关键帧数" 
          path="min_required_success_register_kf_for_align"
          v-model="configData.min_required_success_register_kf_for_align"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="最大对齐点数" 
          path="max_trajectory_align_pt_num"
          v-model="configData.max_trajectory_align_pt_num"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="VO标准差增长比例" 
          path="vo_std_increase_ratio"
          v-model="configData.vo_std_increase_ratio"
          type="number"
          :step="0.01"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="接受匹配阈值" 
          path="threshold_accept_successful_match"
          v-model="configData.threshold_accept_successful_match"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="放弃匹配角度阈值" 
          path="threshold_angle_to_giveup_match"
          v-model="configData.threshold_angle_to_giveup_match"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="初始放弃匹配角度阈值" 
          path="threshold_init_angle_to_giveup_match"
          v-model="configData.threshold_init_angle_to_giveup_match"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="稳定角度持续时间" 
          path="threshold_stable_angle_duration"
          v-model="configData.threshold_stable_angle_duration"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="最大速度" 
          path="max_speed"
          v-model="configData.max_speed"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="图像延迟时间" 
          path="image_delay_time"
          v-model="configData.image_delay_time"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="GPS可视距离阈值" 
          path="threshold_gps_vis_distance"
          v-model="configData.threshold_gps_vis_distance"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="无GPS匹配阈值" 
          path="threshold_no_gps_match"
          v-model="configData.threshold_no_gps_match"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="GPS速度阈值" 
          path="threshold_gps_speed"
          v-model="configData.threshold_gps_speed"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="校正时间阈值" 
          path="threshold_correct_time"
          v-model="configData.threshold_correct_time"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="匹配方法" 
          path="dmatch_method"
          v-model="configData.dmatch_method"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="匹配填充比例" 
          path="dmatch_padding_scale"
          v-model="configData.dmatch_padding_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="匹配地址" 
          path="dmatcher_address"
          v-model="configData.dmatcher_address"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="日志代码" 
          path="log_code"
          v-model="configData.log_code"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="围栏代码" 
          path="fence_code"
          v-model="configData.fence_code"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="GPS内点比例阈值" 
          path="threshold_gps_inlier_scale"
          v-model="configData.threshold_gps_inlier_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="PK值" 
          path="PK_round_num"
          v-model="configData.PK_round_num"
          type="number"
          @change="handleChange"
        />
      </div>
    </div>
  </div>



  <div v-if="configData && currentConfig === 'trans'" class="config-sections">
    <div class="config-section">
      <h2 class="section-title">外场配置</h2>
      
      <div class="field-warning">
        <i class="fas fa-exclamation-triangle"></i> 以下为外场需特别注意修改的配置项
      </div>
      
      <div class="config-group">
        <h3 class="group-title">图像存储设置</h3>
        
        <ConfigItem 
          label="存储图像数据" 
          path="save_video_flag"
          v-model="configData.save_video_flag"
          type="select"
          :options="[
            {value: 0, text: '不存储'},
            {value: 1, text: '存储'}
          ]"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">拒止策略设置</h3>
        
        <ConfigItem 
          label="拒止策略" 
          path="lonlat_std_max_threshold"
          v-model="configData.lonlat_std_max_threshold"
          type="select"
          :options="[
            {value: 1, text: '半拒止'},
            {value: 2000, text: '全拒止'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="起飞点经度" 
          path="init_lon"
          v-model="configData.init_lon"
          type="number"
          :step="0.000001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="起飞点纬度" 
          path="init_lat"
          v-model="configData.init_lat"
          type="number"
          :step="0.000001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="飞行器最大速度 (米/秒)" 
          path="plane_max_speed"
          v-model="configData.plane_max_speed"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">数据获取设置</h3>
        
        <ConfigItem 
          label="图像来源" 
          path="ppdh_img_source"
          v-model="configData.ppdh_img_source"
          type="select"
          :options="[
            {value: 'ZMQ', text: 'ZMQ接收'},
            {value: 'SDI', text: 'SDI获取'},
            {value: 'USB', text: 'USB获取'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="飞参数据来源" 
          path="aircraft_param_source"
          v-model="configData.aircraft_param_source"
          type="select"
          :options="[
            {value: 'SPT', text: '硬件串口'},
            {value: 'ZMQ', text: 'ZMQ接收'}
          ]"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">导航设置</h3>
        
        <ConfigItem 
          label="高精度惯导可用" 
          path="is_INS_ava"
          v-model="configData.is_INS_ava"
          type="select"
          :options="[
            {value: 0, text: '不可用'},
            {value: 1, text: '可用'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="空速管可用" 
          path="is_KSG_ava"
          v-model="configData.is_KSG_ava"
          type="select"
          :options="[
            {value: 0, text: '不可用'},
            {value: 1, text: '可用'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="北向速度 (Vn_KSG)" 
          path="Vn_KSG"
          v-model="configData.Vn_KSG"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="东向速度 (Ve_KSG)" 
          path="Ve_KSG"
          v-model="configData.Ve_KSG"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="平滑最大速度" 
          path="smooth_max_speed"
          v-model="configData.smooth_max_speed"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">时间补偿设置</h3>
        
        <ConfigItem 
          label="图像采集延时补偿 (毫秒)" 
          path="image_dealy_time"
          v-model="configData.image_dealy_time"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">惯导设置</h3>
        
        <ConfigItem 
          label="惯导比例系数" 
          path="ins_std_ratio"
          v-model="configData.ins_std_ratio"
          type="number"
          :step="0.01"
          @change="handleChange"
        />
        
        <Config 
          label="无惯导时伪空惯导系数" 
          path="no_ins_std_ratio"
          v-model="configData.no_ins_std_ratio"
          type="number"
          :step="0.01"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">串口设置</h3>
        
        <ConfigItem 
          label="串口波特率" 
          path="baud_rate"
          v-model="configData.baud_rate"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="field-warning">
        <i class="fas fa-info-circle"></i> 以下为通常不需要修改的配置项
      </div>
      
      <!-- 其余配置项 -->
      <div class="config-group">
        <h3 class="group-title">基础设置</h3>
        
        <ConfigItem 
          label="调试日志路径" 
          path="debug_log_file_path"
          v-model="configData.debug_log_file_path"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="初始位置不确定范围 (米)" 
          path="lonlat_basic_std"
          v-model="configData.lonlat_basic_std"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="起飞点基准高度" 
          path="basic_height"
          v-model="configData.basic_height"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="调试判读频率" 
          path="DEBUG_JUDGE"
          v-model="configData.DEBUG_JUDGE"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="开始计算起飞高度阈值 (米)" 
          path="height_of_begin_cal_take_off_tm"
          v-model="configData.height_of_begin_cal_take_off_tm"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="图像启动高度 (米)" 
          path="send_frame_height_thres"
          v-model="configData.send_frame_height_thres"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">图像处理设置</h3>
        
        <ConfigItem 
          label="使用CLAHE" 
          path="is_CHALE"
          v-model="configData.is_CHALE"
          type="select"
          :options="[
            {value: 0, text: '不使用'},
            {value: 1, text: '使用'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="CLAHE clip limit" 
          path="CLAHE_clip_limit"
          v-model="configData.CLAHE_clip_limit"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="拼接功能" 
          path="mosaic_switch"
          v-model="configData.mosaic_switch"
          type="select"
          :options="[
            {value: 0, text: '关闭'},
            {value: 1, text: '开启'}
          ]"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">云台设置</h3>
        
        <ConfigItem 
          label="云台类型" 
          path="gimbal_class"
          v-model="configData.gimbal_class"
          type="select"
          :options="[
            {value: 0, text: '没有云台'},
            {value: 1, text: '下视增稳航向随动云台'},
            {value: 2, text: '自由旋转云台'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="云台俯仰角 (0.01度)" 
          path="gimbal_pitch"
          v-model="configData.gimbal_pitch"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">网络地址设置</h3>
        
        <ConfigItem 
          label="接收图像地址" 
          path="zmq_addr_recv_frame"
          v-model="configData.zmq_addr_recv_frame"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="接收飞参地址" 
          path="zmq_addr_recv_aircraft"
          v-model="configData.zmq_addr_recv_aircraft"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="发送飞参地址" 
          path="zmq_addr_send_fk_param"
          v-model="configData.zmq_addr_send_fk_param"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="发送图像地址" 
          path="zmq_addr_send_frame"
          v-model="configData.zmq_addr_send_frame"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="接收视觉结果地址" 
          path="zmq_addr_recv_vis_res"
          v-model="configData.zmq_addr_recv_vis_res"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="发送定位结果地址" 
          path="zmq_addr_send_locate_res"
          v-model="configData.zmq_addr_send_locate_res"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">硬件设置</h3>
        
        <ConfigItem 
          label="串口设备" 
          path="serial_device"
          v-model="configData.serial_device"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机设备" 
          path="camera_device"
          v-model="configData.camera_device"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="地图信息地址" 
          path="info_address"
          v-model="configData.info_address"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="视觉结果刷新阈值 (毫秒)" 
          path="vis_res_fresh_tm_thres"
          v-model="configData.vis_res_fresh_tmres"
          type="number"
          @change="handleChange"
        />
      </div>
    </div>
  </div>

  <div v-if="configData && currentConfig === 'DMatcher_plane'" class="config-sections">
    <div class="config-section">
      <h2 class="section-title">DMatcher 配置</h2>
      
      <div class="config-group">
        <h3 class="group-title">路径设置</h3>
        
        <!-- 修复点1：修正v-model绑定 -->
        <ConfigItem 
          label="卫星地图路径" 
          path="DMatcher.sat_map"
          v-model="configData.DMatcher.sat_map" 
          @change="handleChange"
        />
        
        <ConfigItem 
          label="调试日志目录" 
          path="DMatcher.debug_dir"
          v-model="configData.DMatcher.debug_dir"
          @change="handleChange"
        />
        
        <!-- 修复点2：修正v-model绑定 -->
        <ConfigItem 
          label="图像匹配地址" 
          path="DMatcher.zmq_image_match_address"
          v-model="configData.DMatcher.zmq_image_match_address"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="图像特征地址" 
          path="DMatcher.zmq_image_feature_address"
          v-model="configData.DMatcher.zmq_image_feature_address"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">特征设置</h3>
        
        <ConfigItem 
          label="特征类型" 
          path="DMatcher.dfeature_type"
          v-model="configData.DMatcher.dfeature_type"
          type="select"
          :options="[
            {value: 0, text: 'Xfeat'},
            {value: 1, text: 'SP'}
          ]"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">模型设置</h3>
        
        <!-- 修复点3：修正路径名 -->
        <ConfigItem 
          label="模型数量 (N)" 
          path="DMatcher.dfeature_model_path.N"
          v-model="configData.DMatcher.dfeature_model_path.N"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="模型路径 1" 
          path="DMatcher.dfeature_model_path.path_00"
          v-model="configData.DMatcher.dfeature_model_path.path_00"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="模型路径 2" 
          path="DMatcher.dfeature_model_path.path_01"
          v-model="configData.DMatcher.dfeature_model_path.path_01"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">调试设置</h3>
        
        <ConfigItem 
          label="调试显示" 
          path="DMatcher.debug_show"
          v-model="configData.DMatcher.debug_show"
          type="select"
          :options="[{value: 0, text: '禁用'}, {value: 1, text: '启用'}]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="RANSAC 阈值" 
          path="DMatcher.ransac_threshold"
          v-model="configData.DMatcher.ransac_threshold"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="搜索模式阈值" 
          path="DMatcher.threshold_ratio_for_search"
          v-model="configData.DMatcher.threshold_ratio_for_search"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="搜索候选数量" 
          path="DMatcher.search_top_n"
          v-model="configData.DMatcher.search_top_n"
          type="number"
          @change="handleChange"
        />
        
        <!-- 修复点4：修正属性名 -->
        <ConfigItem 
          label="运行设备" 
          path="DMatcher.device"
          v-model="configData.DMatcher.device"
          type="select"
          :options="[
            {value: 0, text: 'NPU'},
            {value: 1, text: 'GPU'}
          ]"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">匹配质量设置</h3>
        
        <ConfigItem 
          label="最大点数" 
          path="DMatcher.MatchQuality.MaxPointNum"
          v-model="configData.DMatcher.MatchQuality.MaxPointNum"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="分布比例阈值" 
          path="DMatcher.MatchQuality.DistributionRatioThreshold"
          v-model="configData.DMatcher.MatchQuality.DistributionRatioThreshold"
          type="number"
          @change="handleChange"
        />
        
        <!-- 修复点5：修正路径 -->
        <ConfigItem 
          label="先验位姿比例" 
          path="DMatcher.MatchQuality.prior_pose_scale"
          v-model="configData.DMatcher.MatchQuality.prior_pose_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="忽略位姿标准差" 
          path="DMatcher.MatchQuality.ignore_pose_std_threshold"
          v-model="configData.DMatcher.MatchQuality.ignore_pose_std_threshold"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配比例"
          path="DMatcher.MatchQuality.second_M_scale"
          v-model="configData.DMatcher.MatchQuality.second_M_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二标准差比例" 
          path="DMatcher.MatchQuality.second_std_scale"
          v-model="configData.DMatcher.MatchQuality.second_std_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配标准差 X" 
          path="DMatcher.MatchQuality.second_match_std_rx"
          v-model="configData.DMatcher.MatchQuality.second_match_std_rx"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配标准差 Y" 
          path="DMatcher.MatchQuality.second_match_std_ry"
          v-model="configData.DMatcher.MatchQuality.second_match_std_ry"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配标准差 Z" 
          path="DMatcher.MatchQuality.second_match_std_rz"
          v-model="configData.DMatcher.MatchQuality.second_match_std_rz"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
      </div>
      
      <div class="config-section">
        <h2 class="section-title">服务器设置</h2>
        
        <ConfigItem 
          label="服务器地址" 
          path="dmatcher_server_address"
          v-model="configData.dmatcher_server_address"
          @change="handleChange"
        />
      </div>
      
      <div class="config-section">
        <h2 class="section-title">安全设置</h2>
        
        <ConfigItem 
          label="日志代码" 
          path="log_code"
          v-model="configData.log_code"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="围栏代码" 
          path="fence_code"
          v-model="configData.fence_code"
          @change="handleChange"
        />
      </div>
    </div>
  </div>

    <!-- DMatcher_target 配置部分 -->
    <div v-if="configData && currentConfig === 'DMatcher_target'" class="config-sections">
    <div class="config-section">
      <h2 class="section-title">DMatcher 配置 (目标定位)</h2>
      
      <div class="config-group">
        <h3 class="group-title">路径设置</h3>
        
        <ConfigItem 
          label="卫星地图路径" 
          path="DMatcher.sat_map"
          v-model="configData.DMatcher.sat_map"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="调试日志目录" 
          path="DMatcher.debug_dir"
          v-model="configData.DMatcher.debug_dir"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="图像匹配地址" 
          path="DMatcher.zmq_image_match_address"
          v-model="configData.DMatcher.zmq_image_match_address"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="图像特征地址" 
          path="DMatcher.zmq_image_feature_address"
          v-model="configData.DMatcher.zmq_image_feature_address"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">特征设置</h3>
        
        <ConfigItem 
          label="特征类型" 
          path="DMatcher.dfeature_type"
          v-model="configData.DMatcher.dfeature_type"
          type="select"
          :options="[
            {value: 0, text: 'Xfeat'},
            {value: 1, text: 'SP'}
          ]"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">模型设置</h3>
        
        <ConfigItem 
          label="模型数量 (N)" 
          path="DMatcher.dfeature_model_path.N"
          v-model="configData.DMatcher.dfeature_model_path.N"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="模型路径 1" 
          path="DMatcher.dfeature_model_path.path_00"
          v-model="configData.DMatcher.dfeature_model_path.path_00"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="模型路径 2" 
          path="DMatcher.dfeature_model_path.path_01"
          v-model="configData.DMatcher.dfeature_model_path.path_01"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">调试设置</h3>
        
        <ConfigItem 
          label="调试显示" 
          path="DMatcher.debug_show"
          v-model="configData.DMatcher.debug_show"
          type="select"
          :options="[{value: 0, text: '禁用'}, {value: 1, text: '启用'}]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="RANSAC 阈值" 
          path="DMatcher.ransac_threshold"
          v-model="configData.DMatcher.ransac_threshold"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="搜索模式阈值" 
          path="DMatcher.threshold_ratio_for_search"
          v-model="configData.DMatcher.threshold_ratio_for_search"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="搜索候选数量" 
          path="DMatcher.search_top_n"
          v-model="configData.DMatcher.search_top_n"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="运行设备" 
          path="DMatcher.device"
          v-model="configData.DMatcher.device"
          type="select"
          :options="[
            {value: 0, text: 'NPU'},
            {value: 1, text: 'GPU'}
          ]"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">匹配质量设置</h3>
        
        <ConfigItem 
          label="最大点数" 
          path="DMatcher.MatchQuality.MaxPointNum"
          v-model="configData.DMatcher.MatchQuality.MaxPointNum"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="分布比例阈值" 
          path="DMatcher.MatchQuality.DistributionRatioThreshold"
          v-model="configData.DMatcher.MatchQuality.DistributionRatioThreshold"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="先验位姿比例" 
          path="DMatcher.MatchQuality.prior_pose_scale"
          v-model="configData.DMatcher.MatchQuality.prior_pose_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="忽略位姿标准差" 
          path="DMatcher.MatchQuality.ignore_pose_std_threshold"
          v-model="configData.DMatcher.MatchQuality.ignore_pose_1_threshold"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配比例" 
          path="DMatcher.MatchQuality.second_M_scale"
          v-model="configData.DMatcher.MatchQuality.second_M_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二标准差比例" 
          path="DMatcher.MatchQuality.second_std_scale"
          v-model="configData.DMatcher.MatchQuality.second_std_scale"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配标准差 X" 
          path="DMatcher.MatchQuality.second_match_std_rx"
          v-model="configData.DMatcher.MatchQuality.second_match_std_rx"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配标准差 Y" 
          path="DMatcher.MatchQuality.second_match_std_ry"
          v-model="configData.DMatcher.MatchQuality.second_match_std_ry"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="第二匹配标准差 Z" 
          path="DMatcher.MatchQuality.second_match_std_rz"
          v-model="configData.DMatcher.MatchQuality.second_match_std_rz"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
      </div>
      
      <div class="config-section">
        <h2 class="section-title">服务器设置</h2>
        
        <ConfigItem 
          label="服务器地址" 
          path="dmatcher_server_address"
          v-model="configData.dmatcher_server_address"
          @change="handleChange"
        />
      </div>
      
      <div class="config-section">
        <h2 class="section-title">安全设置</h2>
        
        <ConfigItem 
          label="日志代码" 
          path="log_code"
          v-model="configData.log_code"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="围栏代码" 
          path="fence_code"
          v-model="configData.fence_code"
          @change="handleChange"
        />
      </div>
    </div>
  </div>



    <!-- obj_loc_config 配置部分 -->
    <div v-if="configData && currentConfig === 'obj_trans'" class="config-sections">
    <div class="config-section">
      <h2 class="section-title">目标定位配置</h2>
      
      <div class="field-warning">
        <i class="fas fa-exclamation-triangle"></i> 外场需特别注意的配置项
      </div>
      
      <div class="config-group">
        <h3 class="group-title">基本设置</h3>
        
        <ConfigItem 
          label="调试判读频率" 
          path="DEBUG_JUDGE"
          v-model="configData.DEBUG_JUDGE"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="高度阈值 (米)" 
          path="height_threshold"
          v-model="configData.height_threshold"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="保存视频" 
          path="save_video_flag"
          v-model="configData.save_video_flag"
          type="select"
          :options="[
            {value: 0, text: '不保存'},
            {value: 1, text: '保存'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机参数源" 
          path="camera_param_source"
          v-model="configData.camera_param_source"
          type="select"
          :options="[
            {value: 0, text: 'ZMQ'},
            {value: 1, text: '配置文件'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="调试日志路径" 
          path="debug_log_file_path"
          v-model="configData.debug_log_file_path"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="基础高度" 
          path="basic_height"
          v-model="configData.basic_height"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">网络设置</h3>
        
        <ConfigItem 
          label="检测结果接收地址" 
          path="zmq_addr_recv_det_res"
          v-model="configData.zmq_addr_recv_det_res"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="定位请求发送地址" 
          path="zmq_addr_send_obj_loc_req"
          v-model="configData.zmq_addr_send_obj_loc_req"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="定位结果发送地址" 
          path="zmq_addr_send_loc_res"
          v-model="configData.zmq_addr_send_loc_res"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="机体定位结果接收地址" 
          path="zmq_addr_recv_plane_res"
          v-model="configData.zmq_addr_recv_plane_res"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">目标参数</h3>
        
        <ConfigItem 
          label="计算间隔(ms)" 
          path="time_for_sleep_per_cal_ms"
          v-model="configData.time_for_sleep_per_cal_ms"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="目标经度" 
          path="Target.lng"
          v-model="configData.Target.lng"
          type="number"
          :step="0.000001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="目标纬度" 
          path="Target.lat"
          v-model="configData.Target.lat"
          type="number"
          :step="0.000001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机设备" 
          path="camera_device"
          v-model="configData.camera_device"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">相机参数</h3>
        
        <ConfigItem 
          label="焦距 X (fx)" 
          path="camera_param_fx"
          v-model="configData.camera_param_fx"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="焦距 Y (fy)" 
          path="camera_param_fy"
          v-model="configData.camera_param_fy"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="中心点 X (cx)" 
          path="camera_param_cx"
          v-model="configData.camera_param_cx"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="中心点 Y (cy)" 
          path="camera_param_cy"
          v-model="configData.camera_param_cy"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 k1" 
          path="camera_param_k1"
          v-model="configData.camera_param_k1"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 k2" 
          path="camera_param_k2"
          v-model="configData.camera_param_k2"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 p1" 
          path="camera_param_p1"
          v-model="configData.camera_param_p1"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="畸变系数 p2" 
          path="camera_param_p2"
          v-model="configData.camera_param_p2"
          type="number"
          :step="0.0001"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">标准差设置</h3>
        
        <ConfigItem 
          label="相机经度标准差" 
          path="std.Camera.lng"
          v-model="configData.std.Camera.lng"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机纬度标准差" 
          path="std.Camera.lat"
          v-model="configData.std.Camera.lat"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机高度标准差" 
          path="std.Camera.h"
          v-model="configData.std.Camera.h"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机姿态X标准差" 
          path="std.Camera.x"
          v-model="configData.std.Camera.x"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机姿态Y标准差" 
          path="std.Camera.y"
          v-model="configData.std.Camera.y"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="相机姿态Z标准差" 
          path="std.Camera.z"
          v-model="configData.std.Camera.z"
          type="number"
          :step="0.1"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="目标经度标准差" 
          path="std.target.lng"
          v-model="configData.std.target.lng"
          type="number"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="目标纬度标准差" 
          path="std.target.lat"
          v-model="configData.std.target.lat"
          type="number"
          @change="handleChange"
        />
      </div>
      
      <div class="config-group">
        <h3 class="group-title">目标定位设置</h3>
        
        <ConfigItem 
          label="投影模式" 
          path="mode"
          v-model="configData.mode"
          type="select"
          :options="[
            {value: 0, text: '卫星投影到航拍'},
            {value: 1, text: '航拍投影到卫星'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="匹配方法" 
          path="method"
          v-model="configData.method"
          type="select"
          :options="[
            {value: 0, text: '相似变化'},
            {value: 1, text: '仿射变化'},
            {value: 2, text: '单应变换'},
            {value: 3, text: '姿态辅助PNP'},
            {value: 4, text: 'PNP'}
          ]"
          @change="handleChange"
        />
        
        <ConfigItem 
          label="目标类别过滤" 
          path="obj_loc_cls_filter"
          v-model="configData.obj_loc_cls_filter"
          @change="handleChange"
        />
      </div>
    </div>
  </div>












    
    
    <!-- 其他配置文件编辑
    <div v-else-if="configData" class="config-sections">
      <div class="config-section">
        <h2 class="section-title">{{ currentConfig }} 配置</h2>
        
        <template v-for="(value, key) in configData" :key="key">
          <div class="config-group">
            <h3 class="group-title">{{ key }}</h3>
            
            <template v-if="typeof value === 'object' && !Array.isArray(value)">
              <template v-for="(subValue, subKey) in value" :key="subKey">
                <ConfigItem 
                  :label="subKey" 
                  :path="`${key}.${subKey}`"
                  v-model="value[subKey]"
                  @change="handleChange"
                />
              </template>
            </template>
            <template v-else>
              <ConfigItem 
                :label="key" 
                :path="key"
                v-model="configData[key]"
                @change="handleChange"
              />
            </template>
          </div>
        </template>
      </div>
    </div> -->
    
    <!-- 加载状态 -->
    <div v-if="loading" class="loading-container">
      <i class="fas fa-spinner fa-spin"></i> 加载配置中...
    </div>
  </div>
</template>

<script setup lang="ts">
import ConfigItem from './ConfigItem.vue'
import { useConfig } from '@/composables/useConfig'

const {
  configData,
  loading,
  saving,
  error,
  successMessage,
  changedItems,
  configFiles,
  currentConfig,
  loadConfigFiles,
  loadConfig,
  handleChange,
  saveAll
} = useConfig()

import { onMounted } from 'vue'
onMounted(() => {
  loadConfigFiles()
  loadConfig('main') // 默认加载配置
})
</script>

<style scoped>
/* 样式保持不变 */
</style>