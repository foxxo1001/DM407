#include "pid.h"

PID_TypeDef g_speed_pid;

void pid_init(void)
{
    g_speed_pid.SetPoint    = 0.0f;
    g_speed_pid.ActualValue = 0.0f;
    g_speed_pid.SumError    = 0.0f;
    g_speed_pid.Error       = 0.0f;
    g_speed_pid.LastError   = 0.0f;
    g_speed_pid.Proportion  = KP;
    g_speed_pid.Integral    = KI;
    g_speed_pid.Derivative  = KD;
}

int32_t increment_pid_ctrl(PID_TypeDef *pid, float feedback)
{
    pid->Error = pid->SetPoint - feedback;

    /* 积分限幅 */
    pid->SumError += pid->Error;
    if (pid->SumError > SUM_ERROR_MAX)  pid->SumError = SUM_ERROR_MAX;
    if (pid->SumError < -SUM_ERROR_MAX) pid->SumError = -SUM_ERROR_MAX;

    pid->ActualValue = pid->Proportion * pid->Error+ pid->Integral* pid->SumError + pid->Derivative * (pid->Error - pid->LastError);

    pid->LastError = pid->Error;

    return (int32_t)pid->ActualValue;
}