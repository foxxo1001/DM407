# 实验 2：DM407_DC_2 — 直流有刷电机编码器测速

[返回实验列表](../../README.md)

在实验 1 的按键 PWM 调速与正反转控制基础上，使用 TIM3 编码器接口采集电机 A/B 相信号，并通过 TIM6 的周期中断计算输出轴转速。

## 硬件配置

| 项目 | 配置 |
|---|---|
| MCU | STM32F407IGT6（LQFP176） |
| 系统时钟 | HSE 8MHz → PLL（M=8, N=168）→ SYSCLK 84MHz |
| 电机 PWM | TIM1 CH1/CH1N |
| 编码器输入 | TIM3 CH1/CH2，四倍频 |
| 测速时基 | TIM6，1ms 更新中断 |

## 引脚分配

| 引脚 | 标签/信号 | 功能 |
|---|---|---|
| PA8 | PWM1_UH | TIM1_CH1，上管 PWM |
| PB13 | PWM1_UL | TIM1_CH1N，下管互补 PWM |
| PF10 | PWM1_SD | 栅极驱动器使能 |
| PE2 | KEY_0 | 正转加速 |
| PE3 | KEY_1 | 反转加速 |
| PE4 | KEY_2 | 停机 |
| PC6 | PM1_ENCA | TIM3_CH1，编码器 A 相 |
| PC7 | PM1_ENCB | TIM3_CH2，编码器 B 相 |

## 测速原理

- TIM3 使用 `TIM_ENCODERMODE_TI12`，ARR=65535，A/B 相双边沿计数。
- TIM6 设置 PSC=83、ARR=999，在 84MHz 定时器时钟下每 1ms 中断一次。
- 每累计 50 次中断读取一次 TIM3 计数，使用 16 位有符号差分得到 `delta`，可处理计数器回绕。
- 默认电机参数为 11 线、四倍频、30:1 减速比，输出轴每圈对应 `11 × 4 × 30 = 1320` 个计数。

```text
speed_now = delta × (60000 / 50) / 1320
```

计算结果经过一阶低通滤波后保存到 `g_motor_speed_rpm`，单位 RPM。若实际电机参数不同，请修改 `main.c` 中的 `ENCODER_LINES`、`ENCODER_MUL` 和 `MOTOR_GEAR_RATIO`。

## 调试观察

当前工程未添加串口或 LCD 显示，可在 Keil 调试器的 Watch 窗口观察：

| 变量 | 含义 |
|---|---|
| `g_encoder_delta` | 最近 50ms 内的编码器计数变化 |
| `g_motor_speed_rpm` | 滤波后的输出轴转速，正负号表示方向 |

## 构建与烧录

1. 用 Keil MDK-ARM 打开 `MDK-ARM/DM407_DC_2.uvprojx`。
2. 编译工程并通过 SWD 下载到开发板。
3. 使用 KEY_0、KEY_1 控制正反转加速，KEY_2 停机，观察测速变量。

CubeMX 配置保存在 `DM407_DC_2.ioc`，用户逻辑位于 `Core/Src/main.c` 的 USER CODE 区域。
