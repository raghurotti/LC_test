/*
 *@file     ql_lpm.h
 *@date     2020-04-01
 *@author   gale
 *@brief    the function is suitable for this product:
*/

#ifndef __QL_LPM_H__
#define __QL_LPM_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Android side wakeup sleep control interface */
typedef enum ANDROID_STATUS_ENUM
{
    ANDROID_STATUS_UNKNOWN = -1, /* unknown status */
    ANDROID_STATUS_WORKING = 0,  /* Android working status */
    ANDROID_STATUS_SLEEP = 1     /* Android sleep status */
} ANDROID_STATUS_E;

/* Set Android status function
 * para:
 * status: android status
 * Return value:
 * 0  --> success
 * -1 --> failed
*/
int32_t ql_set_android_status(ANDROID_STATUS_E status);

/* Get Android status function
 * Return value:
 * android status(ANDROID_STATUS_E)
*/
ANDROID_STATUS_E ql_get_android_status(void);

#ifdef __cplusplus
}
#endif

#endif // __QL_LPM_H__
