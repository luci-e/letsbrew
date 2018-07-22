/*
 * lb_service.h
 *
 *  Created on: 21 Jul 2018
 *      Author: lucie
 */

#pragma once



#ifdef __cplusplus
 extern "C" {
#endif

#include "bluenrg_gap.h"
#include "string.h"
#include "hci_const.h"
#include "bluenrg_hal_aci.h"
#include "bluenrg_aci_const.h"
#include "hci.h"
#include "hci_le.h"
#include "sm.h"
#include <stdlib.h>

void       setConnectable(void);
void       GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle);
void       GAP_DisconnectionComplete_CB(void);
void       user_notify(void * pData);

#ifdef __cplusplus
}
#endif
