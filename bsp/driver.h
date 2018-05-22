/*
 * driver.h
 *
 *  Created on: 2018Äê5ÔÂ15ÈÕ
 *      Author: cking
 */

#ifndef BSP_DRIVER_H_
#define BSP_DRIVER_H_

void driver_init_hardware(void);

inline int driver_get_address(void);

void driver_init_system(void);

inline void driver_start_tick();
#endif /* BSP_DRIVER_H_ */
