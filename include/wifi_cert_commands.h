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
#ifndef COMPONENT_4390X
#include "cyhal_rtc.h"
#endif
#include "cy_wcm.h"

/* Revisit this change when the WPA3 security changes are merged */
#ifndef H1CP_SUPPORT
#include "cy_enterprise_security.h"
#endif
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
#define MAX_PASSPHRASE_LEN  128

#define TEST_PASSPHRASE_DEFAULT     "12345678"
#define TEST_SECTYPE_DEFAULT        "none"
#define TEST_ENCPTYPE_DEFAULT       "none"
#define TEST_KEYMGMTTYPE_DEFAULT    "wpa2"
#define TEST_PMF_DEFAULT            "Default"
#define TEST_SSID_DEFAULT           "Cypress"
#define TEST_BSSID_DEFAULT          "00:00:00:00:00:00"
#define TEST_CHANNEL_DEFAULT        "1"
#define TEST_INTERFACE              "wlan0"
#define TEST_PWRSAVE_DEFAULT        "on"
#ifdef QUICK_TRACK_SUPPORT
#define QT_PASSPHRASE_DEFAULT       ""
#define QT_SECTYPE_DEFAULT          "WPA-PSK"
#define QT_ENCPTYPE_DEFAULT         "CCMP TKIP"
#define QT_KEYMGMTTYPE_DEFAULT      "WPA RSN"
#endif
#define IOVAR_STR_MFP               "mfp"
#define IOVAR_STR_MPDU_PER_AMPDU    "ampdu_mpdu"
#define IOVAR_STR_VHT_FEATURES      "vht_features"
#define IOVAR_STR_AMPDU_RX          "ampdu_rx"
#define IOVAR_STR_LDPC_CAP          "ldpc_cap"
#define IOVAR_STR_LDPC_TX           "ldpc_tx"
#define IOVAR_STR_SGI_RX            "sgi_rx"
#define IOVAR_STR_SGI_TX            "sgi_tx"
#define IOVAR_STR_SEND_ADDBA        "ampdu_send_addba"
#define IOVAR_STR_HE_LTF_GI_SEL     "he_ltf_gi_sel"
#define IOVAR_STR_AMSDU             "amsdu"
#define IOVAR_STR_AMSDU_IN_AMPDU    "rx_amsdu_in_ampdu"
#define IOVAR_STR_2G_RATE           "2g_rate"
#define IOVAR_STR_5G_RATE           "5g_rate"
#define IOVAR_STR_6G_RATE           "6g_rate"
#define IOVAR_STR_STF_TXCHAIN       "txchain"
#define IOVAR_STR_STF_RXCHAIN       "rxchain"
#define IOVAR_STR_HE                "he"
#define IOVAR_STR_WNM_BTM_DIS       "wnm_btm_dis"
#define IOVAR_STR_WNM               "wnm" 
#define IOVAR_STR_FORCE_PRB_RESPEC  "force_prb_rspec"
#define IOVAR_STR_OCE               "oce"
#define IOVAR_STR_ENABLE_GIANTRX    "giantrx"
#ifdef H1CP_SUPPORT
#define IOVAR_STR_TXOP11N_WAR       "txop_11n_cert_war"
#define IOVAR_STR_STBC_RX           "stbc_rx"
#define IOVAR_STR_AKM_TRANS_CLEAR   "akm_trans_clear"
#endif /* H1CP_SUPPORT */

#define WL_HE_CMD_MUEDCA_OPT 28

#define WL_RSPEC_ENCODE_HE 0x03000000
#define WL_RSPEC_HE_MCS_MASK 0x0000000F
#define WL_RSPEC_HE_NSS_MASK 0x000000F0
#define WL_RSPEC_HE_NSS_SHIFT 4
#define WL_RSPEC_HE_NSS_UNSPECIFIED 0xF

#define WL_RSPEC_BW_20MHZ	0x00010000
#define WL_RSPEC_BW_40MHZ	0x00020000
#define WL_RSPEC_BW_80MHZ	0x00030000
#define WL_RSPEC_BW_160MHZ	0x00040000

#define WL_RSPEC_HE_1x_LTF_GI_0_8us	(0x0)
#define WL_RSPEC_HE_2x_LTF_GI_0_8us	(0x1)
#define WL_RSPEC_HE_2x_LTF_GI_1_6us	(0x2)
#define WL_RSPEC_HE_4x_LTF_GI_3_2us	(0x3)

#define WL_STF_CONFIG_CHAINS_DISABLED -1

#define SIGMA_AGENT_DELAY_1MS 1
#define SIGMA_AGENT_DELAY_10MS 10
#define SIGMA_AGENT_DELAY_100MS 100
#define SIGMA_AGENT_DELAY_1S    1000

#ifndef OS_THREADX
#define SIGMA_AGENT_DELAY_MULTI_STREAM 1
#else
#define SIGMA_AGENT_DELAY_MULTI_STREAM 10
#endif

/* According to 802.11ac Draft 4.0 specification, Extended VHT rate support enables all VHT MCS rates 0-9 for
 * all bandwidths but however it currently prohibits the use of VHT in 2.4GHz and specific rates such as
 * VHT MCS9 nss1 20Mhz, VHT MCS9 nss2 20Mhz, VHT MCS6 nss3 80Mhz */
#ifdef COMPONENT_CAT5
#define VHT_FEATURES_PROPRATES_ENAB  (16)
#else
#define VHT_FEATURES_PROPRATES_ENAB  (2)
#endif

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
#ifdef QUICK_TRACK_SUPPORT
#define MAX_CONNECTION_RETRY 3
#else
#define MAX_CONNECTION_RETRY 10
#endif
#define STACK_MEM_ALLOC_RETRIES 100
#define WLAN_SW_VERSION_LEN 200
#define MAX_H2E_AP_RESULTS 15
#define H2E_PT_XY_SIZE 65
#define DEFAULT_FRAME_SIZE (1000)
#define DEFAULT_BSS_MAX_IDLE_PERIOD 10
#define MAC_ADDR_LEN 17
#define HE_ERSU_PPDU_RUALLOCTONE_242 54626320 /* value taken from firmware */
#define FORCE_PRB_RESPEC_TO_5_5_MBPS    11      /* value taken from firmware */
#define ENABLE_WNM_MAXIDLE		261
#define DISABLE_WNM_MAXIDLE		257

/** subcommands that can apply to randmac */
enum {
        WL_RANDMAC_SUBCMD_NONE                          = 0,
        WL_RANDMAC_SUBCMD_GET_VERSION                   = 1,
        WL_RANDMAC_SUBCMD_ENABLE                        = 2,
        WL_RANDMAC_SUBCMD_DISABLE                       = 3,
        WL_RANDMAC_SUBCMD_CONFIG                        = 4,
        WL_RANDMAC_SUBCMD_STATS                         = 5,
        WL_RANDMAC_SUBCMD_CLEAR_STATS                   = 6,

        WL_RANDMAC_SUBCMD_MAX
};
typedef uint16_t wl_randmac_subcmd_t;

/* MBO attributes as defined in the mbo spec */
enum {
    SIGMA_MBO_ATTR_NON_PREF_CHAN_REPORT = 2,
    SIGMA_MBO_ATTR_CELL_DATA_CAP = 3,
};

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
#define TS_THREAD_STACK_SIZE ( 4*1024 )
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

/** struct to store SSID, Passphrase and PT */
typedef struct
{
   bool ssid_set;                           /**< ssid set           */
   bool passphrase_set;                     /**< passphrase set     */
   bool pt_set;                             /**< PT set             */
   uint8_t ssid[MAX_SSID_LEN];              /**< SSID               */
   uint8_t ssid_len;                        /**< SSID length        */
   uint8_t passhphrase[MAX_PASSPHRASE_LEN]; /**< passphrase         */
   uint8_t passphrase_len;                  /**< passphrase length  */
   uint8_t pt_point_xy[H2E_PT_XY_SIZE];     /**< uncompress pointxy */
} wpa3_pt_info_t;

/** struct to store TWT parameters */
typedef struct
{
    uint8_t ndppagingind;          /**< Indicates ndppagingind */
    uint8_t resppmmode;            /**< Indicates resppmmode */
    uint8_t negotiationtype;       /**< Indicates negotiationtype */
    uint8_t twt_setup;             /**< Indicates twt_setup */
    uint8_t setupcommand;          /**< Indicates setupcommand */
    uint8_t twt_trigger;           /**< Indicates twt_trigger */
    uint8_t implicit;              /**< Indicates implicit */
    uint8_t flowtype;              /**< Indicates flowtype */
    uint8_t wakeintervalexp;       /**< Indicates wakeintervalexp */
    uint8_t protection;            /**< Indicates protection */
    uint8_t nominalminwakedur;     /**< Indicates nominalminwakedur */
    uint8_t twt_channel;           /**< Indicates twt_channel */
    uint8_t flowid;                /**< Indicates flowid */
    uint8_t reserved;              /**< Indicates padding */
    uint16_t wakeintervalmantissa; /**< Indicates wakeintervalmantissa */
    uint8_t resume_duration;       /**< Indicates resume_duration */
    uint8_t btwt_id;		   /**< Indicates btwt_id */
} twt_param_t;

/** struct to store HE LTF_GI parameters */
typedef struct
{
    char ltf[8];          /**< Indicates ltf */
    char gi[8];           /**< Indicates gi */
} ltf_gi_t;

/** struct to store HE TX OMI parameters */
typedef struct
{
    uint8_t txnsts;          /**< Indicates txnsts */
    uint8_t chnlwidth;       /**< Indicates channel width */
    uint8_t ulmudisable;     /**< Indicates UL MU disable */
    uint8_t ulmudatadisable; /**< Indicates UL MU data disable */
} tx_omi_t;

/** struct to store MBO parameters */
typedef struct
{
    uint8_t opclass;         /**< Indicates operating class */
    uint8_t chan;            /**< Indicates channel */
    uint8_t pref;            /**< Indicates channel preference */
    uint8_t reason;          /**< Indicates reason */
} mbo_param_t;

/* Program types for sta_preset_testparameters */
typedef enum
{
    MBO = 1,
} preset_param_program;

typedef enum
{
    LTF_GI = 0x00000001,
    TWT_SETUP = 0x00000002,
    TWT_TEARDOWN = 0x00000004,
    TWT_ITWT = 0x00000008,
    TWT_BTWT = 0x00000010,
    HE_OMI = 0x00000020,
    MBO_ADD = 0x00000040,
    MBO_CLEAR = 0x00000080,
    TWT_SUSPEND = 0x00000100,
    TWT_RESUME = 0x00000200,
    PPDUTXTYPE = 0x00000400,
    RUALLOCTONE = 0x00000800,
} rfeature_action;

int spawn_ts_thread( int (*ts_function) (traffic_stream_t* ts), traffic_stream_t *ts, int idx );
int udp_rx( traffic_stream_t* ts );
int udp_tx( traffic_stream_t* ts );
int udp_transactional( traffic_stream_t* ts );
int wifi_traffic_stop_ping( void );

#define PING_ID              (0xAFAF)
#define PING_SEQNO_INC(iecho,ping_seqnum)  ((iecho->seqno) = htons(ping_seqnum))
typedef struct icmp_echo_hdr icmp_echo_hdr_t;

void wifi_ping_prepare_echo( icmp_echo_hdr_t *iecho, int len, uint16_t ping_seqnum );
void wifi_handle_ping_thread_exit( void );
void wifi_cleanup_ping_thread(void);

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
	cy_thread_t thread_handle;                  /**< Thread handle object              */
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
   cy_thread_t thread_handle;      /**< thread handle object*/
   void *thread_instance;          /**< pointer to thread instance */
   void *thread_details;           /**< pointer to thread details */
   void *stack_mem;                /**< pointer to the stack memory */
   void *sigmadut;                 /**< pointer to sigmadut instance */
   void *network_interface;        /**< pointer to network interface */
   void *arg;                      /**< pointer to ping argument */
   void *sock;                     /**< pointer to socket  */
   int ping_enabled;               /**< ping enabled flag */
   int (*ping_function)( void * ); /**< callback function of ping */
} sigmadut_ping_thread_details_t;


typedef   int (*set_prog)( int argc, char* params[] );
typedef   int (*reset_prog)(void );

typedef   int (*set_param) ( char *string[] );
typedef   int (*disable_param)( void );

int set_vht_params ( int argc, char* params[] );
int set_pmf_params ( int argc, char* params[] );
int set_nan_params ( int argc, char* params[] );
int set_p2p_params ( int argc, char* params[] );
int set_wfd_params ( int argc, char* params[] );
int set_wpa3_params ( int argc, char* params[] );
int set_he_params( int argc, char* params[] );
int set_mbo_params( int argc, char* params[] );
int set_oce_params( int argc, char* params[] );

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
 * � NSS (number of spatial streams) and channel width.
 * Example - For setting the operating mode notification element
 * with NSS=1 & BW=20Mhz - Opt_md_notif_ie,1;20
 */
int set_vht_opt_md_notif_ie ( char* string [] );

/*
 * nss_capabilty;mcs_capability. This parameter gives  a description
 * of the supported spatial streams and MCS rate capabilities of the STA
 * For example � If a STA supports 2SS with MCS 0-9, then nss_mcs_cap,2;0-9
 */
int set_vht_nss_mcs_cap     ( char* string [] );

/* set the Tx Highest Supported Long Gi Data Rate subfield */
int set_vht_tx_lgi_rate     ( char* string [] );

/* set the CRC field to all 0�s */
int set_vht_zero_crc        ( char* string [] );

/* Enable TKIP in VHT mode */
int enable_vht_tkip         ( char* string [] );

/* Enable WEP in VHT mode */
int enable_vht_wep          ( char* string [] );

/* Enable the ability to send out RTS with bandwidth signaling */
int enable_vht_bw_sgnl      ( char* string [] );

/* Enable BCC code at the physical layer for both TX and RX side */
int enable_he_bcc(char *string[]);

/* HE MCS Fixed Rate */
int set_he_mcs_fixedrate(char* string []);

/* Set HE tx stream */
int set_he_txsp_stream(char *string []);

/* Set HE rx stream */
int set_he_rxsp_stream(char *string []);

cy_rslt_t start_ap_sigma(const char *ssid, const char *key, uint8_t channel,cy_wcm_security_t security_type,
    cy_wcm_custom_ie_info_t *custom_ie, uint8_t band);

int reset_vht_params ( void );
int reset_pmf_params (void );
int reset_nan_params (void );
int reset_p2p_params (void );
int reset_wfd_params (void );
int reset_wpa3_params (void );
int reset_he_params(void );
int reset_mbo_params(void );
int reset_oce_params(void );
#ifdef QUICK_TRACK_SUPPORT
int reset_qt_params(void);
#endif

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

/* set STA power save mode  */
extern int sta_set_pwrsave        ( int argc, char *argv[], tlv_buffer_t** data );

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

 /* sta set AP SSID and Passphrase(PFN) for WPA3 */
 extern int sta_set_wpa3_pfn_network      ( int argc, char *argv[], tlv_buffer_t** data  );

 /* sta commit PFN List for WPA3 H2E PT derivation */
 extern int sta_commit_wpa3_pfn_network   ( int argc, char *argv[], tlv_buffer_t** data  );

 /* sta dump WPA3 H2E AP(s) */
 extern int sta_dump_wpa3_h2e_aps ( int argc, char *argv[], tlv_buffer_t** data);

/* enable/disable legacy STA power-saving */
extern int sta_set_power_save(int argc, char *argv[], tlv_buffer_t** data);

/* get STA information */
extern int sta_get_parameter(int argc, char *argv[], tlv_buffer_t** data);

/* set run-time functional dfeature */
extern int sta_set_rfeature(int argc, char *argv[], tlv_buffer_t** data);

/* device sends a specific frame */
extern int dev_send_frame(int argc, char *argv[], tlv_buffer_t** data);

/* configure 802.11 parameters on the STAUT for Quick Track*/
extern int sta_configure      ( int argc, char *argv[], tlv_buffer_t** data );

/* set the status for STA role */
extern bool is_sta_up;

/* set for STA-CFON */
extern int oce_stacfon;

extern bool is_ap_up;

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
    { "sta_configure",                   sta_configure,								1,	  NULL, NULL,	  NULL,  "" }, \
    { "sta_set_security",                sta_set_security,                          1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapttls",                 sta_set_eapttls,                           1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eaptls",                  sta_set_eaptls,                            1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_peap",                    sta_set_peap,                              1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapaka",                  sta_set_eapaka,                            1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapfast",                 sta_set_eapfast,                           1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_eapsim",                  sta_set_eapsim,                            1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_systime",                 sta_set_systime,                           1,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_pwrsave",                 sta_set_pwrsave,                           1,    NULL, NULL,     NULL,  "" }, \
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
    { "sta_set_wpa3_pfn_network",        sta_set_wpa3_pfn_network,                  0,    NULL, NULL,     NULL,  "" }, \
    { "sta_commit_wpa3_pfn_network",     sta_commit_wpa3_pfn_network,               0,    NULL, NULL,     NULL,  "" }, \
    { "dump_h2e_aps",                    sta_dump_wpa3_h2e_aps,                     0,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_power_save",              sta_set_power_save,                        0,    NULL, NULL,     NULL,  "" }, \
    { "sta_get_parameter",               sta_get_parameter,                         0,    NULL, NULL,     NULL,  "" }, \
    { "sta_set_rfeature",                sta_set_rfeature,                          0,    NULL, NULL,     NULL,  "" }, \
    { "dev_send_frame",                  dev_send_frame,                            0,    NULL, NULL,     NULL,  "" }, \
    { "reboot",                          reboot_sigma,                              0,    NULL, NULL,     NULL,  "Reboot the device"}, \


