
#ifndef __DRV_MOUSE_H
#define __DRV_MOUSE_H

#include "stm32f1xx_hal.h"
#include "drv_mouse.h"



struct Control
{
    uint8_t array[12];
    uint8_t length;

};
extern struct Control * MouseControl ;
extern int32_t PutMouseControl_Init(struct Control * MouseControl) ;

#endif














