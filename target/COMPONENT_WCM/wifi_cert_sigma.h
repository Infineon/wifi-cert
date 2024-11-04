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

#include "wifi_cert_commands.h"
#ifndef WIFICERT_NO_HARDWARE
#ifdef COMPONENT_FREERTOS
#include "task.h"
#endif
#endif

#define GET_TARGET_NAME_STR(tgt_name)   #tgt_name
#define GET_TARGET_NAME(tgt_name)       GET_TARGET_NAME_STR(tgt_name)
#define MODEL GET_TARGET_NAME(TARGET_NAME)
#define GET_BUILD_TIME(str) #str
#define GET_BUILD_TIME_STRING(str) GET_BUILD_TIME(str)
#define BUILD_TIME_STAMP_VALUE GET_BUILD_TIME_STRING(BUILD_TIME_STAMP)

#ifndef PLATFORM
#define PLATFORM MODEL
#endif /* PLATFORM */
#define PLATFORM_VERSION "1"

#ifdef COMPONENT_LWIP
#define FREERTOS_VERSION tskKERNEL_VERSION_NUMBER
#endif

#define CY_SATURATION_LINK_CP   2912
#define CY_SATURATION_LINK_IDL  4708
#define CY_CP_TRAFFIC_PUSH_RATE 2081

#define PRIO_OFFSET_BETWEEN_AC 1

/* All TX thread priority will be atleast this much lower than RX thread */
#define PRIO_LOW_THRESH 4
#define PRIO_HIGH_THRESH 1

#ifdef OS_THREADX
/* For H1-CP, adjust this based on IP_THREAD_PRIORITY and WHD thread priority */
#define SIGMA_TRAFFIC_PRIORITY (CY_RTOS_PRIORITY_HIGH + 2)
#define RTOS_TX_ADJUSTMENT 3
#define RTOS_RX_ADJUSTMENT 2
#else /* FreeRTOS */
#define SIGMA_TRAFFIC_PRIORITY (CY_RTOS_PRIORITY_HIGH)
#define RTOS_TX_ADJUSTMENT 1
#define RTOS_RX_ADJUSTMENT 3
#endif /* OS_THREADX */

#define TRAFFIC_AGENT_SOCKET_TIMEOUT 1
#define PRIO_RX_HIGH_THRESH 0
#define SINGLE_STREAM_PRIO_BOOST_OF_RX_THREAD_WRT_TX 1
#define PRIO_BOOST_OF_RX_THREAD_WRT_TX 1
#define PRIORITY_TO_NATIVE_PRIORITY(priority) (SIGMA_TRAFFIC_PRIORITY - priority)
#define TRAFFIC_END_OF_THREAD( thread )   cy_rtos_exit_thread()

/** Sigma Certification Commands
 *  Class that represents a Sigma DUT Agent
 *  Common inteface to external test agent (UCC) and
 *  interface to host MBED Stack and WLAN Chip
 */
typedef struct sigmadut
{
    cy_mutex_t sigmadut_mutex;/* data protection mutex */
    wifi_cert_time_t time;  /* Current Date and time set to 0 */
    char _interface[16];    /*   = TEST_INTERFACE */
    char _passphrase[64];   /*   = TEST_PASSPHRASE_DEFAULT;  */
    uint8_t _wepkey_buffer[64];/*   = HEX values of WEP */
    char _sectype[32];      /*   = TEST_SECTYPE_DEFAULT;  */
    char _encptype[16];     /*   = TEST_ENCPTYPE_DEFAULT; */
    char _keymgmttype[16];  /*   = TEST_KEYMGMTTYPE_DEFAULT; */
    char _pmf[32];          /*   = TEST_PMF_DEFAULT; */
    char _ssid[33];         /*   = TEST_SSID_DEFAULT; */
    char _bssid[18];        /*   = TEST_BSSID_DEFAULT; */
    char _channel[4];       /*   = TEST_CHANNEL_DEFAULT; */
    char _using_dhcp[4];    /*   = TEST_USING_DHCP_DEFAULT; */
    char _pwrsave[16];      /*   = TEST_PWRSAVE_DEFAULT;  */
    char dut_ip_addr[16];       /*   = DUT_IP_ADDR_DEFAULT; */
    char dut_netmask[16];       /*   = DUT_NETMASK_DEFAULT; */
    char dut_gateway[16];       /*   = DUT_GATEWAY_DEFAULT; */
    char dut_primary_dns[16];   /*   = DUT_PRIMARY_DNS_DEFAULT; */
    char dut_secondary_dns[16]; /*   = DUT_SECONDARY_DNS_DEFAULT; */
    char username[32];          /*   USER_NAME for WPA-ENT */
    char password[64];          /*   password for PEAP */
    char clientcert[32];        /*   Client Certificate name */
    char trustedrootcrt[32];    /*   Trusted Root CA name*/
    char innereap[32];          /*   Inner EAP type */
    char peapver[32];           /*   PEAP Version   */
    void *enterprise_sec_handle;/*   Enterprise security handle */
    wpa2_ent_eap_type_t ent_eap_type; /* Type of EAP */
    wiced_wep_key_t wepkey;     /* WEP encryption key */
    traffic_stream_t stream_table[NUM_STREAM_TABLE_ENTRIES];
    twt_param_t twt_param;
    ltf_gi_t ltf_gi;
    tx_omi_t tx_omi;
    mbo_param_t mbo_param;
}sigmadut_t;

cy_mutex_t *sigmadut_get_mutex_instance (void );
void sigmadut_init ( void );
