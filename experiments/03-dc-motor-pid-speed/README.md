# 实验 3：DM407_DC_3 — 直流电机 PID 闭环调速

[返回实验列表](../../README.md)

在实验 2 编码器测速的基础上加入速度闭环：以 TIM6 1 ms 中断为时基，每 50 ms 采样一次转速，用增量式 PID 计算 PWM 输出，按键调整目标转速（目标符号表示转向）。

## 硬件配置

| 项目 | 配置 |
|---|---|
| MCU | STM32F407IGT6（LQFP176） |
| 系统时钟 | HSE 8MHz → PLL（M=8, N=168）→ SYSCLK 84MHz |
| 电机 PWM | TIM1 CH1/CH1N，ARR=4199（20 kHz），输出限幅 ±4180 |
| 编码器输入 | TIM3 CH1/CH2，四倍频 |
| 控制时基 | TIM6，1ms 更新中断（PSC=83，ARR=999） |

## 引脚分配

| 引脚 | 标签/信号 | 功能 |
|---|---|---|
| PA8 | PWM1_UH | TIM1_CH1，上管 PWM |
| PB13 | PWM1_UL | TIM1_CH1N，下管互补 PWM |
| PF10 | PWM1_SD | 栅极驱动器使能 |
| PE2 | KEY_0 | 目标转速 +30 RPM |
| PE3 | KEY_1 | 目标转速 −30 RPM |
| PE4 | KEY_2 | 停机（目标清零） |
| PC6 | PM1_ENCA | TIM3_CH1，编码器 A 相 |
| PC7 | PM1_ENCB | TIM3_CH2，编码器 B 相 |

## 闭环控制原理

### 转速采样（每 50 ms）

- TIM3 使用 `TIM_ENCODERMODE_TI12`，ARR=65535，16 位有符号差分处理计数器回绕。
- 默认电机参数为 11 线、四倍频、30:1 减速比，输出轴每圈对应 `11 × 4 × 30 = 1320` 个计数：

```text
speed_now = delta × (60000 / 50) / 1320
```

- 结果经一阶低通滤波（新值 0.48 + 旧值 0.52）后保存到 `g_motor_speed_rpm`。若实际电机参数不同，请修改 `main.c` 中的 `ENCODER_LINES`、`ENCODER_MUL` 和 `MOTOR_GEAR_RATIO`。

### 增量式 PID（每 50 ms）

`Core/Src/pid.c` 中的 `increment_pid_ctrl()`：

```text
out = KP × e + KI × Σe + KD × (e − e_last)
```

- 默认参数 `KP = 10`、`KI = 0`、`KD = 0`（见 `Core/Src/pid.h`），积分限幅 `±SUM_ERROR_MAX = 2000`。
- PID 输出先限幅到 `±MOTOR_MAX = 4180`，再由 `Motor_pwm_set()` 决定转向：正值正转（TIM1 CH1），负值反转（TIM1 CH1N），0 停机。
- 目标转速 `g_speed_pid.SetPoint` 的符号即转向，正转加速与反转减速在同一个闭环里完成。

### 目标转速与启停

- KEY_0 / KEY_1 每按一次目标变化 ±30 RPM（`SPEED_STEP`），上限 `±300 RPM`（`SPEED_MAX`）。
- 目标为 0 或按 KEY_2 时停机并调用 `pid_init()` 清空积分与历史误差，避免再次启动时积分残留导致转速过冲。
- `g_run_flag` 允许闭环输出；停机期间 TIM6 中断仍正常运行，只采样不调节。

## 调试观察

当前工程未添加串口或 LCD 显示，可在 Keil 调试器的 Watch 窗口观察：

| 变量 | 含义 |
|---|---|
| `g_motor_speed_rpm` | 滤波后的输出轴转速，正负号表示方向 |
| `g_speed_pid.SetPoint` | 目标转速 |
| `g_motor_pwm` | PID 输出的 PWM 比较值 |
| `g_encoder_delta` | 最近 50ms 内的编码器计数变化 |
| `g_run_flag` | 闭环输出允许标志 |

调参直接修改 `Core/Src/pid.h` 中的 `KP`、`KI`、`KD` 后重新编译；建议先给 KP，转速出现稳态静差时再逐步引入 KI。

## 构建与烧录

1. 用 Keil MDK-ARM 打开 `MDK-ARM/DM407_DC_3.uvprojx`（工程同时附带 STM32CubeIDE `.project` 与 IAR `EWARM/Project.eww` 工程，可任选）。
2. 编译工程并通过 SWD 下载到开发板。
3. KEY_0 升速、KEY_1 降速（目标为负时自动反转），KEY_2 停机，观察 `g_motor_speed_rpm` 是否跟随 `g_speed_pid.SetPoint`。

CubeMX 配置保存在 `DM407_DC_3.ioc`，用户逻辑位于 `Core/Src/main.c` 的 USER CODE 区域，PID 实现在 `Core/Src/pid.c`。
