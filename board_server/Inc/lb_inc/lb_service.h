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
 * @brief  This function is used to receive data related to the sample service
 *         (received over the air from the remote board).
 * @param  data_buffer : pointer to store in received data
 * @param  Nb_bytes : number of bytes to be received
 * @retval None
 */
void lb_receive_data(uint8_t* data_buffer, uint8_t bytes_no);


/**
 * Transmit the data by writing on the tx buffer
 * @param data_buffer
 */
int lb_transmit_data(char* data_buffer);

/**
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
void lb_attribute_modified_cb(uint16_t handle, uint8_t data_length, uint8_t *att_data);

/**
 * @brief  This function is called when there is a notification from the sever.
 * @param  attr_handle Handle of the attribute
 * @param  attr_len    Length of attribute value in the notification
 * @param  attr_value  Attribute value in the notification
 * @retval None
 */
void lb_GATT_notification_cb(uint16_t attr_handle, uint8_t attr_len, uint8_t *attr_value);

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
 * @brief  Read request callback.
 * @param  uint16_t Handle of the attribute
 * @retval None
 */
void lb_read_request_cb(uint16_t handle);

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
