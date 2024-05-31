/*
 * Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

#include <stdbool.h>
#include <stddef.h>
#include "wifi_cert_commands.h"

typedef enum
{
	SIGMADUT_INTERFACE,
	SIGMADUT_DHCP,
	SIGMADUT_IPADDRESS,
	SIGMADUT_GATEWAY,
	SIGMADUT_NETMASK,
	SIGMADUT_PRIMARY_DNS,
	SIGMADUT_SECONDARY_DNS,
	SIGMADUT_SSID,
	SIGMADUT_USERNAME,
	SIGMADUT_PASSWORD,
	SIGMADUT_TRUSTEDROOTCA,
	SIGMADUT_CLIENTCERT,
	SIGMADUT_SECURITY_TYPE,
	SIGMADUT_KEYMGMT_TYPE,
	SIGMADUT_INNEREAP,
	SIGMADUT_PEAPVERSION,
	SIGMADUT_ENCRYPTION_TYPE,
	SIGMADUT_PASSPHRASE,
	SIGMADUT_PMF,
	SIGMADUT_BSSID,
	SIGMADUT_CHANNEL,
	SIGMADUT_PWRSAVE
}SIGMADUT_CONFIG_DATA_STR_TYPE_T;


typedef enum
{
	SIGMADUT_TRAFFIC_PAYLOAD_SIZE,
	SIGMADUT_TRAFFIC_FRAME_RATE,
	SIGMADUT_TRAFFIC_DURATION,
	SIGMADUT_TRAFFIC_STOP_TIME,
	SIGMADUT_TRAFFIC_FRAME_SENT,
	SIGMADUT_TRAFFIC_FRAME_RECVD,
	SIGMADUT_TRAFFIC_BYTES_SENT,
	SIGMADUT_TRAFFIC_BYTES_RECVD,
	SIGMADUT_TRAFFIC_OUT_OF_SEQ
}SIGMADUT_TRAFFIC_DATA_INT_TYPE_T;

typedef enum
{
    SIGMADUT_NDPPAGINGIND,
    SIGMADUT_RESPPMMODE,
    SIGMADUT_NEGOTIATIONTYPE,
    SIGMADUT_TWT_SETUP,
    SIGMADUT_SETUPCOMMAND,
    SIGMADUT_TWT_TRIGGER,
    SIGMADUT_IMPLICIT,
    SIGMADUT_FLOWTYPE,
    SIGMADUT_WAKEINTERVALEXP,
    SIGMADUT_PROTECTION,
    SIGMADUT_NOMINALMINWAKEDUR,
    SIGMADUT_WAKEINTERVALMANTISSA,
    SIGMADUT_TWT_CHANNEL,
    SIGMADUT_FLOWID,
    SIGMADUT_RESUME_DURATION,
    SIGMADUT_BTWT_ID,
    SIGMADUT_PPDUTXTYPE,
    SIGMADUT_RUALLOCTONE,
}SIGMADUT_TWT_INT_TYPE_T;

typedef enum
{
    SIGMADUT_LTF,
    SIGMADUT_GI,
}SIGMADUT_LTF_GI_STR_TYPE_T;

typedef enum
{
    SIGMADUT_TXNSTS,
    SIGMADUT_CHNLWIDTH,
    SIGMADUT_ULMUDISABLE,
    SIGMADUT_ULMUDATADISABLE,
}SIGMADUT_TX_OMI_INT_TYPE_T;

typedef enum
{
    SIGMADUT_MBO_OPCLASS,
    SIGMADUT_MBO_CHANNEL,
    SIGMADUT_MBO_PREFERENCE,
    SIGMADUT_MBO_REASON,
}SIGMADUT_MBO_INT_TYPE_T;

typedef enum
{
    SIGMADUT_DATE,
    SIGMADUT_MONTH,
    SIGMADUT_YEAR,
    SIGMADUT_HOURS,
    SIGMADUT_MINUTES,
    SIGMADUT_SECONDS
}SIGMADUT_DATE_TIME_INT_TYPE_T;

typedef enum
{
	SIGMADUT_TRAFFIC_SRC_IPADDRESS,
	SIGMADUT_TRAFFIC_DST_IPADDRESS
}SIGMADUT_TRAFFIC_CONFIG_STR_TYPE_T;

typedef enum
{
	SIGMADUT_TRAFFIC_SRC_PORT,
	SIGMADUT_TRAFFIC_DST_PORT
}SIGMADUT_TRAFFIC_DATA_UINT16_TYPE_T;

/****************************************************************
 *   Function Prototypes
 ***************************************************************/


/** This function sets the traffic class for a given stream id
 *
 * @param stream_id   : The stream id of traffic
 * @param qos_cat     : Qos category of the traffic
 *
 *******************************************************************************/
void sigmadut_set_traffic_class ( int stream_id, qos_access_category_t qos_cat);


/** This function gets the SW version
 *
 * @param buf         : The pointer to a buffer to copy the SW version.
 * @param buflen      : Length of the buffer
 *
 *******************************************************************************/
void sigmadut_get_sw_version( char *buf, uint16_t buflen);


/** This function sets the destination port for a traffic stream
 *
 * @param stream_index   : The stream index of traffic
 * @param port           : The destination port of the traffic
 *
 *******************************************************************************/
void sigmadut_set_traffic_dst_port ( int stream_index, uint16_t port);


/** This function sets the source port for a traffic stream
 *
 * @param stream_index   : The stream index of traffic
 * @param port           : The source port of the traffic
 *
 *******************************************************************************/
void sigmadut_set_traffic_src_port ( int stream_index, uint16_t port);


/** This function sets the destination IPv4 address for a traffic stream
 *
 * @param stream_id   : The stream index of traffic
 * @param ip_addr     : IPv4 address in dot notation
 *
 *******************************************************************************/
void sigmadut_set_traffic_dst_ipaddress(int stream_id, char *ip_addr);

/** This function sets EAP type
 *
 *  @param eap_type  : set supplicant EAP type
 */
void sigmadut_set_eap_type ( wpa2_ent_eap_type_t eap_type);

/** This function gets EAP type
 *
 *  @return wpa2_ent_eap_type_t  : get supplicant EAP type
 */
wpa2_ent_eap_type_t sigmadut_get_eap_type(void);

/** This function sets the source IPv4 address for a traffic stream
 *
 * @param stream_id   : The stream index of traffic
 * @param ip_addr     : IPv4 address in dot notation
 *
 *******************************************************************************/
void sigmadut_set_traffic_src_ipaddress(int stream_id, char *ip_addr);


/** This function sets the direction of a traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @param dir         : The direction of the traffic
 *
 *******************************************************************************/
void sigmadut_set_traffic_direction (int stream_id, traffic_direction_t dir);

/** This function sets the profile for a traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @param profile     : profile of the traffic
 *
 *******************************************************************************/
void sigmadut_set_traffic_profile(int stream_id, traffic_profile_t profile);

/** This function sets the payload size for a traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @param payload_size: Payload size of the traffic
 *
 *******************************************************************************/
void sigmadut_set_traffic_payloadsize(int stream_id, int payload_size);

/** This function sets the duration for a traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @param duration    : time duration of traffic in seconds
 *
 *******************************************************************************/
void sigmadut_set_traffic_duration(int stream_id, int duration);

/** This function sets the frame rate for a traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @param frame_rate  : Number of frames per second
 *
 *******************************************************************************/
void sigmadut_set_traffic_framerate( int stream_id, int frame_rate );

/** This function sets the SSID
 *
 * @param ssid        : The pointer to the SSID of the WiFi Access Point
 *
 *******************************************************************************/
void sigmadut_set_ssid(char *ssid );

/** This function sets the string data  based on enum SIGMADUT_CONFIG_DATA_STR_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut configuration string type
 * @param str         : The pointer to the string to be set
 *
 *******************************************************************************/
int sigmadut_set_string ( SIGMADUT_CONFIG_DATA_STR_TYPE_T type, char *str);

/** This function gets the string data based on enum SIGMADUT_CONFIG_DATA_STR_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut configuration string type
 * @return str        : string value of the enum SIGMADUT_CONFIG_DATA_STR_TYPE_T type
 *
 *******************************************************************************/
char *sigmadut_get_string ( SIGMADUT_CONFIG_DATA_STR_TYPE_T type);

/** This function sets the date integer type of enum SIGMADUT_DATE_TIME_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type integer
 * @param data        : The value of to be set for enumeration type SIGMADUT_DATE_TIME_INT_TYPE_T
 *
 *******************************************************************************/
int sigmadut_set_time_date_int ( SIGMADUT_DATE_TIME_INT_TYPE_T type, int data );

/** This function gets the date type of enum SIGMADUT_DATE_TIME_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type SIGMADUT_DATE_TIME_INT_TYPE_T
 * @return int        : The int value of the enum SIGMADUT_DATE_TIME_INT_TYPE_T type
 *
 *******************************************************************************/
int sigmadut_get_time_date_int ( SIGMADUT_DATE_TIME_INT_TYPE_T type );

/** This function sets the integer type of enum SIGMADUT_TWT_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type integer
 * @param data        : The value of to be set for enumeration type SIGMADUT_TWT_INT_TYPE_T
 *
 *******************************************************************************/
int sigmadut_set_twt_int(SIGMADUT_TWT_INT_TYPE_T type, uint32_t data);

/** This function gets the integer type of enum SIGMADUT_TWT_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type SIGMADUT_TWT_INT_TYPE_T
 * @return int        : The int of the enum SIGMADUT_TWT_INT_TYPE_T type
 *
 *******************************************************************************/
int sigmadut_get_twt_int(SIGMADUT_TWT_INT_TYPE_T type);

/** This function sets the string type of enum SIGMADUT_LTF_GI_STR_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type string
 * @param str         : The value of to be set for enumeration type SIGMADUT_LTF_GI_STR_TYPE_T
 *
 *******************************************************************************/
int sigmadut_set_ltf_gi_str(SIGMADUT_LTF_GI_STR_TYPE_T type, char *str);

/** This function gets the string type of enum SIGMADUT_LTF_GI_STR_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type SIGMADUT_LTF_GI_STR_TYPE_T
 * @return str        : The string of the enum SIGMADUT_LTF_GI_STR_TYPE_T type
 *
 *******************************************************************************/
char *sigmadut_get_ltf_gi_str(SIGMADUT_LTF_GI_STR_TYPE_T type);

/** This function sets the integer type of enum SIGMADUT_TX_OMI_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type integer
 * @param data        : The value of to be set for enumeration type SIGMADUT_TX_OMI_INT_TYPE_T
 *
 *******************************************************************************/
int sigmadut_set_tx_omi_int(SIGMADUT_TX_OMI_INT_TYPE_T type, uint32_t data);

/** This function gets the integer type of enum SIGMADUT_TX_OMI_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type SIGMADUT_TX_OMI_INT_TYPE_T
 * @return int        : The int of the enum SIGMADUT_TX_OMI_INT_TYPE_T type
 *
 *******************************************************************************/
int sigmadut_get_tx_omi_int(SIGMADUT_TX_OMI_INT_TYPE_T type);

/** This function sets the integer type of enum SIGMADUT_MBO_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type integer
 * @param data        : The value of to be set for enumeration type SIGMADUT_MBO_INT_TYPE_T
 *
 *******************************************************************************/
int sigmadut_set_mbo_int(SIGMADUT_MBO_INT_TYPE_T type, uint32_t data);

/** This function gets the integer type of enum SIGMADUT_MBO_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type SIGMADUT_MBO_INT_TYPE_T
 * @return int        : The int of the enum SIGMADUT_MBO_INT_TYPE_T type
 *
 *******************************************************************************/
int sigmadut_get_mbo_int(SIGMADUT_MBO_INT_TYPE_T type);

/** This function sets the traffic integer type of enum SIGMADUT_TRAFFIC_DATA_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type integer
 * @param stream_id   : The stream id of the traffic
 * @param data        : The value of to be set for enumeration type SIGMADUT_TRAFFIC_DATA_INT_TYPE_T
 *
 *******************************************************************************/
int sigmadut_set_traffic_int ( SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type, int stream_id, int data );

/** This function gets the traffic integer type of enum SIGMADUT_TRAFFIC_DATA_INT_TYPE_T
 *
 * @param type        : The enumeration of the sigmadut data type SIGMADUT_TRAFFIC_DATA_INT_TYPE_T
 * @param stream_id   : The stream id of the traffic
 * @return str        : The string value of the enum SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type
 *
 *******************************************************************************/
int sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type, int stream_id );

/** This function increments the traffic statistics for a traffic stream
 *
 * @param type        : The enumeration of the sigmadut data type SIGMADUT_TRAFFIC_DATA_INT_TYPE_T
 * @param ts          : The pointer to the traffic stream
 * @param size        : The value to be incremented
 *
 *******************************************************************************/
int sigmadut_traffic_increment_data (SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type, traffic_stream_t *ts, int size );

/** This function sets the traffic instance
 *
 * @param sigmadut_ptr: The Pointer to the instance of sigmadut
 *
 *******************************************************************************/
int sigmadut_set_sigmadut_instance(void *sigmadut_ptr);

/** This function initializes the traffic stream table
 *
 *
 *******************************************************************************/
void sigmadut_init_stream_table( void );

/** This function find the stream index for a stream id
 *
 * @param id          : The stream identifier
 * @return            : The index of the traffic stream
 *
 *******************************************************************************/
int sigmadut_find_stream_table_entry( int id );

/** This function enables or disables the traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @param enable      : value of the traffic ( enable 1 disable 0)
 *
 *******************************************************************************/
void sigmadut_set_traffic_stream_enable ( int stream_id, int enable );

/** This function gets the traffic duration
 *
 * @param stream_id   : The stream id of traffic
 * @return            : The traffic duration in seconds
 *
 *******************************************************************************/
int sigmadut_get_traffic_duration ( int stream_id );

/** This function gets direction of traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @return traffic_direction_t : traffic direction
 *
 *******************************************************************************/
traffic_direction_t sigmadut_get_traffic_direction (int stream_id);

/** This function gets the traffic gets the stream id
 *
 * @param stream_id   : The stream id of traffic
 * @return            : The stream index of the traffic
 *
 *******************************************************************************/
uint32_t sigmadut_get_traffic_streamid ( int stream_id );

/** This function gets the enterprise security handle
 *
 * @param enteprise_handle   : The pointer to enterprise security handle
 *
 *******************************************************************************/
void sigmadut_get_enterprise_security_handle( void **enteprise_handle);

/** This function sets the enterprise security handle
 *
 * @param enterprise_handle   : The pointer to enterprise security handle
 *
 *******************************************************************************/
void sigmadut_set_enterprise_security_handle ( void *enterprise_handle);

/** This function finds the number of active traffic streams
 *
 * @return : Number of active streams
 *
 *******************************************************************************/
int sigmadut_find_num_active_streams (void );

/** This function gets the traffic stream instance of a traffic stream
 *
 * @param stream_id   : The stream id of traffic
 * @return @ref traffic_stream_t : The Pointer to the traffic stream instance
 *
 *******************************************************************************/
traffic_stream_t *sigmadut_get_traffic_stream_instance (int stream_id );

/** This function finds the free stream table index
 *
 * @return : free index of the stream table entry
 *
 *******************************************************************************/
int sigmadut_find_unallocated_stream_table_entry ( void );

/** This function sets the stream id of the traffic stream
 *
 * @param stream_idx   : The stream index of traffic
 * @param stream_id    : The stream id of the traffic
 *
 *******************************************************************************/
void sigmadut_set_streamid ( int stream_idx, int stream_id );

/** This function gets the traffic profile
 *
 * @param stream_id              : The stream id of traffic
 * @return traffic_profile_t     : Qos category of the traffic
 *
 *******************************************************************************/
traffic_profile_t sigmadut_get_traffic_profile ( int stream_id);

/** This function sets the traffic IP address of a traffic stream
 *
 * @param type        : The enum SIGMADUT_TRAFFIC_CONFIG_STR_TYPE_T type of traffic
 * @param stream_id   : The stream id of the traffic
 * @param ip_addr     : IP address in dot notation
 *
 *******************************************************************************/
int sigmadut_set_traffic_ip_address(SIGMADUT_TRAFFIC_CONFIG_STR_TYPE_T type, int stream_id, char *ip_addr);

/** This function gets the traffic IP address of a traffic stream
 *
 * @param type        : The enum SIGMADUT_TRAFFIC_CONFIG_STR_TYPE_T type of traffic
 * @param stream_id   : The stream id of the traffic
 * @return            : IP address in dot notation
 *
 *******************************************************************************/
char *sigmadut_get_traffic_ip_address(SIGMADUT_TRAFFIC_CONFIG_STR_TYPE_T type, int stream_id);

/** This function sets the port number for a traffic stream
 *
 * @param type        : The enum SIGMADUT_TRAFFIC_DATA_UINT16_TYPE_T type of traffic
 * @param stream_id   : The stream id of traffic
 * @param port        : The port number of the traffic
 *
 *******************************************************************************/
int sigmadut_set_traffic_port ( SIGMADUT_TRAFFIC_DATA_UINT16_TYPE_T type, int stream_id, uint16_t port);

/** This function gets the port number for a traffic stream
 *
 * @param type        : The enum SIGMADUT_TRAFFIC_DATA_UINT16_TYPE_T type of traffic
 * @param stream_id   : The stream id of traffic
 * @return            : The port number of the traffic
 *
 *******************************************************************************/
uint16_t sigmadut_get_traffic_port ( SIGMADUT_TRAFFIC_DATA_UINT16_TYPE_T type, int stream_id);

/** This function gets the traffic class for a given stream id
 *
 * @param stream_id   : The stream id of traffic
 * @return            : The traffic qos category
 *
 *******************************************************************************/
qos_access_category_t sigmadut_get_traffic_class ( int stream_id);

/** This function sets the WEP key
 *
 * @param key         : The pointer to the WEP key structure
 *
 *******************************************************************************/
void sigmadut_set_wepkey ( wiced_wep_key_t *key );

/** This function gets the WEP key
 *
 * @return @ref  wiced_wep_key_t : The pointer to the WEP key structure
 *
 *******************************************************************************/
wiced_wep_key_t *sigmadut_get_wepkey(void);

/** This function initializes the sigmadut
 *
 * @return cy_rslt_t    : CY_RSLT_SUCCESS
 *                      : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_init_sigmadut ( void );

/** This function initializes wifi
 *
 * @return  cy_rslt_t   : CY_RSLT_SUCCESS
 *                      : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cysigma_wifi_init(void );

/** This function sets the WEP key
 *
 * @param wepkey_buffer : The pointer to the WEP key buffer
 *
 *******************************************************************************/
void sigmadut_set_wepkey_buffer ( uint8_t *wepkey_buffer );

/** This function gets the WEP key
 *
 * @return  uint8_t     : The pointer to the key buffer
 *
 *******************************************************************************/
uint8_t *sigmadut_get_wepkey_buffer(void);

/** This function gets the SW version
 * @param   buf         : The pointer to the buffer
 * @param   buflen      : The length of the buffer
 *
 *******************************************************************************/
void cywifi_get_sw_version (char *buf , uint16_t buflen);

/** This function gets the WLAN SW version
 * @param   buf         : The pointer to the buffer
 *
 *******************************************************************************/
void cywifi_get_wlan_version(char *buf );

/** This function gets the WLAN CLM version
 * @param   buf         : The pointer to the buffer
 *
 *******************************************************************************/
void cywifi_get_wlan_clm_version(char *buf );

/** This function prints the WHD version
 *
 *******************************************************************************/
void cywifi_print_whd_version( void );

/** This function gets the wlan Chip ID
 * 
 * @return int			: The wlan chip ID
 *******************************************************************************/
int cywifi_get_wlan_platform( void );

/** This function sets enterprise security certificate type
 *
 * @param  client_certificate_type  : The type of client certificate
 * @return cy_rslt_t  : CY_RSLT_SUCCESS
 *                    : CY_RSLT_TYPE_ERROR
 *******************************************************************************/
cy_rslt_t cywifi_set_enterprise_security_cert(WIFI_CLIENT_CERT_TYPE_T client_certificate_type);

/** This function populates WPA2_ENT parameters
 *
 * @param  ent_params  : The pointer to Enterprise security parameters
 * @param  ent_sec_hdl : The pointer to Enterprise security handle
 * @return cy_rslt_t  : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 *******************************************************************************/
#ifndef H1CP_SUPPORT
cy_rslt_t cywifi_update_enterpise_security_params(cy_enterprise_security_parameters_t *ent_params, void *ent_sec_hdl);
#endif

/** This function gets system date and time from build string
 * @param   str       : The pointer to date and time string
 * @param   curr_date : The pointer to the date wifi_cert_time_t structure
 * @return  cy_rslt_t : CY_RSLT_SUCCESS
 *                    : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_get_system_date_time(char *str, wifi_cert_time_t* curr_date);

/** This function sets system date in RTC peripheral
 * @param   curr_date_time : The pointer to date and time structure
 * @return  cy_rslt_t      : CY_RSLT_SUCCESS
 *                         : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_set_system_time(wifi_cert_time_t * curr_date_time);

/** This function prints system date and time by reading date and time
 *  from RTC peripheral
 *
 * @return  cy_rslt_t : CY_RSLT_SUCCESS
 *                    : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_print_system_time( void );

/** This function gets day of the week form the date and time structure
 * @param   curr_date : The pointer to date and time structure
 * @return  int       : Day of the week in integer for ex: Sunday (0), Monday(1).. so on
 *
 *******************************************************************************/
int cywifi_get_day_of_week(wifi_cert_time_t* curr_date);

/** This function dumps H2E AP list
 *
 *******************************************************************************/
void cy_wpa3_dump_h2e_ap_list( void );
