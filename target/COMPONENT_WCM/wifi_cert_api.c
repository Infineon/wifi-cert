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

#ifndef WIFICERT_NO_HARDWARE
#ifdef COMPONENT_LWIP
#include "lwipopts.h"
#include "lwip/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/init.h"
#endif
#include "wifi_cert_commands.h"
#include "cy_result.h"
#include "wifi_cert_sigma.h"
#include "wifi_cert_sigma_api.h"
#include "cy_wcm.h"
#include "ip4string.h"
#include "whd_wifi_api.h"
#include "cy_nw_helper.h"
#include "cy_time.h"
#include "wifi_cert_utils.h"
#ifdef COMPONENT_NETXDUO
#include "nx_api.h"
#endif
#include "cy_network_mw_core.h"

#define RTOS_TASK_TICKS_TO_WAIT 100
#define SCAN_TIMEOUT 10000
#define MAX_SCAN_RESULTS 30
#define WL_RANDMAC_API_VERSION          0x0100 /**< version 1.0 */
#define WL_RANDMAC_API_MIN_VERSION      0x0100 /**< version 1.0 */
#define WLC_RANDMAC                     "randmac"

/* Randmac IOVAR struct */
typedef struct wl_randmac {
        uint16_t version;
        uint16_t len;                     /* total length */
        wl_randmac_subcmd_t subcmd_id;  /* subcommand id */
        uint8_t data[0];                  /* subcommand data */
} wl_randmac_t;

cy_wcm_config_t cy_wcm_cfg;
whd_interface_t whd_iface = NULL;

extern cy_rslt_t cy_wcm_get_whd_interface(cy_wcm_interface_t interface_type, whd_interface_t *whd_iface);
extern cy_rslt_t cy_enterprise_security_set_static_ip( cy_wcm_ip_setting_t* ip_settings );
extern char *strtok_r(char *, const char *, char **);

typedef struct
{
    cy_semaphore_t    semaphore;
    uint32_t          result_count;
} wcm_scan_data_t;

wcm_scan_data_t scan_data;
cy_wcm_scan_result_t scan_result_list[MAX_SCAN_RESULTS] = { 0 };

extern int oce_stacfon;
extern bool is_ap_up;

typedef struct
{
    cy_semaphore_t    semaphore;
    uint32_t          result_count;
} wpa3_h2e_scan_data_t;

wpa3_pt_info_t wpa3_h2e_ap_list[MAX_H2E_AP_RESULTS] = {0};

wpa3_h2e_scan_data_t wpa3_h2e_scan_data;

static bool wcm_check_bssid_in_list ( cy_wcm_scan_result_t *result_ptr );
void wcm_scan_result_callback ( cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status );
void cy_wpa3_scan_result_callback ( cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status );
static bool wpa3_check_ssid_in_list ( cy_wcm_scan_result_t *result_ptr );
uint16_t whd_chip_get_chip_id(whd_driver_t whd_driver);
cy_rslt_t cywifi_is_interface_connected ( void )
{
	bool link_up = cy_wcm_is_connected_to_ap();
	if ( link_up == true)
	{
		return CY_RSLT_SUCCESS;
	}
	else
	{
		return CY_RSLT_MW_ERROR;
	}
}

cy_rslt_t cywifi_set_network ( char *ip_addr, char *netmask, char *gateway )
{
    cy_wcm_ip_setting_t static_ip_settings;

    if ( cy_wcm_is_connected_to_ap() == false && is_ap_up == 0 )
    {
        return CY_RSLT_SUCCESS;
    }

    memset(&static_ip_settings, 0, sizeof(cy_wcm_ip_setting_t));

    if ( ip_addr != NULL )
    {
        cy_nw_str_to_ipv4((const char *)ip_addr, (cy_nw_ip_address_t *)&static_ip_settings.ip_address);
    }

    if ( netmask != NULL)
    {
        cy_nw_str_to_ipv4((const char *)netmask, (cy_nw_ip_address_t *)&static_ip_settings.netmask);
    }

    if ( gateway != NULL)
    {
        cy_nw_str_to_ipv4((const char *)gateway, (cy_nw_ip_address_t *)&static_ip_settings.gateway);
    }

#if defined(COMPONENT_NETXDUO)
    NX_IP *ip_ptr;
    if (is_ap_up == 1)
        ip_ptr = cy_network_get_nw_interface(CY_NETWORK_WIFI_AP_INTERFACE, CY_NETWORK_WIFI_AP_INTERFACE);
    else
        ip_ptr = cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);

    /* NetXDuo addresses are in host byte order */
    ULONG ipv4_addr = ntohl(static_ip_settings.ip_address.ip.v4);
    ULONG ipv4_netmask = ntohl(static_ip_settings.netmask.ip.v4);

    nx_ip_address_set(ip_ptr, ipv4_addr, ipv4_netmask);

#elif defined(COMPONENT_LWIP)
    struct netif *netif = (struct netif *)cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, CY_NETWORK_WIFI_STA_INTERFACE);
    ip4_addr_t ip_st, netmask_st, gateway_st;

    /* LWIP addresses are in network byte order */
    ip_st.addr = static_ip_settings.ip_address.ip.v4;
    netmask_st.addr = static_ip_settings.netmask.ip.v4;
    gateway_st.addr = static_ip_settings.gateway.ip.v4;

    netifapi_netif_set_addr(netif, (const ip4_addr_t *)&ip_st, (const ip4_addr_t *)&netmask_st, (const ip4_addr_t *)&gateway_st);
#endif

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_get_bssid ( uint8_t *bssid )
{
    cy_rslt_t res;
    whd_mac_t whd_bssid;

    /* set the WHD inteface role */
    whd_iface->role = WHD_STA_ROLE;

    res = whd_wifi_get_bssid(whd_iface, &whd_bssid);
    if ( res == CY_RSLT_SUCCESS)
    {
        memcpy(bssid, whd_bssid.octet, sizeof(whd_bssid.octet));
    }
	return res;
}

cy_rslt_t cywifi_set_dhcp ( bool enable)
{
	char *dhcp_str = NULL;
	dhcp_str = sigmadut_get_string(SIGMADUT_DHCP);

	if ( dhcp_str == NULL )
	{
		return CY_RSLT_MW_ERROR;
	}

	if ( enable )
	{
		if ( strncmp((const char *)dhcp_str, "1", strlen(dhcp_str)) == 0 )
		{
		    return CY_RSLT_SUCCESS;
		}
		else
		{
			sigmadut_set_string(SIGMADUT_DHCP, "1");
		}
	}
	else
	{
		sigmadut_set_string(SIGMADUT_DHCP, "0");
	}
	return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_get_macaddr ( uint8_t *mac_addr)
{
    cy_rslt_t result;

    if (oce_stacfon == 1)
    {
        result = cy_wcm_get_mac_addr(CY_WCM_INTERFACE_TYPE_AP, (cy_wcm_mac_t *)mac_addr);
	if ( result != CY_RSLT_SUCCESS )
	{
	    printf("AP get MAC Address failed result:%u\n", (unsigned int)result );
	}
    }
    else
    {
        result = cy_wcm_get_mac_addr(CY_WCM_INTERFACE_TYPE_STA, (cy_wcm_mac_t *)mac_addr);
        if ( result != CY_RSLT_SUCCESS )
        {
            printf("STA get MAC Address failed result:%u\n", (unsigned int)result );
        }
    }
	
    return CY_RSLT_SUCCESS;
}

void cywifi_get_sw_version (char *buf , uint16_t buflen)
{
#ifdef COMPONENT_LWIP
   snprintf( buf, (size_t)buflen, "FreeRTOS %s LWIP v%s",FREERTOS_VERSION, LWIP_VERSION_STRING);
#else
   snprintf( buf, (size_t)buflen, "Threadx %d.%d.%d , NetxDuo v%d.%d.%d", THREADX_MAJOR_VERSION,
	       THREADX_MINOR_VERSION, THREADX_PATCH_VERSION, NETXDUO_MAJOR_VERSION,
	       NETXDUO_MINOR_VERSION, NETXDUO_PATCH_VERSION);
#endif
}

void cywifi_get_wlan_version(char *buf )
{
    cy_rslt_t result;
    result = whd_wifi_get_wifi_version(whd_iface, buf, WLAN_SW_VERSION_LEN);
    if ( result != CY_RSLT_SUCCESS )
    {
       printf("WLAN version get failed: CY_RSLT TYPE = %lx MODULE = %lx CODE= %lx\n",
               CY_RSLT_GET_TYPE(result), CY_RSLT_GET_MODULE(result), CY_RSLT_GET_CODE(result));
    }
}

void cywifi_get_wlan_clm_version(char *buf )
{
   cy_rslt_t result;
   result = whd_wifi_get_clm_version(whd_iface, buf, WLAN_SW_VERSION_LEN);
   if ( result != CY_RSLT_SUCCESS )
   {
       printf("WLAN CLM version get failed: CY_RSLT TYPE = %lx MODULE = %lx CODE= %lx\n",
               CY_RSLT_GET_TYPE(result), CY_RSLT_GET_MODULE(result), CY_RSLT_GET_CODE(result));
   }
}

void cywifi_print_whd_version( void )
{
    printf("WHD VERSION      : " WHD_VERSION);
    printf(" : " WHD_BRANCH);
    #if defined(__ARMCC_VERSION)
        printf(" : ARM CLANG %u", __ARMCC_VERSION);
    #elif defined(__ICCARM__)
        printf(" : IAR %u", __VER__);
    #elif defined(__GNUC__)
        printf(" : GCC %u.%u", __GNUC__, __GNUC_MINOR__);
    #else
        printf(" : UNKNOWN CC");
    #endif
    printf(" : " WHD_DATE "\n");
}

int cywifi_get_wlan_platform( void )
{
    uint16_t wlan_chip_id;
    wlan_chip_id = whd_chip_get_chip_id(whd_iface->whd_driver);
    return wlan_chip_id;
}

cy_rslt_t cywifi_set_channel ( int channel )
{
    cy_rslt_t res;
    res = whd_wifi_set_channel(whd_iface, channel);
	return res;
}

cy_rslt_t cysigma_wifi_init(void )
{
	cy_rslt_t result;
	cy_wcm_cfg.interface = CY_WCM_INTERFACE_TYPE_STA;

	result = cy_wcm_init(&cy_wcm_cfg);

	if ( result != CY_RSLT_SUCCESS )
	{
	    printf("cy_wcm_init failed ret:%u\n", (unsigned int)result );
		return result;
	}

	/* Get WHD interface */
	result =  cy_wcm_get_whd_interface(cy_wcm_cfg.interface, &whd_iface);

	if ( result != CY_RSLT_SUCCESS)
	{
	    printf("WHD interface get failed result:%u\n", (unsigned int)result );
	}

	/* init scan structure */
	memset(&scan_data, 0, sizeof(wcm_scan_data_t));

	result = cy_rtos_init_semaphore(&scan_data.semaphore, 1, 0);
	if ( result != CY_RSLT_SUCCESS )
	{
	    printf("semaphore init failed ret:%u\n", (unsigned int)result);
	    return result;
	}

	/* init wpa3 H2E scan structure */
	memset(&wpa3_h2e_scan_data, 0, sizeof(wpa3_h2e_scan_data_t));
	result = cy_rtos_init_semaphore(&wpa3_h2e_scan_data.semaphore, 1, 0);
	if ( result != CY_RSLT_SUCCESS )
	{
	    printf("wpa3 H2E semaphore init failed ret:%u\n", (unsigned int)result);
	    return result;
	}
	return result;
}

/*!
 ******************************************************************************
 * Convert a security authentication type string to a wiced_security_t.
 *
 * @param[in] arg  The string containing the value.
 *
 * @return    The value represented by the string.
 */
int cywifi_get_authtype( char* encptype, char* keymgmttype, char* sectype, char* pmf )
{
	if ( ( strcmp( encptype, "none" ) == 0 ) || ( strcasecmp( sectype, "open" ) == 0 ) )
	{
		return CY_WCM_SECURITY_OPEN;
	}
	else if ( ( strcasecmp( encptype, "aes-ccmp" ) == 0 ) && ( strcasecmp( keymgmttype, "wpa2" ) == 0 ) &&
    ( strcasecmp( sectype, "SAE" ) == 0 ) )
	{
		return CY_WCM_SECURITY_WPA3_SAE;
	}
	else if ( ( ( strcasecmp( encptype, "aes-ccmp" ) == 0 ) || ( strcasecmp( encptype, "AES-CCMP-128" ) == 0 ) )
    && ( strcasecmp( keymgmttype, "wpa2" ) == 0 ) &&
    ( strcasecmp( sectype, "PSK-SAE" ) == 0 ) )
	{
		return CY_WCM_SECURITY_WPA3_WPA2_PSK;
	}
	else if ( ( ( strcasecmp( encptype, "aes-ccmp" ) == 0 ) || ( strcasecmp( encptype, "AES-CCMP-128" ) == 0 ) )
    && ( strcasecmp( keymgmttype, "wpa2" ) == 0 ) )
	{
		if ( ( strcasecmp ( pmf, "Optional" ) == 0 ) || ( strcasecmp ( pmf, "Enabled" ) == 0 ) )
		{
			return CY_WCM_SECURITY_WPA2_MIXED_PSK;
		}
		else if ( strcasecmp ( pmf, "Required" ) == 0 )
		{
			return CY_WCM_SECURITY_WPA2_AES_PSK_SHA256;
		}
		else
		{
			return CY_WCM_SECURITY_WPA2_AES_PSK;
		}
	}
	else if ( ( ( strcasecmp( encptype, "aes-ccmp" ) == 0 ) || ( strcasecmp( encptype, "AES-CCMP-128" ) == 0 ) )
    && ( strcasecmp( keymgmttype, "wpa2-ft" ) == 0 ) )
	{
		return CY_WCM_SECURITY_WPA2_FBT_PSK;
	}
	else if ( ( strcasecmp( encptype, "aes-ccmp-tkip" ) == 0 ) && ( strcasecmp( keymgmttype, "wpa2-wpa-psk" ) == 0 ) )
	{
		return CY_WCM_SECURITY_WPA2_MIXED_PSK;
	}
	else if ( ( strcasecmp( encptype, "tkip" ) == 0 ) && ( strcasecmp( keymgmttype, "wpa" ) == 0 ) &&
    ( ( strcasecmp( sectype, "PSK" ) == 0 ) || ( strcasecmp( sectype, "none" ) == 0 ) ) )
	{
		return CY_WCM_SECURITY_WPA_TKIP_PSK;
	}
	else if ( strcasecmp( encptype, "wep" ) == 0 )
	{
		return CY_WCM_SECURITY_WEP_PSK;
	}
	else if (( ( strcasecmp( encptype, "aes-ccmp" ) == 0 ) && ( strcasecmp( keymgmttype, "owe" ) == 0 ) &&
		( strcasecmp( sectype, "OWE" ) == 0 ) ) || ( strcasecmp( keymgmttype, "owe" ) == 0 ) || ( strcasecmp( sectype, "OWE" ) == 0 ) )
	{
#ifdef COMPONENT_WIFI6
		return CY_WCM_SECURITY_OWE;
#endif
	}
    else
    {
    	return CY_WCM_SECURITY_UNKNOWN;
    }
}

#ifdef QUICK_TRACK_SUPPORT
int cywifi_get_qt_authtype( char* encptype, char* keymgmttype, char* sectype, char* pmf )
{
    if ( strcasecmp( sectype, "NONE" ) == 0 )
    {
        return CY_WCM_SECURITY_OPEN;
    }
    else if ( strcasecmp( sectype, "SAE" ) == 0 )
    {
        return CY_WCM_SECURITY_WPA3_SAE;
    }
    else if ( strcasecmp( sectype, "SAE WPA-PSK" ) == 0 )
    {
        return CY_WCM_SECURITY_WPA3_WPA2_PSK;
    }
    else if ( (strcasecmp( keymgmttype, "RSN" ) == 0 ) || (strcasecmp( keymgmttype, "WPA2" ) == 0 ) )
    {    
        if ( strcasecmp( sectype, "WPA-PSK" ) == 0  )
        {
            if ( strcasecmp( encptype, "CCMP TKIP" ) == 0 )
            {
                return CY_WCM_SECURITY_WPA2_MIXED_PSK;
            }
            else if ( strcasecmp( encptype, "CCMP" ) == 0 )
            {
                return CY_WCM_SECURITY_WPA2_AES_PSK;
            }
            else if ( strcasecmp( encptype, "TKIP" ) == 0 )
            {
                return CY_WCM_SECURITY_WPA2_TKIP_PSK;
            }
        }
        else if ( strcasestr( sectype, "WPA-PSK-SHA256" ) )
        {
            if ( strcasecmp( encptype, "CCMP" ) == 0 )
            {
                return CY_WCM_SECURITY_WPA2_AES_PSK_SHA256;
            }
            else
            {
                return CY_WCM_SECURITY_UNKNOWN;
            }
        }
        else
        {
    	    return CY_WCM_SECURITY_UNKNOWN;
        }
    }	
    else if ( (strcasecmp( keymgmttype, "WPA RSN" ) == 0 ) || (strcasecmp( keymgmttype, "WPA WPA2" ) == 0 ) )
    {
        if ( strcasecmp( sectype, "WPA-PSK" ) == 0  )
		{
		    if ( strcasecmp( encptype, "CCMP TKIP" ) == 0 )
            {
                return CY_WCM_SECURITY_WPA2_WPA_MIXED_PSK;
            }
            else if ( strcasecmp( encptype, "CCMP" ) == 0 )
            {
                return CY_WCM_SECURITY_WPA2_WPA_AES_PSK;
            }
			else
			{
				return CY_WCM_SECURITY_UNKNOWN;
			}
        }
    }
	else if ( ( strcasecmp( keymgmttype, "WPA" ) == 0 ) && ( strcasecmp( sectype, "WPA-PSK" ) == 0 ) && 
              ( strcasecmp( encptype, "TKIP" ) == 0) )
	{
		return CY_WCM_SECURITY_WPA_TKIP_PSK;
	}
	else if ( ( strcasecmp( keymgmttype, "OWE" ) == 0 ) || ( strcasecmp( sectype, "OWE" ) == 0 ) )
	{
		return CY_WCM_SECURITY_OWE;
	}
    else
    {
    	return CY_WCM_SECURITY_UNKNOWN;
    }
}
#endif //#ifdef QUICK_TRACK_SUPPORT

cy_rslt_t cywifi_set_auth_credentials ( int *auth, uint8_t *wep_key_buffer, int argc )
{
	wiced_wep_key_t wep_key;
	uint8_t         key_length;
	cy_wcm_security_t auth_type = *( cy_wcm_security_t *) auth;
	char *passphrase = NULL;
	uint8_t* security_key;
	cy_wcm_scan_filter_t scan_filter;
    int i;
    cy_rslt_t ret;
	char *ssid = sigmadut_get_string(SIGMADUT_SSID);

	if  (ssid == NULL )
	{
		return CY_RSLT_MW_ERROR;
	}

	if ( auth_type == CY_WCM_SECURITY_WEP_PSK )
	{
	    wiced_wep_key_t* temp_wep_key = (wiced_wep_key_t*)wep_key_buffer;
		memcpy(&wep_key, sigmadut_get_wepkey(), sizeof(wiced_wep_key_t));
		key_length = wep_key.length;

		/* Setup WEP key 0 */
		temp_wep_key[0].index = 0;
		temp_wep_key[0].length = wep_key.length;
		memcpy(temp_wep_key[0].data, wep_key.data, wep_key.length);

		/* Setup WEP keys 1 to 3 */
		memcpy(wep_key_buffer + 1*(2 + key_length), temp_wep_key, (2 + key_length));
		memcpy(wep_key_buffer + 2*(2 + key_length), temp_wep_key, (2 + key_length));
		memcpy(wep_key_buffer + 3*(2 + key_length), temp_wep_key, (2 + key_length));
		wep_key_buffer[1*(2 + key_length)] = 1;
		wep_key_buffer[2*(2 + key_length)] = 2;
		wep_key_buffer[3*(2 + key_length)] = 3;
		key_length = 4*(2 + key_length);
		sigmadut_set_wepkey_buffer ( wep_key_buffer );
	}
	else if ( auth_type != CY_WCM_SECURITY_OPEN )
	{
        passphrase = sigmadut_get_string(SIGMADUT_PASSPHRASE);
        security_key = (uint8_t*)passphrase;
        key_length = strlen((char*)security_key);
#ifdef COMPONENT_WIFI6
        if ( !key_length && auth_type != CY_WCM_SECURITY_OWE )
#else 
	if ( !key_length )
#endif /* COMPONENT_WIFI6 */
        {
            printf("Error: Missing security key\n" );
        }
#ifdef COMPONENT_WIFI6	
        else if ( auth_type == CY_WCM_SECURITY_WPA2_MIXED_PSK || auth_type == CY_WCM_SECURITY_OWE )
#else
	else if ( auth_type == CY_WCM_SECURITY_WPA2_MIXED_PSK  )
#endif /* COMPONENT_WIFI6 */
        {
		    /* To pass 5.2.53 test case, DUT in WPA2-WPA-PSK mixed mode should be able to connect to AP in
             * WPA (TKIP only), WPA2 (AES-CCMP only), or WPA2-WPA-PSK (AES-CCMP and TKIP). Although the wifi
             * firmware supports mixed mode, it is either WPA-MIXED or WPA2-MIXED. In WPA-MIXED, DUT can connect
             * to AP in WPA or WPA2-WPA-PSK, but not WPA2. In WPA2-MIXED, DUT can connect to AP in WPA2-WPA-PSK
             * or WPA2, but not WPA. To support all 3 cases, a scan is run and the security mode is changed
             * when necessary.
             */
	    	 do {
		         scan_data.result_count = 0;
		         scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_RSSI;
		         scan_filter.param.rssi_range = CY_WCM_SCAN_RSSI_GOOD;
		     } while(0);
		     /* clear the scan results */
		     memset(scan_result_list, 0, sizeof(scan_result_list));
	    	 /* Scan for test SSID and possibly modify authentication type. */
	    	 ret =  cy_wcm_start_scan(wcm_scan_result_callback, &scan_data, &scan_filter);
	    	 if ( ret != CY_RSLT_SUCCESS )
	    	 {
	    	     printf("scan() failed ret:%u\n", (unsigned int)ret );
	    	 }

	    	 ret = cy_rtos_get_semaphore(&scan_data.semaphore, SCAN_TIMEOUT, false);
	    	 if ( ret == CY_RTOS_TIMEOUT)
	    	 {
	    	     printf("Scan semaphore timeout \n");
	    	 }

		     for ( i = 0; i < scan_data.result_count; i++)
		     {
		         if ( strncmp((const char *)&scan_result_list[i].SSID[0], (const char *)ssid, strlen(ssid)) == 0 )
		    	 {
		             /* This is required to pass test 5.2.53 */
		    	     if ( ( auth_type == CY_WCM_SECURITY_WPA2_MIXED_PSK  ) && ( scan_result_list[i].security == CY_WCM_SECURITY_WPA_TKIP_PSK ) )
		    	     {
		    		     *auth = scan_result_list[i].security;
		    		     break;
		    	     }
			     if( ( auth_type == CY_WCM_SECURITY_WPA2_MIXED_PSK ) && ( ( scan_result_list[i].security == CY_WCM_SECURITY_WPA2_AES_PSK) ||
				( scan_result_list[i].security == CY_WCM_SECURITY_WPA2_AES_PSK_SHA256 ) ) )
			     {
				     *auth = scan_result_list[i].security;
			             break;
			     }
#ifdef COMPONENT_WIFI6
			     if(auth_type == CY_WCM_SECURITY_OWE && scan_result_list[i].security == CY_WCM_SECURITY_OPEN)
			     {
				     *auth = CY_WCM_SECURITY_OPEN;
				     break;
			     }
#endif /* COMPONENT_WIFI6 */
		    	 }
		     }
	     } /* end of if */
	 } /* end of else */
	return CY_RSLT_SUCCESS;
}
cy_wcm_ip_setting_t sigma_staticip_settings;

cy_rslt_t cywifi_update_staticip_settings (void )
{
    char *gw = NULL;
    char *nmask = NULL;
    char *static_ipaddr = NULL;
    char *dhcp_str = NULL;

    static_ipaddr = sigmadut_get_string(SIGMADUT_IPADDRESS);
    gw = sigmadut_get_string(SIGMADUT_GATEWAY);
    nmask = sigmadut_get_string(SIGMADUT_NETMASK);

    memset(&sigma_staticip_settings, 0, sizeof(cy_wcm_ip_setting_t));

    if ( static_ipaddr != NULL )
    {
        cy_nw_str_to_ipv4((const char *)static_ipaddr, (cy_nw_ip_address_t *)&sigma_staticip_settings.ip_address);
        sigma_staticip_settings.ip_address.version = CY_WCM_IP_VER_V4;
    }
    if ( gw != NULL)
    {
        cy_nw_str_to_ipv4((const char *)gw, (cy_nw_ip_address_t *)&sigma_staticip_settings.gateway);
        sigma_staticip_settings.gateway.version = CY_WCM_IP_VER_V4;
    }

    if ( nmask != NULL)
    {
        cy_nw_str_to_ipv4((const char *)nmask, (cy_nw_ip_address_t *)&sigma_staticip_settings.netmask);
        sigma_staticip_settings.netmask.version = CY_WCM_IP_VER_V4;
    }

    dhcp_str = sigmadut_get_string(SIGMADUT_DHCP);
    if ( dhcp_str == NULL )
    {
        return CY_RSLT_MW_ERROR;
    }

    if ( strncmp((const char *)dhcp_str, "0", strlen(dhcp_str)) == 0 )
    {
#ifndef H1CP_SUPPORT
        cy_enterprise_security_set_static_ip( &sigma_staticip_settings );
#endif
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_connect_ap( const char *ssid, const char *passphrase, uint32_t auth_type )
{
    cy_rslt_t ret;
    cy_wcm_connect_params_t connect_params;
    cy_wcm_ip_address_t ip_addr;
    cy_wcm_ip_setting_t static_ip_settings;
    char *gw = NULL;
    char *nmask = NULL;
    char *static_ipaddr = NULL;
    uint8_t *wepkey_buf = NULL;
    char *dhcp_str = NULL;
    char *bssid = NULL;
    int retry = 0;

	static_ipaddr = sigmadut_get_string(SIGMADUT_IPADDRESS);
	gw = sigmadut_get_string(SIGMADUT_GATEWAY);
	nmask = sigmadut_get_string(SIGMADUT_NETMASK);
	bssid = sigmadut_get_string(SIGMADUT_BSSID);

	memset(&connect_params, 0, sizeof(cy_wcm_connect_params_t));
    memset(&static_ip_settings, 0, sizeof(cy_wcm_ip_setting_t));

	if ( strlen(ssid )  < sizeof(connect_params.ap_credentials.SSID ))
	{
	    memcpy(connect_params.ap_credentials.SSID, ssid, strlen(ssid));
	    if ( auth_type == CY_WCM_SECURITY_WEP_PSK)
	    {
	    	wepkey_buf = sigmadut_get_wepkey_buffer();
	      	memcpy(connect_params.ap_credentials.password, wepkey_buf, CY_WCM_MAX_PASSPHRASE_LEN);
	    }
	    else
	    {
	    	memcpy(connect_params.ap_credentials.password, passphrase, strlen(passphrase));
	    }
	}
	else
	{
	    printf("ssid too big to hold in whd_ssid_t structure\n");
	    return 1;
	}

    if ( strlen(bssid ) == MAC_ADDR_LEN)
    {
        cywifi_ether_aton(bssid, (whd_mac_t *)&connect_params.BSSID);
    }
    else
    {
        printf("Invalid bssid: %s\n", bssid);
        return 1;
    }

	connect_params.ap_credentials.security = (cy_wcm_security_t)auth_type;
	if ( static_ipaddr != NULL )
	{
	    cy_nw_str_to_ipv4((const char *)static_ipaddr, (cy_nw_ip_address_t *)&ip_addr);
	    cy_nw_str_to_ipv4((const char *)static_ipaddr, (cy_nw_ip_address_t *)&static_ip_settings.ip_address);
	}
	if ( gw != NULL)
	{
		cy_nw_str_to_ipv4((const char *)gw, (cy_nw_ip_address_t *)&static_ip_settings.gateway);
	}
	if ( nmask != NULL)
	{
		cy_nw_str_to_ipv4((const char *)nmask, (cy_nw_ip_address_t *)&static_ip_settings.netmask);
	}

	dhcp_str = sigmadut_get_string(SIGMADUT_DHCP);
	if ( dhcp_str == NULL )
	{
	   return CY_RSLT_MW_ERROR;
	}

	if ( strncmp((const char *)dhcp_str, "0", strlen(dhcp_str)) == 0 )
	{
	    connect_params.static_ip_settings = &static_ip_settings;
	}

	while (retry < MAX_CONNECTION_RETRY)
	{
	    ret = cy_wcm_connect_ap(&connect_params, &ip_addr);
	    if (ret != CY_RSLT_SUCCESS)
	    {
	        retry++;
	    }
	    else
	    {
	        break;
	    }
	}
	return ret;
}

void wcm_scan_result_callback ( cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status )
{
   wcm_scan_data_t* scan_data = (wcm_scan_data_t *)user_data;

   if( scan_data != NULL )
   {
      if ( status == CY_WCM_SCAN_COMPLETE )
      {
          cy_rtos_set_semaphore(&scan_data->semaphore, false);
      }
      else
      {
        if ( result_ptr != NULL )
        {
            if ( wcm_check_bssid_in_list (result_ptr) == false )
            {
                if(scan_data->result_count < MAX_SCAN_RESULTS)
                {
                    memcpy(&scan_result_list[scan_data->result_count], (void *)result_ptr, sizeof(cy_wcm_scan_result_t));
                    scan_data->result_count++;
                }
            } /* end of if  ( wcm_check_bssid_in_list (result_ptr) == false ) */
        } /* end of if ( result_ptr != NULL ) */
      } /* end of else */
   }/* end of if ( scan_data != NULL ) */
}

void cy_wpa3_scan_result_callback ( cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status )
{
   wpa3_h2e_scan_data_t* scan_data = (wpa3_h2e_scan_data_t *)user_data;

   if( scan_data != NULL )
   {
        if ( result_ptr != NULL )
        {
            if ( wpa3_check_ssid_in_list (result_ptr) == false )
            {
                if ( (result_ptr->flags) & WHD_SCAN_RESULT_FLAG_SAE_H2E)
                {
                    printf("AP :%s Supports H2E \n", result_ptr->SSID);
                    memcpy(wpa3_h2e_ap_list[scan_data->result_count].ssid, (void *)result_ptr->SSID, strlen((const char *)result_ptr->SSID));
                    wpa3_h2e_ap_list[scan_data->result_count].ssid_len = strlen((const char *)result_ptr->SSID);
                    scan_data->result_count++;
                }
            } /* end of if  ( wcm_check_bssid_in_list (result_ptr) == false ) */
        } /* end of if ( result_ptr != NULL ) */
        if ( ( status == CY_WCM_SCAN_COMPLETE )  || (scan_data->result_count >= MAX_AP_SSID ))
        {
             cy_rtos_set_semaphore(&scan_data->semaphore, false);
        }
   } /* end of if ( scan_data != NULL ) */
}

void cy_wpa3_dump_h2e_ap_list( void )
{
    int i, j;

    printf("\n**** H2E AP LIST");
    for ( i = 0; i < MAX_AP_SSID; i++)
    {
        if ( wpa3_h2e_ap_list[i].ssid_len != 0 )
        {
            printf(" \n SSID :%s", wpa3_h2e_ap_list[i].ssid);
        }
        if ( wpa3_h2e_ap_list[i].passphrase_len != 0 )
        {
            printf(" \t Passphrase :%s ", wpa3_h2e_ap_list[i].passhphrase);
        }
        if ( wpa3_h2e_ap_list[i].pt_set == true)
        {
            printf(" \t PT DUMP : ");
            for ( j = 0; j < sizeof(wpa3_h2e_ap_list[i].pt_point_xy); j++ )
            {
                printf("%02x", wpa3_h2e_ap_list[i].pt_point_xy[j] );
            }
        }
    }
    printf("\n**** H2E AP LIST\n");
}

static bool wcm_check_bssid_in_list ( cy_wcm_scan_result_t *result_ptr )
{
	 int i = 0;
	 bool present = false;
	 for ( i = 0; i < MAX_AP_SSID; i++ )
	 {
		 if ( memcmp(result_ptr->BSSID, &scan_result_list[i].BSSID, sizeof(cy_wcm_mac_t)) == 0 )
		 {
			/* Already existing BSSID, ignore the result */
			 present = true;
			 return present;
		 }
	 }
	 return present;
}

static bool wpa3_check_ssid_in_list ( cy_wcm_scan_result_t *result_ptr )
{
     int i = 0;
     bool present = false;
     for ( i = 0; i < MAX_AP_SSID; i++ )
     {
         if ( memcmp(result_ptr->SSID, wpa3_h2e_ap_list[i].ssid, strlen((const char *)result_ptr->SSID)) == 0 )
         {
            /* Already existing SSID, ignore the result */
             present = true;
             return present;
         }
     }
     return present;
}

cy_rslt_t cywifi_get_ip_settings(void)
{
	cy_rslt_t ret;
	cy_wcm_ip_address_t ip_addr, gw, nmask;
    char *ipstr, *gwstr, *nmstr;

	ipstr = sigmadut_get_string(SIGMADUT_IPADDRESS);
	gwstr = sigmadut_get_string(SIGMADUT_GATEWAY);
	nmstr = sigmadut_get_string(SIGMADUT_NETMASK);

	ret = cy_wcm_get_ip_addr (CY_WCM_INTERFACE_TYPE_STA, &ip_addr);
	if (ret  != CY_RSLT_SUCCESS)
	{
	    printf(" Failed to get IP Address\n");
	    return CY_RSLT_MW_ERROR;
	}

	if ( ipstr != NULL )
	{
	    ip4tos(&ip_addr.ip.v4, ipstr);
	}
	sigmadut_set_string(SIGMADUT_IPADDRESS, ipstr);

	ret = cy_wcm_get_ip_netmask (CY_WCM_INTERFACE_TYPE_STA, &nmask);
	if (ret  != CY_RSLT_SUCCESS)
	{
	    printf(" Failed to get netmask Address\n");
	    return CY_RSLT_MW_ERROR;
	}

	if ( nmstr != NULL )
	{
	    ip4tos(&nmask.ip.v4, nmstr);
	}
	sigmadut_set_string(SIGMADUT_NETMASK, nmstr);

	ret = cy_wcm_get_gateway_ip_address (CY_WCM_INTERFACE_TYPE_STA, &gw);
	if (ret  != CY_RSLT_SUCCESS)
	{
	    printf(" Failed to get gateway Address\n");
	    return CY_RSLT_MW_ERROR;
	}

	if ( gwstr != NULL )
	{
		ip4tos(&gw.ip.v4, gwstr);
	}
	sigmadut_set_string(SIGMADUT_GATEWAY, gwstr);

	return ret;
}

cy_rslt_t cywifi_disable_wifi_powersave (void )
{
    cy_rslt_t res;
    res = whd_wifi_disable_powersave(whd_iface);
    return res;
}

cy_rslt_t cywifi_enable_wifi_powersave(void)
{
    cy_rslt_t res;
    res = whd_wifi_enable_powersave(whd_iface);
    return res;
}

cy_rslt_t cywifi_get_wifi_powersave (uint32_t *value)
{
    cy_rslt_t res;
    res =  whd_wifi_get_powersave_mode(whd_iface, value);
    return res;
}

cy_rslt_t cywifi_wifi_bss_max_idle( uint32_t value )
{
    cy_rslt_t res;
    res = whd_wifi_bss_max_idle(whd_iface, value);
    return res;
}

cy_rslt_t cywifi_set_randmac( uint32_t value )
{
    cy_rslt_t res;
    wl_randmac_t iov_buf;
    iov_buf.version = WL_RANDMAC_API_VERSION;
    iov_buf.subcmd_id = value;
    uint16_t len = sizeof(iov_buf);

    res = (cy_rslt_t)whd_wifi_set_iovar_buffer(whd_iface, WLC_RANDMAC, (uint8_t *)&iov_buf, len);
    return res;
}

cy_rslt_t cywifi_set_iovar_value( const char *iovar, uint32_t value )
{
    cy_rslt_t res;
    res = whd_wifi_set_iovar_value(whd_iface, iovar, value);
	return res;
}

cy_rslt_t cywifi_set_ioctl_value( uint32_t ioctl, uint32_t value)
{
    cy_rslt_t res;
    res = whd_wifi_set_ioctl_value(whd_iface, ioctl, value);
	return res;
}

cy_rslt_t cywifi_set_ioctl_buffer( uint32_t ioctl, uint8_t *buffer, uint16_t len)
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_set_ioctl_buffer(whd_iface, ioctl, buffer, len);
	return res;
}

cy_rslt_t cywifi_set_iovar_buffer( const char *iovar, uint8_t *buffer, uint16_t len)
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_set_iovar_buffer(whd_iface, iovar, buffer, len);
    return res;
}

cy_rslt_t cywifi_get_iovar_value( const char *iovar, uint32_t *value )
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_get_iovar_value(whd_iface, iovar, value);
	return res;
}

cy_rslt_t cywifi_get_ioctl_buffer( uint32_t ioctl, uint8_t *buffer, uint16_t len)
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_get_ioctl_buffer(whd_iface, ioctl, buffer, len );
	return res;
}

cy_rslt_t cywifi_get_ioctl_value( uint32_t ioctl, uint32_t *value)
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_get_ioctl_value(whd_iface, ioctl, value);
	return res;
}

cy_rslt_t cywifi_set_up  ( void )
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_set_up(whd_iface);
	return res;
}

cy_rslt_t cywifi_set_down  ( void )
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_set_down(whd_iface);
	return res;
}

cy_rslt_t cywifi_get_bss_info( uint8_t *data )
{
    cy_rslt_t res;
    wl_bss_info_t *bi = (wl_bss_info_t *)data;
    res = whd_wifi_get_bss_info(whd_iface, bi);
    return res;
}
cy_rslt_t cywifi_scan (char *buf , uint32_t buflen)
{
    uint32_t i;
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_scan_filter_t scan_filter;

    scan_data.result_count = 0;
    memset(scan_result_list, 0, sizeof(scan_result_list));
    scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_RSSI;
    scan_filter.param.rssi_range = CY_WCM_SCAN_RSSI_FAIR;

    ret =  cy_wcm_start_scan(wcm_scan_result_callback, &scan_data, &scan_filter);
    if ( ret != CY_RSLT_SUCCESS )
    {
        printf("scan failed ret:%u\n", (unsigned int)ret );
        return ret;
    }
    ret = cy_rtos_get_semaphore(&scan_data.semaphore, SCAN_TIMEOUT, false);
    if ( ret == CY_RTOS_TIMEOUT)
    {
        printf("Scan semaphore timeout \n");
    }
    if (buf != NULL)
    {
        for (i = 0; i < scan_data.result_count; i++)
        {
            buf += snprintf(buf, (size_t)buflen, ",SSID%lu,%s,BSSID%lu,%02X:%02X:%02X:%02X:%02X:%02X",
                                (unsigned long)(i + 1),
                                scan_result_list[i].SSID,
                                (unsigned long)(i + 1),
                                scan_result_list[i].BSSID[0],
                                scan_result_list[i].BSSID[1],
                                scan_result_list[i].BSSID[2],
                                scan_result_list[i].BSSID[3],
                                scan_result_list[i].BSSID[4],
                                scan_result_list[i].BSSID[5]);
        }
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_start_pt_scan( void *wifi_ap, uint32_t count)
{
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_scan_filter_t scan_filter;

    wpa3_h2e_scan_data.result_count = 0;
    scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_BAND;
    scan_filter.param.rssi_range = CY_WCM_SCAN_RSSI_GOOD;
    scan_filter.param.band = CY_WCM_WIFI_BAND_ANY;

    ret =  cy_wcm_start_scan(cy_wpa3_scan_result_callback, &wpa3_h2e_scan_data, &scan_filter);
    if ( ret != CY_RSLT_SUCCESS )
    {
         printf("scan failed ret:%u\n", (unsigned int)ret );
         return ret;
    }
    ret = cy_rtos_get_semaphore(&wpa3_h2e_scan_data.semaphore, SCAN_TIMEOUT, false);
    if ( ret == CY_RTOS_TIMEOUT)
    {
        printf("Scan semaphore timeout \n");
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_disconnect( void )
{
    is_ap_up = 0;
    cy_wcm_disconnect_ap();
	return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_get_wifilog( uint8_t *buffer, uint16_t buflen )
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_wifi_print_whd_log(whd_iface->whd_driver);
	return res;
}

cy_rslt_t cywifi_print_whd_stats( void )
{
    cy_rslt_t res;
    res = (cy_rslt_t)whd_print_stats(whd_iface->whd_driver, WHD_TRUE);
    return res;
}

#ifndef COMPONENT_4390X
void cywifi_system_reset(void )
{
    /* reset the device */
#ifndef H1CP_SUPPORT
	NVIC_SystemReset();
#endif
}

cy_rslt_t cywifi_set_system_time(wifi_cert_time_t *curr_date_time)
{
    cyhal_rtc_t my_rtc;
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize RTC */
    result = cyhal_rtc_init(&my_rtc);

    /* set the RTC instance as per new CLIB support requires it */
    cy_set_rtc_instance(&my_rtc);

    /* Update the current time and date to the RTC peripheral */
    result = cyhal_rtc_write(&my_rtc, (const struct tm *)curr_date_time);

    return result;
}

cy_rslt_t cywifi_print_system_time(void )
{
    cyhal_rtc_t my_rtc;
    char buffer[80] = {0};
    wifi_cert_time_t data_set = {0};

    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Get the current time and date from the RTC peripheral */
    result = cyhal_rtc_read(&my_rtc,  (struct tm *)&data_set);

    if (CY_RSLT_SUCCESS == result)
    {
        /* strftime() is a C library function which is used to format date and time into string.
         * It comes under the header file "time.h" which is included by HAL (See http://www.cplusplus.com/reference/ctime/strftime/)
         */
        strftime(buffer, sizeof(buffer), "%c", (const struct tm *)&data_set);
        /* Print the buffer in serial terminal to view the current date and time */
    }
    printf("%s \n", buffer);
    return result;
}

#else
void cywifi_system_reset(void )
{
    /* reset the device */
}

cy_rslt_t cywifi_set_system_time(wifi_cert_time_t *curr_date_time)
{
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_print_system_time(void )
{
    return CY_RSLT_SUCCESS;
}
#endif

cy_rslt_t cywifi_get_system_date_time(char *str, wifi_cert_time_t* curr_date)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;
    char *token = NULL;
    char *tokptr = NULL;
    char *time_string = NULL;
    int i = 0, j = 0;
    char datestring[64] = {0};

    if ( ( str == NULL ) || (curr_date == NULL ))
    {
       printf("Date string is NULL\n");
       return res;
    }
    memcpy(datestring, str, strlen(str));

    token = strtok_r(datestring, "_", &tokptr);

    if (token == NULL )
    {
        printf("Date not found\n");
        return CY_RSLT_MW_ERROR;
    }

    while (tokptr != NULL && tokptr[0] != '\0')
    {
        token = strtok_r(NULL, "-", &tokptr);
        if ( ( tokptr != NULL ) && ( token != NULL ) && ( tokptr[0] != '\0'))
        {
            if ( i == 0 )
            {
                curr_date->tm_year = atoi(token) - WIFI_CERT_TIME_YEAR_BASE;
            }
            else if (i == 1)
            {
                curr_date->tm_month = atoi(token) - 1;
            }
            else if ( i == 2 )
            {
                time_string = token;
                if ( strcmp(tokptr,"0800") == 0)
                {
                    curr_date->tm_isdst = 0;
                }
                else
                {
                    curr_date->tm_isdst = 1;
                }
            }
            i++;
        }
    }

    if ( ( tokptr == NULL ) && (time_string == NULL ))
    {
        time_string = token;
    }

    token = strtok_r(time_string, "T", &tokptr);
    if (token == NULL )
    {
        return CY_RSLT_MW_ERROR;
    }

    curr_date->tm_mday = atoi(token);
    while (tokptr != NULL && tokptr[0] != '\0')
    {
          token = strtok_r(NULL, ":", &tokptr);
          if ( ( tokptr != NULL ) && ( token != NULL ) && ( tokptr[0] != '\0'))
          {
              if ( j == 0 )
              {
                  curr_date->tm_hour = atoi(token);
              }
              else if (j == 1)
              {
                  curr_date->tm_min = atoi(token);
              }
              else if ( j == 2 )
              {
                  time_string = token;
                  curr_date->tm_sec = atoi(token);
              }
              j++;
          }
    }
    return res;
}

int cy_wpa3_set_ssid( char *ssid )
{
    int i;
    if ( ssid != NULL )
    {
        for ( i = 0; i < MAX_AP_SSID ; i++)
        {
           if (wpa3_h2e_ap_list[i].ssid_set == false)
           {
               memcpy(wpa3_h2e_ap_list[i].ssid, ssid, strlen(ssid));
               wpa3_h2e_ap_list[i].ssid_len = strlen(ssid);
               wpa3_h2e_ap_list[i].ssid_set = true;
               break;
           }
        }
    }
    return 0;
}

int cy_wpa3_set_ssid_passphrase (char *passphrase)
{
    int i;
    if ( passphrase != NULL )
    {
        for ( i = 0; i < MAX_AP_SSID ; i++)
        {
           if ( wpa3_h2e_ap_list[i].passphrase_set == false)
           {
               memcpy(wpa3_h2e_ap_list[i].passhphrase, passphrase, strlen(passphrase));
               wpa3_h2e_ap_list[i].passphrase_len = strlen(passphrase);
               wpa3_h2e_ap_list[i].passphrase_set = true;
               break;
           }
        }
    }
    return 0;
}

int cy_wpa3_commit_wp3_pfn_network (void )
{
    int i;
    cy_rslt_t ret = CY_RSLT_SUCCESS;

    for ( i = 0; i < MAX_AP_SSID; i ++)
    {
       if ( ( wpa3_h2e_ap_list[i].ssid_set == true ) && (wpa3_h2e_ap_list[i].passphrase_set == true ))
       {
           ret = wpa3_supplicant_h2e_pfn_list_derive_pt ( wpa3_h2e_ap_list[i].ssid, wpa3_h2e_ap_list[i].ssid_len,
                                                          wpa3_h2e_ap_list[i].passhphrase, wpa3_h2e_ap_list[i].passphrase_len,
                                                          wpa3_h2e_ap_list[i].pt_point_xy, sizeof(wpa3_h2e_ap_list[i].pt_point_xy));
           if ( ret != CY_RSLT_SUCCESS)
           {
               printf("wpa3_supplicant_h2e_pfn_list_derive_pt failed for index=%d ssid:%s, passphrase:%s\n", i,
                       wpa3_h2e_ap_list[i].ssid, wpa3_h2e_ap_list[i].passhphrase);
           }
           else
           {
               wpa3_h2e_ap_list[i].pt_set = true;
           }
       }
    }
    return 0;
}

cy_rslt_t cy_wpa3_get_pfn_network( uint8_t * ssid, uint8_t *passphrase, uint8_t *pt )
{
    cy_rslt_t ret = CY_RSLT_MW_ERROR;
    int i;
    if ( ( ssid == NULL ) || ( passphrase == NULL ) || (pt == NULL))
    {
        printf("cy_wpa3_get_pfn_network ssid or passphrase NULL \n");
        return ret;
    }

    for ( i = 0; i < MAX_AP_SSID; i++ )
    {
        if ( ( wpa3_h2e_ap_list[i].ssid_set == true ) && (wpa3_h2e_ap_list[i].passphrase_set == true ))
        {
            if ( ( memcmp( ssid, wpa3_h2e_ap_list[i].ssid, strlen((const char *)wpa3_h2e_ap_list[i].ssid)) == 0 ) &&
                 ( memcmp(passphrase, wpa3_h2e_ap_list[i].passhphrase, strlen((const char *)wpa3_h2e_ap_list[i].passhphrase)) == 0 ))
            {
               if ( wpa3_h2e_ap_list[i].pt_set == true )
               {
                   memcpy(pt, wpa3_h2e_ap_list[i].pt_point_xy, sizeof(wpa3_h2e_ap_list[i].pt_point_xy));
                   ret = CY_RSLT_SUCCESS;
                   break;
               }
            }
        }
    }
    return ret;
}

int cywifi_get_day_of_week(wifi_cert_time_t* curr_date)
{
    int year;
    int month;
    static int offset_table[] = { 0, 3, 3, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

    year = curr_date->tm_year + WIFI_CERT_TIME_YEAR_BASE;
    month = curr_date->tm_month;
    year -= month < 3;

    return ( year + year / 4 - year / 100 +
             year / 400 + offset_table[month] +
             curr_date->tm_mday) % 7;
}

CYPRESS_WEAK cy_rslt_t wpa3_supplicant_h2e_pfn_list_derive_pt (uint8_t *ssid, uint8_t ssid_len, uint8_t *passphrase, uint8_t passphrase_len, uint8_t *output, uint8_t outlen )
{
   return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_get_rssi(int32_t *value)
{
    cy_rslt_t res;
    res =  whd_wifi_get_rssi(whd_iface, value);
    return res;
}

cy_rslt_t cywifi_itwt_setup(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;

    whd_itwt_setup_params_t param;
    param.setup_cmd = (uint8_t)sigmadut_get_twt_int(SIGMADUT_SETUPCOMMAND);
    param.trigger = (uint8_t)sigmadut_get_twt_int(SIGMADUT_TWT_TRIGGER);
    param.flow_type = (uint8_t)sigmadut_get_twt_int(SIGMADUT_FLOWTYPE);
    param.flow_id = (uint8_t)sigmadut_get_twt_int(SIGMADUT_FLOWID);
    param.wake_duration = (uint8_t)sigmadut_get_twt_int(SIGMADUT_NOMINALMINWAKEDUR);
    param.exponent = (uint8_t)sigmadut_get_twt_int(SIGMADUT_WAKEINTERVALEXP);
    param.mantissa = (uint16_t)sigmadut_get_twt_int(SIGMADUT_WAKEINTERVALMANTISSA);
    param.wake_time_h = 0;
    param.wake_time_l = 102400;
    res = whd_wifi_itwt_setup(whd_iface, &param);

    return res;
}

cy_rslt_t cywifi_btwt_setup(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;

    whd_btwt_join_params_t param;
    param.setup_cmd = (uint8_t)sigmadut_get_twt_int(SIGMADUT_SETUPCOMMAND);
    param.trigger = (uint8_t)sigmadut_get_twt_int(SIGMADUT_TWT_TRIGGER);
    param.flow_type = (uint8_t)sigmadut_get_twt_int(SIGMADUT_FLOWTYPE);
    param.wake_duration = (uint8_t)sigmadut_get_twt_int(SIGMADUT_NOMINALMINWAKEDUR);
    param.exponent = (uint8_t)sigmadut_get_twt_int(SIGMADUT_WAKEINTERVALEXP);
    param.mantissa = (uint16_t)sigmadut_get_twt_int(SIGMADUT_WAKEINTERVALMANTISSA);
    param.wake_time_h = 0;
    param.wake_time_l = 0;
    param.bid = (uint8_t)sigmadut_get_twt_int(SIGMADUT_BTWT_ID);
    res = whd_wifi_btwt_join(whd_iface, &param);
    return res;
}

cy_rslt_t cywifi_twt_teardown(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;

    whd_twt_teardown_params_t param;
    param.negotiation_type = (uint8_t)sigmadut_get_twt_int(SIGMADUT_NEGOTIATIONTYPE);
    param.flow_id = (uint8_t)sigmadut_get_twt_int(SIGMADUT_FLOWID);
    param.bcast_twt_id = 0;
    param.teardown_all_twt = 0;
    res = whd_wifi_twt_teardown(whd_iface, &param);

    return res;
}

cy_rslt_t cywifi_twt_operation(uint32_t operation)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;

    whd_twt_information_params_t param;
    param.suspend = operation;
    param.flow_id = (uint8_t)sigmadut_get_twt_int(SIGMADUT_FLOWID);
    param.resume_time = (uint8_t)sigmadut_get_twt_int(SIGMADUT_RESUME_DURATION);
    res = whd_wifi_twt_information_frame(whd_iface, &param);
    return res;
}

cy_rslt_t cywifi_set_ltf_gi(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;
    char *ltf = sigmadut_get_ltf_gi_str(SIGMADUT_LTF);
    char *gi = sigmadut_get_ltf_gi_str(SIGMADUT_GI);

    if ((strcasecmp(ltf, "6.4") == 0) && (strcasecmp(gi, "0.8") == 0))
    {
        cywifi_set_iovar_value(IOVAR_STR_HE_LTF_GI_SEL, WL_RSPEC_HE_2x_LTF_GI_0_8us);
    }
    else if ((strcasecmp(ltf, "6.4") == 0) && (strcasecmp(gi, "1.6") == 0))
    {
        cywifi_set_iovar_value(IOVAR_STR_HE_LTF_GI_SEL, WL_RSPEC_HE_2x_LTF_GI_1_6us);
    }
    else if ((strcasecmp(ltf, "12.8") == 0) && (strcasecmp(gi, "3.2") == 0))
    {
        cywifi_set_iovar_value(IOVAR_STR_HE_LTF_GI_SEL, WL_RSPEC_HE_4x_LTF_GI_3_2us);
    }
    else
    {
        cywifi_set_iovar_value(IOVAR_STR_HE_LTF_GI_SEL, WL_RSPEC_HE_1x_LTF_GI_0_8us);
    }
    return res;
}

cy_rslt_t cywifi_he_omi(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;

    whd_he_omi_params_t param;
    param.rx_nss = 0xFF;
    param.chnl_wdth = (uint8_t)sigmadut_get_tx_omi_int(SIGMADUT_CHNLWIDTH);
    param.ul_mu_dis = (uint8_t)sigmadut_get_tx_omi_int(SIGMADUT_ULMUDISABLE);
    param.tx_nsts = (uint8_t)sigmadut_get_tx_omi_int(SIGMADUT_TXNSTS);
    param.er_su_dis = 0;
    param.dl_mu_resound = 0;
    param.ul_mu_data_dis = 0;
    res = whd_wifi_he_omi(whd_iface, &param);

    return res;
}

cy_rslt_t cywifi_he_muedca_war_enable(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;

    whd_xtlv_t *he_muedca_iovar;
	uint8_t buffer[256] = {0};
	int len;

    he_muedca_iovar = (whd_xtlv_t *)buffer;
    he_muedca_iovar->id = WL_HE_CMD_MUEDCA_OPT;
    he_muedca_iovar->len = sizeof(uint32_t);
    he_muedca_iovar->data[0] = 1; // enable MU EDCA WAR
    he_muedca_iovar->data[1] = 0;
    he_muedca_iovar->data[2] = 0;
    he_muedca_iovar->data[3] = 0;

    len = sizeof(uint32_t) + 4;

    res = cywifi_set_iovar_buffer(IOVAR_STR_HE, buffer, len);

    return res;
}

cy_rslt_t cywifi_clear_pmkid (void )
{
    cy_rslt_t res;
    res = whd_wifi_pmkid_clear(whd_iface);
    return res;
}

cy_rslt_t cywifi_mbo_add_chan_pref(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;
    whd_mbo_add_chan_pref_params_t param;

    param.opclass = (uint8_t)sigmadut_get_mbo_int(SIGMADUT_MBO_OPCLASS);
    param.chan = (uint8_t)sigmadut_get_mbo_int(SIGMADUT_MBO_CHANNEL);
    param.pref = (uint8_t)sigmadut_get_mbo_int(SIGMADUT_MBO_PREFERENCE);
    param.reason = (uint8_t)sigmadut_get_mbo_int(SIGMADUT_MBO_REASON);
    res = whd_wifi_mbo_add_chan_pref(whd_iface, &param);

    return res;
}

cy_rslt_t cywifi_mbo_clear_chan_pref(void)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;
    whd_mbo_del_chan_pref_params_t param;

    param.opclass = 0x73;
    param.chan = 48;
    res = whd_wifi_mbo_del_chan_pref(whd_iface, &param);

    param.opclass = 0x73;
    param.chan = 44;
    res = whd_wifi_mbo_del_chan_pref(whd_iface, &param);

    param.opclass = 0x73;
    param.chan = 40;
    res = whd_wifi_mbo_del_chan_pref(whd_iface, &param);

    param.opclass = 0x83;
    param.chan = 37;
    res = whd_wifi_mbo_del_chan_pref(whd_iface, &param);

    param.opclass = 0x83;
    param.chan = 33;
    res = whd_wifi_mbo_del_chan_pref(whd_iface, &param);

    return res;
}

cy_rslt_t cywifi_mbo_send_notif(uint8_t sub_elem_type)
{
    cy_rslt_t res = CY_RSLT_SUCCESS;
    res = whd_wifi_mbo_send_notif(whd_iface, sub_elem_type);
    return res;
}

cy_rslt_t cywifi_clear_saepassword(void)
{
    cy_rslt_t res;
    uint8_t buffer[256] = {0};
    wsec_sae_password_t *sae_password;

    sae_password = (wsec_sae_password_t *)buffer;
    memset(sae_password->password, 0, sizeof(sae_password->password));
    sae_password->password_len = htod16(0);

    res = cywifi_set_iovar_buffer(IOVAR_STR_SAE_PASSWORD, buffer, sizeof(wsec_sae_password_t));

    return res;
}

#endif /* WIFICERT_NO_HARDWARE */
