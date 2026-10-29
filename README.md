# 实验 1：DM407_DC_H_1 — STM32F407 直流电机 PWM 调速控制

基于 STM32F407IGT6 的直流电机调速工程：TIM1 互补 PWM（带死区）驱动栅极驱动器，三个按键控制正转加速、反转加速与停机。

## 硬件配置

| 项目 | 配置 |
|---|---|
| MCU | STM32F407IGT6（LQFP176） |
| 系统时钟 | HSE 8MHz → PLL（M=8, N=168）→ SYSCLK 84MHz |
| 工具链 | Keil MDK-ARM V5.32 |
| 固件库 | STM32Cube FW_F4 V1.28.3 |
| 生成工具 | STM32CubeMX 6.18.1 |

## 引脚分配

| 引脚 | 标签 | 功能 |
|---|---|---|
| PA8 | PWM1_UH | TIM1_CH1，上管 PWM |
| PB13 | PWM1_UL | TIM1_CH1N，下管互补 PWM |
| PF10 | PWM1_SD | 栅极驱动器使能（GPIO 输出，默认低电平关断） |
| PE2 | KEY_0 | 正转加速按键（上拉输入，按下为低） |
| PE3 | KEY_1 | 反转加速按键（上拉输入） |
| PE4 | KEY_2 | 停机按键（上拉输入） |

## PWM 参数（TIM1 CH1/CH1N）

- 定时器时钟 84MHz（APB2 Timer），Prescaler=0，Period=4199 → **PWM 频率 20kHz**
- 死区 DTG=0x0F，CKD=DIV1 → **死区 ≈ 179ns**（15 × t_DTS）
- OC/OCN 极性均为 LOW，OCMode=PWM1，空闲态复位
- OSSR 使能（运行态关闭输出时引脚按死区互补安全态输出）
- Break 禁用

## 功能说明

### 按键操作

| 按键 | 动作 |
|---|---|
| KEY_0 | `motor_pwm += 400`，上限 +4180（正转加速） |
| KEY_1 | `motor_pwm -= 400`，下限 −4180（反转加速） |
| KEY_2 | 归零停机 |

> 长按连续加减（消抖只判按下不判释放，约每 50ms 一档）。

### 电机控制逻辑（`main.c`）

- `Motor_pwm_set(int16_t pwm)`：正负号决定转向，绝对值为 PWM 占空比计数
- `Motor_Control(state, speed)`：
  - `STOP` 或速度 0 → 停机（停 PWM + 拉低 SD）
  - 方向不变 → 仅更新 CCR（比较值钳位到 ARR−15 = 4184）
  - 正反转直接切换 → 先停机并延时 50ms 再重启，防桥臂直通
- 启动流程：设置 CCR → `HAL_TIM_PWM_Start` / `HAL_TIMEx_PWMN_Start` 启动对应半桥 → `PWM1_SD` 拉高使能驱动器

## 目录结构

```
DM407_DC_H_1/
├── Core/
│   ├── Inc/          # main.h / tim.h / gpio.h / stm32f4xx_hal_conf.h
│   └── Src/          # main.c（电机控制逻辑）/ tim.c / gpio.c
├── Drivers/          # STM32F4 HAL 库 + CMSIS
├── MDK-ARM/          # Keil 工程（DM407_DC_H_1.uvprojx）
└── DM407_DC_H_1.ioc  # CubeMX 工程配置
```

## 构建与烧录

1. 用 Keil MDK-ARM 打开 `MDK-ARM/DM407_DC_H_1.uvprojx`
2. 编译，HEX 等产物生成在 `MDK-ARM/DM407_DC_H_1/` 下（编译产物不纳入 Git）
3. SWD 烧录（PA13/PA14），或使用编译生成的 `DM407_DC_H_1.hex`

## 二次开发注意

- 所有用户逻辑均在 `/* USER CODE BEGIN/END */` 区内，CubeMX 重新生成代码不会覆盖
- 修改 PWM 频率/死区：改 `.ioc` 中 TIM1 的 Period / DeadTime 后重新生成
- 若需单击单步（禁止长按连发），在 `Key_Pressed` 中增加等待按键释放逻辑
