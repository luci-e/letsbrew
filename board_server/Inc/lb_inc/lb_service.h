/*
 * lb_service.h
 *
 *  Created on: 21 Jul 2018
 *      Author: lucie
 */

#pragma once



#include "bluenrg_gap.h"
#include "string.h"
#include "hci_const.h"
#include "bluenrg_hal_aci.h"
#include "bluenrg_aci_const.h"
#include "hci.h"
#include "hci_le.h"
#include "sm.h"
#include <stdlib.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * Make a new connection
 */
void lb_make_connection(void);

/**
 * @brief  Make the device connectable advertise its services
 * @param  None
 * @retval None
 */
void lb_set_connectable(void);

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
void       lb_GAP_connection_complete_cb(uint8_t addr[6], uint16_t handle);

/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None
 * @retval None
 */
void       lb_GAP_disconnection_complete_cb(void);

/**
 * Add the brewing services to the controller
 */
int lb_add_brewing_service();

/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void       lb_user_notify(void * pData);

#ifdef __cplusplus
}
#endif
