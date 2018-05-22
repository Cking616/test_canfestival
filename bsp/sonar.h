/*
 * sonar.h
 *
 *  Created on: 2018Äê5ÔÂ15ÈÕ
 *      Author: cking
 */

#ifndef BSP_SONAR_H_
#define BSP_SONAR_H_

void sonar_init(void);

void sonar_send_pulse(void);

inline int sonar_get_echo_wait(void);

inline int sonar_get_dis(void);

#endif /* BSP_SONAR_H_ */
