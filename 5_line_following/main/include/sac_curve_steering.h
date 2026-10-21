#ifndef SAC_CURVE_STEERING_H
#define SAC_CURVE_STEERING_H

/* Error is the unscaled symmetric position (-4..4). PWM is forward-only.
 * Reduce base speed with error; stop the inner wheel rather than commanding
 * an uncalibrated low PWM that may stall it. Tune floor on the actual motors. */
#define SAC_CURVE_MIN_PWM 45.0f
static inline float sac_curve_bound(float value, float low, float high)
{
    return value < low ? low : value > high ? high : value;
}
static inline void sac_curve_steer(float position, float correction,
                                   float cruise, float maximum, int clearing,
                                   float *left, float *right)
{
    float magnitude = position < 0 ? -position : position;
    float base = sac_curve_bound(cruise - 3.0f * magnitude,
                                 SAC_CURVE_MIN_PWM, maximum);
    if (clearing && base > SAC_CURVE_MIN_PWM) base = SAC_CURVE_MIN_PWM;
    *left = sac_curve_bound(base + correction, 0, maximum);
    *right = sac_curve_bound(base - correction, 0, maximum);
    if (*left < SAC_CURVE_MIN_PWM) *left = 0;
    if (*right < SAC_CURVE_MIN_PWM) *right = 0;
}
#endif
