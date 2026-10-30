# DM407 实验代码

本仓库用于保存 DM407 的各个实验。每个实验放在独立目录中，包含各自的源码、驱动库、CubeMX 配置和 Keil 工程。

## 实验列表

| 编号 | 实验 | 工程目录 | 说明 |
|---|---|---|---|
| 01 | 直流电机 PWM 调速与正反转控制 | [01-dc-motor-pwm](experiments/01-dc-motor-pwm/) | [实验说明](experiments/01-dc-motor-pwm/README.md) |
| 02 | 直流有刷电机编码器测速 | [02-dc-motor-encoder-speed](experiments/02-dc-motor-encoder-speed/) | [实验说明](experiments/02-dc-motor-encoder-speed/README.md) |
| 03 | 直流电机 PID 闭环调速 | [03-dc-motor-pid-speed](experiments/03-dc-motor-pid-speed/) | [实验说明](experiments/03-dc-motor-pid-speed/README.md) |

## 目录结构

```text
DM407/
├── README.md
├── .gitignore
└── experiments/
    ├── 01-dc-motor-pwm/
    │   ├── README.md
    │   ├── Core/
    │   ├── Drivers/
    │   ├── MDK-ARM/
    │   └── DM407_DC_H_1.ioc
    ├── 02-dc-motor-encoder-speed/
    │   ├── README.md
    │   ├── Core/
    │   ├── Drivers/
    │   ├── MDK-ARM/
    │   └── DM407_DC_2.ioc
    └── 03-dc-motor-pid-speed/
        ├── README.md
        ├── Core/
        ├── Drivers/
        ├── MDK-ARM/
        └── DM407_DC_3.ioc
```

## 使用方法

进入相应实验目录，按该实验的 README 配置、编译和烧录。实验 1 的 Keil 工程位于：

[`experiments/01-dc-motor-pwm/MDK-ARM/DM407_DC_H_1.uvprojx`](experiments/01-dc-motor-pwm/MDK-ARM/DM407_DC_H_1.uvprojx)

## 添加后续实验

1. 在 `experiments/` 下按 `02-实验英文名`、`03-实验英文名` 的方式新建目录。
2. 将对应的完整工程和实验说明放入该目录，保留工程内部的相对路径。
3. 在上方实验列表中添加条目。
4. 编译产物和个人配置不提交；若工程使用其他输出目录，在 `.gitignore` 中补充对应路径。
