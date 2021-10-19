/*
 * Copyright 2021, Cypress Semiconductor Corporation (an Infineon company) or
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

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef WIFICERT_NO_HARDWARE
#include "cyabs_rtos_impl.h"
#include "command_console.h"
#include "cyabs_rtos.h"
#include "whd.h"
#include "whd_version.h"
#include "whd_int.h"
#include "whd_types.h"
#include "whd_wlioctl.h"
#include "wifi_cert_enterprise_certificate.h"
#include "cyhal_rtc.h"
#include "cy_enterprise_security.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cy_result.h"
#include "wifi_cert_stubs.h"
#include "wifi_cert_rtos_stubs.h"
#include "wifi_cert_enterprise_certificate.h"
#endif

#define VENDOR "Cypress"
#define GET_WIFI_CERT_VER(str) #str
#define GET_WIFI_CERT_STRING(str) GET_WIFI_CERT_VER(str)
#define GET_WIFI_CERT_VER_STRING  GET_WIFI_CERT_STRING(WIFI_CERT_VER)

#define SIGMA_DUT_VERSION GET_WIFI_CERT_VER_STRING

#define TEST_TX_POWER 31
#define MAX_SSID_LEN 32
#define MAC_ADDRESS_LOCALLY_ADMINISTERED_BIT  0x02

#define TEST_PASSPHRASE_DEFAULT     "12345678"
#define TEST_SECTYPE_DEFAULT        "none"
#define TEST_ENCPTYPE_DEFAULT       "none"
#define TEST_KEYMGMTTYPE_DEFAULT    "wpa2"
#define TEST_PMF_DEFAULT            "Default"
#define TEST_SSID_DEFAULT           "Cypress"
#define TEST_BSSID_DEFAULT          "00:00:00:00:00:00"
#define TEST_CHANNEL_DEFAULT        "1"
#define TEST_INTERFACE              "wlan0"
#define IOVAR_STR_MFP               "mfp"
#define IOVAR_STR_MPDU_PER_AMPDU    "ampdu_mpdu"
#define IOVAR_STR_VHT_FEATURES      "vht_features"
#define IOVAR_STR_AMPDU_RX          "ampdu_rx"
#define IOVAR_STR_LDPC_CAP          "ldpc_cap"
#define IOVAR_STR_LDPC_TX           "ldpc_tx"
#define IOVAR_STR_SGI_RX            "sgi_rx"
#define IOVAR_STR_SGI_TX            "sgi_tx"
#define IOVAR_STR_SEND_ADDBA        "ampdu_send_addba"


#define SIGMA_AGENT_DELAY_1MS 1
#define SIGMA_AGENT_DELAY_10MS 10
#define SIGMA_AGENT_DELAY_100MS 100
#define SIGMA_AGENT_DELAY_1S    1000
#define SIGMA_AGENT_DELAY_MULTI_STREAM 1
#define VHT_FEATURES_PROPRATES_ENAB  (2)

/* Structure TIME stores years since 1900 */
#define WIFI_CERT_TIME_YEAR_BASE (1900u)

#define TEST_USING_DHCP_DEFAULT     "1"
#define DUT_IP_ADDR_DEFAULT         "0.0.0.0"
#define DUT_NETMASK_DEFAULT         "0.0.0.0"
#define DUT_GATEWAY_DEFAULT         "0.0.0.0"
#define DUT_PRIMARY_DNS_DEFAULT     "0.0.0.0"
#define DUT_SECONDARY_DNS_DEFAULT   "0.0.0.0"

#define DELIMITER_STRING ","
#define MAX_PARAM_CNT 64
#define AC_COUNT 4
#define MAX_AP_SSID 15
#define MAX_TID_PRIORITY 7
#define ETH_ADDR_LEN 6
#define MAX_CONNECTION_RETRY 10
#define STACK_MEM_ALLOC_RETRIES 100
#define WLAN_SW_VERSION_LEN 200

/*
 * Rate-limit- the number of packets sent in a interval
 *
 */
#define RATE_CHECK_TIME_LIMIT ( 100 ) // Milliseconds

void get_sw_version(char *buf);

bool get_mfptype ( char *mfp_string, uint32_t *mfp );
bool set_mfptype ( char *mfp_string, uint32_t mfp );
int wifi_get_ac_params_sta(edcf_acparam_t *ac_param );
void wifi_prioritize_acparams ( edcf_acparam_t * ac_params, int * priority );

typedef enum
{
	CMD_SUCCESS = 0,
	CMD_FAILED
} CMD_STATUS_T;

/**
 * Structure for storing a WEP key
 */
typedef struct
{
    uint8_t index;    /**< WEP key index [0/1/2/3]                                             */
    uint8_t length;   /**< WEP key length. Either 5 bytes (40-bits) or 13-bytes (104-bits) */
    uint8_t data[32]; /**< WEP key as values NOT chars                                     */
} wiced_wep_key_t;

uint8_t ascii_to_hex(uint8_t value);

cy_time_t get_time (void) ;

extern void wait_ms(int ms);

#define NUM_STREAM_TABLE_ENTRIES ( 4 )
#define SINGLE_ACTIVE_TRAFFIC_STREAM (1)
#define TS_THREAD_STACK_SIZE ( 1024 )
#define GET_CURRENT_TIME  get_time()
#define MAX_PAYLOAD_SIZE (1500)
#define TX_MAX_PAYLOAD_SIZE (1500)
#define MAX_PING_PAYLOAD_SIZE (1500)
#define UDP_RX_BUFSIZE MAX_PAYLOAD_SIZE

#define MAX_SECONDS  (59)
#define MAX_MINUTES  (59)
#define MAX_HOURS    (24)
#define MAX_DAYS     (31)
#define MAX_WDAYS    (7)
#define MAX_MONTHS   (12)

/** Traffic direction */
typedef enum
{
    TRAFFIC_ANY        = 0x00,  /**< Traffic Direction TX/RX */
    TRAFFIC_SEND       = 0x01,  /**< Traffic TX */
    TRAFFIC_RECV       = 0x02,  /**< Traffic RX */
} traffic_direction_t;

/** Traffic profile */
typedef enum
{
    PROFILE_FILE_TRANSFER = 0x01, /**< File transfer */
    PROFILE_MULTICAST     = 0x02, /**< Multicast  */
    PROFILE_IPTV          = 0x03, /**< IPTV */
    PROFILE_TRANSACTION   = 0x04, /**< Transaction */
} traffic_profile_t;

/** Qos of the traffic */
typedef enum
{
    WMM_AC_BE =         0,      /**< Best Effort */
    WMM_AC_BK =         1,      /**< Background  */
    WMM_AC_VI =         2,      /**< Video       */
    WMM_AC_VO =         3,      /**< Voice       */
} qos_access_category_t;

/** WPA2 ENT EAP Types
 *
 * @brief
 * Various WPA2 ENT supplicant EAP Types
 */
typedef enum
{
    WPA_ENT_EAP_TYPE_NONE         = 0   /**<  Invalid EAP type. */,
    WPA_ENT_EAP_TYPE_GTC          = 6,  /**<  EAP-GTC type refer to RFC 3748. */
    WPA_ENT_EAP_TYPE_TLS          = 13  /**<  EAP-TLS type refer to RFC 2716. */,
    WPA_ENT_EAP_TYPE_SIM          = 18  /**<  EAP-SIM type refer to draft-haverinen-pppext-eap-sim-12.txt. */,
    WPA_ENT_EAP_TYPE_TTLS         = 21  /**<  EAP-TTLS type refer to draft-ietf-pppext-eap-ttls-02.txt. */,
    WPA_ENT_EAP_TYPE_AKA          = 23  /**<  EAP-AKA type refer to draft-arkko-pppext-eap-aka-12.txt. */,
    WPA_ENT_EAP_TYPE_PEAP         = 25  /**<  PEAP type refer to draft-josefsson-pppext-eap-tls-eap-06.txt. */,
    WPA_ENT_EAP_TYPE_MSCHAPV2     = 26  /**<  MSCHAPv2 type refer to draft-kamath-pppext-eap-mschapv2-00.txt. */,
} wpa2_ent_eap_type_t;

/* Client certificate type */
typedef enum
{
    WIFI_CLIENT_CERTIFICATE_CRED_HOSTAPD = 0,
    WIFI_CLIENT_CERTIFICATE_CRED_MSFT,
    WIFI_CLIENT_CERTIFICATE_CRED_PMF
} WIFI_CLIENT_CERT_TYPE_T;

/** Traffic stream details - this is a private structure; visible to allow for static definition. */
typedef struct
{
    uint8_t allocated;             /**< Indicates if stream table entry has been allocated for use */
    uint8_t enabled;               /**< Indicates if stream is currently active */
    uint32_t stream_id;            /**< Traffic stream id */
    traffic_profile_t profile;     /**< Type of Traffic */
    traffic_direction_t direction; /**< Traffic direction */
    char dest_ipaddr[16];          /**< IPv4 destination address in dot notation */
    uint16_t dest_port;            /**< Destination port */
    char src_ipaddr[16];           /**< IPv4 source address in dot notation */
    uint16_t src_port;             /**< Source port */
    int payload_size;              /**< Payload size */
    int frame_rate;                /**< Frames per second */
    int duration;                  /**< Traffic duration in seconds */
    int start_delay;               /**< start delay */
    qos_access_category_t ac;      /**< Qos access category */
    int max_frame_count;           /**< Maximum frame count */
    int frames_sent;               /**< Frames sent in a traffic session */
    int frames_received;           /**< Frames received in a traffic session */
    int out_of_sequence_frames;    /**< Out of sequence frames in a session */
    int bytes_sent;                /**< Bytes sent in a traffic session */
    int bytes_received;            /**< Bytes received in a traffic session */
    void *tx_socket;               /**< UDP TX socket handle */
    void *rx_socket;               /**< UDP RX socket handle */
    int stop_time;                 /**< Traffic stop time */
    void *thread_ptr;              /**< This allows the console thread to delete threads */
    void *networkinteface;         /**< Network interface pointer */
    uint8_t *payload;              /**< Pointer to data payload */
    void* sigmadut;                /**< Instance of sigmadut */
} traffic_stream_t;

/** struct wifi_cert_time_t (date) for setting Date and Time */
typedef struct
{
    int tm_sec;                    /**< Seconds */
    int tm_min;                    /**< Minutes */
    int tm_hour;                   /**< Hour    */
    int tm_mday;                   /**< Day of the month */
    int tm_month;                  /**< Month */
    int tm_year;                   /**< Year since 1900 */
    int tm_wday;                   /**< Weekday Sunday is 0, Monday is 1 and so on */
    int tm_isdst;                  /**< Daylight Savings. 0 - Disabled, 1 - Enabled */
} wifi_cert_time_t;

int spawn_ts_thread( int (*ts_function) (traffic_stream_t* ts), traffic_stream_t *ts );
int udp_rx( traffic_stream_t* ts );
int udp_tx( traffic_stream_t* ts );
int udp_transactional( traffic_stream_t* ts );
int wifi_traffic_stop_ping( void );

#define PING_ID              (0xAFAF)
#define PING_SEQNO_INC(iecho,ping_seqnum)  ((iecho->seqno) = htons(ping_seqnum))
typedef struct icmp_echo_hdr icmp_echo_hdr_t;

void wifi_ping_prepare_echo( icmp_echo_hdr_t *iecho, int len, uint16_t ping_seqnum );

#define test_print( a ) { printf a; }

unsigned short inet_chksum(const void *dataptr, unsigned short len);
unsigned short lwip_standard_chksum(const void *dataptr, int len);

#ifndef FOLD_U32T
#define FOLD_U32T(u)          (((u) >> 16) + ((u) & 0x0000ffffUL))
#endif

#ifndef SWAP_BYTES_IN_WORD
#define SWAP_BYTES_IN_WORD(w) (((w) & 0xff) << 8) | (((w) & 0xff00) >> 8)
#endif /* SWAP_BYTES_IN_WORD */

/** Traffic thread details - this is a private structure; visible to allow for static definition. */
typedef struct
{
	void *thread_handle;                        /**< Thread handle                     */
	void *thread_instance;                      /**< Thread instance                   */
	void *stack_mem;                            /**< Thread stack memory               */
    traffic_stream_t *ts;                       /**< Pointer to traffic stream         */
    int (*ts_function)( traffic_stream_t * );   /**< Callback traffic function pointer */
} thread_details_t;

typedef int (*ping_function) (void *arg );
int spawn_ping_thread( int (*ping_function)(void *arg ), void *arg );
int wifi_traffic_send_ping( void *arg );
void ping_thread_wrapper( void *arg );

/** Ping stats details */
typedef struct
{
   uint32_t      num_ping_requests;  /**< num ping requests */
   uint32_t      num_ping_replies;   /**< num ping replies */
} ping_stats_t;

/** Ping thread details - this is a private structure; visible to allow for static definition. */
typedef struct
{
   void *thread_handle;            /**< pointer to thread handle */
   void *thread_instance;          /**< pointer to thread instance */
   void *thread_details;           /**< pointer to thread details */
   void *stack_mem;                /**< pointer to the stack memory */
   void *sigmadut;                 /**< pointer to sigmadut instance */
   void *network_interface;        /**< pointer to network interface */
   void *arg;                      /**< pointer to ping argument */
   void *sock;                     /**< pointer to socket  */
   int ping_enabled;               /**< ping enabled flag */
   int (*ping_function)( void * ); /**< callback function of ping */
} ping_thread_details_t;


typedef   int (*set_prog)( char* params[] );
typedef   int (*reset_prog)(void );

typedef   int (*set_param) ( char *string[] );
typedef   int (*disable_param)( void );

int set_vht_params ( char* params [] );
int set_pmf_params ( char* params [] );
int set_nan_params ( char* params [] );
int set_p2p_params ( char* params [] );
int set_wfd_params ( char* params [] );
int set_wpa3_params ( char* params [] );

/* Do not reject any ADDBA request by sending ADDBA response with status "decline" */
int disable_addba_reject    ( void );

/* Disable AMPDU Aggregation on the transmit side */
int disable_vht_ampdu       ( void );

/* Disable AMSDU Aggregation on the transmit side */
int disable_vht_amsdu       ( void );

/* Disable TKIP in VHT mode */
int disable_vht_tkip        ( void );

/* Disable WEP in VHT mode */
int disable_vht_wep         ( void );

/* Disable LDPC code at the physical layer for both TX and RX side */
int disable_vht_ldpc        ( void );

/* Disable SU(single-user) TxBF beamformee capability with explicit feedback */
int disable_vht_txbf        ( void );

/* STA sends RTS with static bandwidth signaling */
int disable_vht_bw_sgnl     ( void );

/* if enabled, then reject any ADDBA request by sending ADDBA response with status "decline" */
int enable_vht_addba_reject ( char *string [] );

/* Enable AMPDU Aggregation on the transmit side */
int enable_vht_ampdu        ( char *string [] );

/* Enable AMSDU Aggregation on the transmit side */
int enable_vht_amsdu        ( char *string [] );

/* Set STBC(Space-Time Block Coding) Receive Streams */
int set_vht_stbc_rx         ( char *string [] );

/* Set channel Width String (20/40/80/160/Auto) */
int set_vht_channel_width   ( char* string [] );

/* Set SMPS (Spatial-Multiplex) Power Save mode  (Dynamic/0, Static/1, No Limit/2) */
int set_vht_sm_power_save   ( char* string [] );

/* Set TX spatial stream Value range (1/2/3) */
int set_vht_txsp_stream     ( char* string [] );

/* Set RX spatial stream Value range ( 1/2/3) */
int set_vht_rxsp_stream     ( char* string [] );

/* Set VHT band 2.4Ghz/5Ghz */
int set_vht_band            ( char* string [] );

/* when set the STA sends the RTS frame with dynamic bandwidth signaling */
int enable_vht_dyn_bw_sgnl  ( char* string [] );

/* Enable Short guard interval at 80 Mhz */
int set_vht_sgi80           ( char* string [] );

/* Enable SU(single-user) TxBF beamformee capability with explicit feedback */
int enable_vht_txbf         ( char* string [] );

/* Enable LDPC code at the physical layer for both TX and RX side */
int enable_vht_ldpc         ( char* string [] );

/* To set the operating mode notification element for 2 values
 * – NSS (number of spatial streams) and channel width.
 * Example - For setting the operating mode notification element
 * with NSS=1 & BW=20Mhz - Opt_md_notif_ie,1;20
 */
int set_vht_opt_md_notif_ie ( char* string [] );

/*
 * nss_capabilty;mcs_capability. This parameter gives  a description
 * of the supported spatial streams and MCS rate capabilities of the STA
 * For example – If a STA supports 2SS with MCS 0-9, then nss_mcs_cap,2;0-9
 */
int set_vht_nss_mcs_cap     ( char* string [] );

/* set the Tx Highest Supported Long Gi Data Rate subfield */
int set_vht_tx_lgi_rate     ( char* string [] );

/* set the CRC field to all 0’s */
int set_vht_zero_crc        ( char* string [] );

/* Enable TKIP in VHT mode */
int enable_vht_tkip         ( char* string [] );

/* Enable WEP in VHT mode */
int enable_vht_wep          ( char* string [] );

/* Enable the ability to send out RTS with bandwidth signaling */
int enable_vht_bw_sgnl      ( char* string [] );


int reset_vht_params ( void );
int reset_pmf_params (void );
int reset_nan_params (void );
int reset_p2p_params (void );
int reset_wfd_params (void );
int reset_wpa3_params (void );

typedef enum
{
    WIFI_VHT = 0,
    WIFI_PMF,
    WIFI_NAN,
    WIFI_P2P,
    WIFI_WFD,
    WIFI_WPA3
} dot11_progtype;

/** DOT11 program details - this is a private structure; visible to allow for static definition. */
typedef struct
{
    const char *prog;    /**< program name */
    set_prog   setcb;   /**<  callback function to set parameters for a given program   */
    reset_prog resetcb; /**<  callback function to reset parameters for a given program */
} dot11_prog_t;

/** Parameter table - this is a private structure; visible to allow for static definition. */
typedef struct
{
        const char    *param;      /**< parameter name                                */
        set_param     setparam_cb; /**< callback function to enable/set the parameter */
        disable_param disable_cb;  /**< callback function to disable the parameter    */
} param_table_t;


void ts_thread_wrapper( void *arg );
/* get configured IP */
extern int sta_get_ip_config  (int argc, char *argv[], tlv_buffer_t** data );

 /* set STA IP address */
extern int sta_set_ip_config  ( int argc, char *argv[], tlv_buffer_t** data );

 /* get STA info */
extern int sta_get_info  ( int argc, char *argv[], tlv_buffer_t** data );

 /* get STA MAC Address */
extern int sta_get_mac_address     ( int argc, char *argv[], tlv_buffer_t** data );

 /* return true if STA is connected to AP */
extern int sta_is_connected        ( int argc, char *argv[], tlv_buffer_t** data );

 /* sta get ip connection information */
extern int sta_verify_ip_connection  ( int argc, char *argv[], tlv_buffer_t** data );

 /* get AP BSSID */
extern int sta_get_bssid      ( int argc, char *argv[], tlv_buffer_t** data );

 /* get version */
extern int ca_get_version     ( int argc, char *argv[], tlv_buffer_t** data );

 /* get device information */
extern int device_get_info     ( int argc, char *argv[], tlv_buffer_t** data );

 /* list device interfaces */
extern int device_list_interfaces   ( int argc, char *argv[], tlv_buffer_t** data );

 /* set STA encryption */
extern int sta_set_encryption      ( int argc, char *argv[], tlv_buffer_t** data );

 /* set STA Pre-shared Key */
extern int sta_set_psk            ( int argc, char *argv[], tlv_buffer_t** data );

 /* set STA security */
extern int sta_set_security       ( int argc, char *argv[], tlv_buffer_t** data );

/* set STA EAP TTLS security */
extern int sta_set_eapttls        ( int argc, char *argv[], tlv_buffer_t** data );

/* set STA EAP TLS security */
extern int sta_set_eaptls         ( int argc, char *argv[], tlv_buffer_t** data );

/* set STA PEAP security */
extern int sta_set_peap           ( int argc, char *argv[], tlv_buffer_t** data );

/* set STA EAP AKA security */
extern int sta_set_eapaka         ( int argc, char *argv[], tlv_buffer_t** data );

/* set STA EAP FAST security */
extern int sta_set_eapfast        ( int argc, char *argv[], tlv_buffer_t** data );

/* set STA EAP SIM security */
extern int sta_set_eapsim         ( int argc, char *argv[], tlv_buffer_t** data );

/* set STA System Time */
extern int sta_set_systime        ( int argc, char *argv[], tlv_buffer_t** data );

 /* join AP */
extern int sta_associate          ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA preset test parameters */
extern int sta_preset_testparameters    ( int argc, char *argv[], tlv_buffer_t** data );

 /* Send Ping Traffic */
extern int traffic_send_ping      ( int argc, char *argv[], tlv_buffer_t** data );

 /* Stop Send Ping Traffic */
extern int traffic_stop_ping      ( int argc, char *argv[], tlv_buffer_t** data );

 /* Traffic agent configure */
extern int traffic_agent_config     ( int argc, char *argv[], tlv_buffer_t** data );

 /* Traffic agent reset */
 extern int traffic_agent_reset      ( int argc, char *argv[], tlv_buffer_t** data );

 /* Traffic agent send */
 extern int traffic_agent_send       ( int argc, char *argv[], tlv_buffer_t** data );

 /* Traffic agent receive start */
 extern int traffic_agent_receive_start   ( int argc, char *argv[], tlv_buffer_t** data );

 /* Traffic agent receive stop */
 extern int traffic_agent_receive_stop    ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA set 11n mode */
 extern int sta_set_11n                   ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA disconnect from AP */
 extern int sta_disconnect                ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA Scan for AP's */
 extern int sta_scan                      ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA re-associate to AP */
 extern int sta_reassoc                   ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA reset parameters to default */
 extern int sta_reset_default             ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA set wireless */
 extern int sta_set_wireless              ( int argc, char *argv[], tlv_buffer_t** data );

 /* STA send ADDBA request */
 extern int sta_send_addba                ( int argc, char *argv[], tlv_buffer_t** data );

 /* Client Cert */
 extern int sta_client_cert               ( int argc, char* argv[], tlv_buffer_t** data);

 /* Read WHD logs */
 extern int whdlog_read                   ( int argc, char *argv[], tlv_buffer_t** data);

 /* Print WHD stats */
 extern int print_whd_stats               ( int argc, char *argv[], tlv_buffer_t** data);

 /* Read host logs */
 extern int wicedlog_read                 ( int argc, char *argv[], tlv_buffer_t** data );

 /* Get system date */
 extern int sta_get_systime               ( int argc, char *argv[], tlv_buffer_t** data );

 /* Get wlan status */
 extern int sta_get_wlan_status           ( int argc, char *argv[], tlv_buffer_t** data );

 /* Reboot Sigma DUT APP */
 extern int reboot_sigma                  ( int argc, char *argv[], tlv_buffer_t** data  );

#define WIFI_CERT_COMMANDS \
    { "sta_get_ip_config",               sta_get_ip_config,                         0,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_ip_config",               sta_set_ip_config,                         0,    NULL, NULL,     NULL,  "" }, \
    { "sta_get_info",                    sta_get_info,                              0,    NULL, NULL,     NULL,  "" }, \
    { "sta_get_mac_address",             sta_get_mac_address,                       0,    NULL, NULL,     NULL,  "" }, \
    { "sta_is_connected",                sta_is_connected,                          0,    NULL, NULL,     NULL,  "" }, \
    { "sta_verify_ip_connection",        sta_verify_ip_connection,                  0,    NULL, NULL,     NULL,  "" }, \
    { "sta_get_bssid",                   sta_get_bssid,                             0,    NULL, NULL,     NULL,  "" }, \
    { "ca_get_version",                  ca_get_version,                            0,    NULL, NULL,     NULL,  "" }, \
    { "device_get_info",                 device_get_info,                           0,    NULL, NULL,     NULL,  "" }, \
    { "device_list_interfaces",          device_list_interfaces,                    0,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_encryption",              sta_set_encryption,                        0,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_psk",                     sta_set_psk,                               1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_security",                sta_set_security,                          1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapttls",                 sta_set_eapttls,                           1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eaptls",                  sta_set_eaptls,                            1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_peap",                    sta_set_peap,                              1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapaka",                  sta_set_eapaka,                            1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapfast",                 sta_set_eapfast,                           1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapsim",                  sta_set_eapsim,                            1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_systime",                 sta_set_systime,                           1,    NULL, NULL,     NULL,  "" }, \
    { "sta_associate",                   sta_associate,                             0,    NULL, NULL,     NULL,  "" }, \
    { "sta_preset_testparameters",       sta_preset_testparameters,                 0,    NULL, NULL,     NULL,  "" }, \
    { "traffic_send_ping",               traffic_send_ping,                         0,    NULL, NULL,     NULL,  "" }, \
    { "traffic_stop_ping",               traffic_stop_ping,                         0,    NULL, NULL,     NULL,  "" }, \
    { "traffic_agent_config",            traffic_agent_config,                      0,    NULL, NULL,     NULL,  "" }, \
    { "traffic_agent_reset",             traffic_agent_reset,                       0,    NULL, NULL,     NULL,  "" }, \
    { "traffic_agent_send",              traffic_agent_send,                        0,    NULL, NULL,     NULL,  "" }, \
    { "traffic_agent_receive_start",     traffic_agent_receive_start,               0,    NULL, NULL,     NULL,  "" }, \
    { "traffic_agent_receive_stop",      traffic_agent_receive_stop,                0,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_11n",                     sta_set_11n,                               0,    NULL, NULL,     NULL,  "" }, \
    { "sta_disconnect",                  sta_disconnect,                            0,    NULL, NULL,     NULL,  "" }, \
    { "sta_scan",                        sta_scan,                                  0,    NULL, NULL,     NULL,  "" }, \
    { "sta_reassoc",                     sta_reassoc,                               0,    NULL, NULL,     NULL,  "" }, \
    { "sta_reset_default",               sta_reset_default,                         0,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_wireless",                sta_set_wireless,                          0,    NULL, NULL,     NULL,  "" }, \
    { "sta_send_addba",                  sta_send_addba,                            0,    NULL, NULL,     NULL,  "" }, \
    { "whdlog",                          whdlog_read,                               0,    NULL, NULL,     NULL,  "" }, \
    { "clientcert",                      sta_client_cert,                           0,    NULL, NULL,     NULL,  "" }, \
    { "whdstats",                        print_whd_stats,                           0,    NULL, NULL,     NULL,  "" }, \
    { "wicedlog",                        wicedlog_read,                             0,    NULL, NULL,     NULL,  "" }, \
    { "date",                            sta_get_systime,                           0,    NULL, NULL,     NULL,  "" }, \
    { "status",                          sta_get_wlan_status,                       0,    NULL, NULL,     NULL,  "" }, \
    { "reboot",                          reboot_sigma,                              0,    NULL, NULL,     NULL,  "Reboot the device"}, \


