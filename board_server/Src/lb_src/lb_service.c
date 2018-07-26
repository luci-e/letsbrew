/*
 * lb_service.c
 *
 *  Created on: 21 Jul 2018
 *      Author: lucie
 */

/**
  ******************************************************************************
  * @file    sensor_service.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   Add a sample service using a vendor specific profile.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "lb_service.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"
#include "connection_config.h"
#include "uart_support.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Private variables ---------------------------------------------------------*/
__IO uint32_t connected = FALSE;
__IO uint8_t set_connectable = 1;
__IO uint16_t connection_handle = 0;
__IO uint8_t notification_enabled = FALSE;

uint16_t tx_handle;
uint16_t rx_handle;

uint16_t lb_service_handle, lb_tx_char_handle, lb_rx_char_handle;
void (*lb_controller_receive_cb)(int, char);


#define RX_TX_BUFFER_LEN 255u

/* Private macros ------------------------------------------------------------*/
#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

/**
 * Set the controller cb to be called when a request is received from the bluetooth
 * @param lb_controller_cb
 */
void set_controller_cb( void(*lb_controller_cb)(int, char) ){
    lb_controller_receive_cb = lb_controller_cb;
}


void lb_make_connection(void)
{
  tBleStatus ret;
    const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B', 'r', 'e', 'w', 'y', 'M', 'c', 'B', 'r', 'e', 'w', 'f', 'a', 'c', 'e'};

    /* disable scan response */
    hci_le_set_scan_resp_data(0,NULL);

    PRINTF("General Discoverable Mode ");
    /*
    Advertising_Event_Type, Adv_Interval_Min, Adv_Interval_Max, Address_Type, Adv_Filter_Policy,
    Local_Name_Length, Local_Name, Service_Uuid_Length, Service_Uuid_List, Slave_Conn_Interval_Min,
    Slave_Conn_Interval_Max
    */
    ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
							   16, local_name, 0, NULL, 0, 0);

    if (ret) {
		PRINTF("Discoverable mode failed.\n");
		Error_Handler();
	}

}


/**
 * @brief  This function is used to receive data related to the sample service
 *         (received over the air from the remote board).
 * @param  data_buffer : pointer to store in received data
 * @param  Nb_bytes : number of bytes to be received
 * @retval None
 */
void lb_receive_data(uint8_t* data_buffer, uint8_t bytes_no) {

    char cmd_str[bytes_no + 1];
    sprintf( cmd_str, "%s", data_buffer);

  for( uint8_t i = 0; i < bytes_no; i++){
      lb_controller_receive_cb(1, data_buffer[i]);
  }
}

/**
 * Transmit the data by writing on the tx buffer
 * @param data_buffer
 */
int lb_transmit_data(char* data_buffer){
    tBleStatus ret;

    ret = aci_gatt_write_charac_value(connection_handle, lb_tx_char_handle, (uint8_t) (strlen(data_buffer) + 1u), (uint8_t*) data_buffer);
    if (ret != BLE_STATUS_SUCCESS) { return BLE_STATUS_ERROR ; }

    return BLE_STATUS_SUCCESS;
}

/**
 * @brief  This function is called when an attribute gets modified
 * @param  handle : handle of the attribute
 * @param  data_length : size of the modified attribute data
 * @param  att_data : pointer to the modified attribute data
 * @retval None
 */
void lb_attribute_modified_cb(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  if(handle == lb_rx_char_handle + 1){
    lb_receive_data(att_data, data_length);
  }
}

/**
 * @brief  This function is called when there is a notification from the sever.
 * @param  attr_handle Handle of the attribute
 * @param  attr_len    Length of attribute value in the notification
 * @param  attr_value  Attribute value in the notification
 * @retval None
 */
void lb_GATT_notification_cb(uint16_t attr_handle, uint8_t attr_len, uint8_t *attr_value)
{
  if (attr_handle == tx_handle+1) {
    lb_receive_data(attr_value, attr_len);
  }
}

/**
 * @brief  Puts the device in connectable mode.
 *         If you want to specify a UUID list in the advertising data, those data can
 *         be specified as a parameter in aci_gap_set_discoverable().
 *         For manufacture data, aci_gap_update_adv_data must be called.
 * @param  None
 * @retval None
 */

/* Ex.:
 *
 *  tBleStatus ret;
 *  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G'};
 *  const uint8_t serviceUUIDList[] = {AD_TYPE_16_BIT_SERV_UUID,0x34,0x12};
 *  const uint8_t manuf_data[] = {4, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 0x05, 0x02, 0x01};
 *
 *  ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
 *                                 8, local_name, 3, serviceUUIDList, 0, 0);
 *  ret = aci_gap_update_adv_data(5, manuf_data);
 *
 */

/**
 * @brief  Make the device connectable advertise its services
 * @param  None
 * @retval None
 */
void lb_set_connectable(void)
{
  tBleStatus ret;

  const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','r','e','w','y','M','c', 'B', 'r', 'e', 'w', 'f', 'a', 'c', 'e' };

  /* disable scan response */
  hci_le_set_scan_resp_data(0,NULL);
  PRINTF("General Discoverable Mode.\n");

  ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while setting discoverable mode (%d)\n", ret);
  }
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
void lb_GAP_connection_complete_cb(uint8_t addr[6], uint16_t handle)
{
  connected = TRUE;
  connection_handle = handle;

  PRINTF("Connected to device:");
  for(uint32_t i = 5; i > 0; i--){
    PRINTF("%02X-", addr[i]);
  }
  PRINTF("%02X\n", addr[0]);
}

/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None
 * @retval None
 */
void lb_GAP_disconnection_complete_cb(void)
{
  connected = FALSE;
  PRINTF("Disconnected\n");
  /* Make the device connectable again. */
  set_connectable = TRUE;
  notification_enabled = FALSE;
}

/**
 * @brief  Read request callback.
 * @param  uint16_t Handle of the attribute
 * @retval None
 */
void lb_read_request_cb(uint16_t handle){
  if(connection_handle != 0){
    aci_gatt_allow_read(connection_handle);
    PRINTF("Read request!\n");
  }
}

/**
 * Add the brewing services to the controller
 */
int lb_add_brewing_service(){
	tBleStatus ret;

	/*
	UUIDs:
	D973F2E0-B19E-11E2-9E96-0800200C9A66
	D973F2E1-B19E-11E2-9E96-0800200C9A66
	D973F2E2-B19E-11E2-9E96-0800200C9A66
	*/

	const uint8_t service_uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
	const uint8_t char_UUID_rx[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
	const uint8_t char_UUID_tx[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};

	const uint8_t name_UUID[2] = { 0x01, 0x29 };

	/* Add the main service */
	ret = aci_gatt_add_serv(UUID_TYPE_128, service_uuid, PRIMARY_SERVICE, 7, &lb_service_handle);
	if (ret != BLE_STATUS_SUCCESS) { goto fail; }


	/* Add the write buffer to reply to the client */
	ret =  aci_gatt_add_char(lb_service_handle, UUID_TYPE_128, char_UUID_tx, RX_TX_BUFFER_LEN, CHAR_PROP_READ | CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
	        MAX_ENCRY_KEY_SIZE, CHAR_VALUE_LEN_VARIABLE, &lb_tx_char_handle);
    if (ret != BLE_STATUS_SUCCESS) { goto fail; }

    /* Add the read buffer to get requests from the client */
	ret =  aci_gatt_add_char(lb_service_handle, UUID_TYPE_128, char_UUID_rx, RX_TX_BUFFER_LEN, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
	        MAX_ENCRY_KEY_SIZE, CHAR_VALUE_LEN_VARIABLE, &lb_rx_char_handle);
    if (ret != BLE_STATUS_SUCCESS) { goto fail; }

//    /* Give a name to the char */
//    char lb_tx_buff_char_name[] = "lb_tx_buffer";
//    uint16_t tx_name_UUID;
//    ret = aci_gatt_add_char_desc(lb_service_handle, lb_tx_char_handle, UUID_TYPE_16, name_UUID, (uint8_t) 15, (uint8_t) strlen(lb_tx_buff_char_name), lb_tx_buff_char_name,
//                        ATTR_PERMISSION_ENCRY_READ, ATTR_ACCESS_READ_ONLY, GATT_DONT_NOTIFY_EVENTS, MAX_ENCRY_KEY_SIZE, CHAR_VALUE_LEN_CONSTANT, &tx_name_UUID);
//    if (ret != BLE_STATUS_SUCCESS) { goto fail; }
//
//    /* Give a name to the char */
//   char lb_rx_buff_char_name[] = "lb_rx_buffer";
//   uint16_t rx_name_UUID;
//   ret = aci_gatt_add_char_desc(lb_service_handle, lb_rx_char_handle, UUID_TYPE_16, name_UUID, (uint8_t) 15, (uint8_t) strlen(lb_rx_buff_char_name), lb_rx_buff_char_name,
//                       ATTR_PERMISSION_ENCRY_READ, ATTR_ACCESS_READ_ONLY, GATT_DONT_NOTIFY_EVENTS, MAX_ENCRY_KEY_SIZE, CHAR_VALUE_LEN_CONSTANT, &rx_name_UUID);
//   if (ret != BLE_STATUS_SUCCESS) { goto fail; }


	PRINTF("Sample Service added.\nTX Char Handle %04X, RX Char Handle %04X\n", lb_tx_char_handle, lb_rx_char_handle);
	return BLE_STATUS_SUCCESS;

	fail:
	PRINTF("Error while adding Sample Service.\n");
	return BLE_STATUS_ERROR ;
}


/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void lb_user_notify(void * pData){
    hci_uart_pckt *hci_pckt = pData;
    /* obtain event packet */
    hci_event_pckt *event_pckt = (hci_event_pckt*) hci_pckt->data;

    if (hci_pckt->type != HCI_EVENT_PKT)
        return;

    switch (event_pckt->evt) {
        case EVT_DISCONN_COMPLETE: {
            lb_GAP_disconnection_complete_cb();
        }
        break;

        case EVT_LE_META_EVENT: {
            evt_le_meta_event *evt = (void *) event_pckt->data;

            switch (evt->subevent) {
                case EVT_LE_CONN_COMPLETE: {
                    evt_le_connection_complete *cc = (void *) evt->data;
                    lb_GAP_connection_complete_cb(cc->peer_bdaddr, cc->handle);
                }
                break;
            }
        }
        break;

        case EVT_VENDOR: {
            evt_blue_aci *blue_evt = (void*) event_pckt->data;
            switch (blue_evt->ecode) {

                case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED: {
                    evt_gatt_attr_modified_IDB05A1 *evt = (evt_gatt_attr_modified_IDB05A1*) blue_evt->data;
                    lb_attribute_modified_cb(evt->attr_handle, evt->data_length, evt->att_data);
                    break;
                }

                case EVT_BLUE_GATT_READ_PERMIT_REQ: {
                    evt_gatt_read_permit_req *pr = (void*) blue_evt->data;
                    lb_read_request_cb(pr->attr_handle);
                    break;
                }

                case EVT_BLUE_GATT_NOTIFICATION: {
                    evt_gatt_attr_notification *evt = (evt_gatt_attr_notification*) blue_evt->data;
                    lb_GATT_notification_cb(evt->attr_handle, evt->event_data_length - 2, evt->attr_value);
                    break;
                }
                break;
            }
        }
        break;
    }
}

#ifdef __cplusplus
 }
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
