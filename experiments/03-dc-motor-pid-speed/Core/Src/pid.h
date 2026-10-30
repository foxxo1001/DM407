#ifndef __PID_H
#define __PID_H

#include "main.h"

#define KP  10.0f
#define KI  0.0f
#define KD  0.0f
#define SUM_ERROR_MAX   2000.0f

typedef struct
{
		volatile float SetPoint;			/*目标位置*/
    volatile float ActualValue;   /* PID 输出 = PWM 比较值 */
    volatile float SumError;      /* 误差累计 */
    volatile float Proportion;  	/*KP*/
    volatile float Integral; 			/*KI*/
    volatile float Derivative; 		/*KD*/
    volatile float Error;         /* 本次误差 */
    volatile float LastError;     /* 上次误差 */
}PID_TypeDef;
extern PID_TypeDef g_speed_pid;
void pid_init(void);
int32_t increment_pid_ctrl(PID_TypeDef *pid, float feedback);
#endif 
