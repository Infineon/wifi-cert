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
#endif /* COMPONENT_LWIP */
#endif
#include "wifi_cert_utils.h"
#include "wifi_cert_sigma_api.h"
#include "wifi_cert_sigma.h"
#include "wifi_intf_api.h"
#include "wifi_traffic_api.h"

static int stream_id         = 0;
static cy_mutex_t tx_done_mutex;
static int he_enabled;
int oce_stacfon;
bool is_ap_up;

sigmadut_ping_thread_details_t ping_thread_details;
thread_details_t detail[4];
bool sigmadut_ping_thread_running = false;
static char wlan_version[WLAN_SW_VERSION_LEN] = {0};

edcf_acparam_t ac_params[AC_COUNT];
int ac_priority[AC_COUNT];
int tx_ac_priority_num[AC_COUNT] = { 0 };
int rx_ac_priority_num[AC_COUNT] = { 0 };
bool is_wpa_ent_security = false;
bool is_sta_up = false;
#ifndef H1CP_SUPPORT
cy_enterprise_security_parameters_t ent_params = {0};
#endif
void *sigmadut_enterprise_sec_handle = NULL;
uint8_t wlan_ioctl_buffer[WLAN_IOCTL_BUF_LEN] = {0};

#define INITIALISER_IPV4_ADDRESS1(addr_var, addr_val) addr_var = {CY_WCM_IP_VER_V4, {.v4 = (uint32_t)(addr_val)}}
#define MAKE_IPV4_ADDRESS1(a, b, c, d)      ((((uint32_t) d) << 24) | (((uint32_t) c) << 16) | (((uint32_t) b) << 8) |((uint32_t) a))

static const cy_wcm_ip_setting_t ap_ip_settings =
{
    INITIALISER_IPV4_ADDRESS1(.ip_address, MAKE_IPV4_ADDRESS1(192, 168, 0, 2)),
    INITIALISER_IPV4_ADDRESS1(.netmask, MAKE_IPV4_ADDRESS1(255, 255, 255, 0)),
    INITIALISER_IPV4_ADDRESS1(.gateway, MAKE_IPV4_ADDRESS1(192, 168, 0, 2)),
};

WIFI_CLIENT_CERT_TYPE_T enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_HOSTAPD;

const cy_command_console_cmd_t wifi_cert_command_table[] =
{
	WIFI_CERT_COMMANDS
	CMD_TABLE_END
};

dot11_prog_t dot_prog_table[] =
{
        { "VHT", set_vht_params, reset_vht_params   },
        { "PMF", set_pmf_params, reset_pmf_params   },
        { "NAN", set_nan_params, reset_nan_params   },
        { "P2P", set_p2p_params, reset_p2p_params   },
        { "WFD", set_wfd_params, reset_wfd_params   },
        { "WPA3", set_wpa3_params, reset_wpa3_params},
        { "HE", set_he_params, reset_he_params      },
        { "MBO", set_mbo_params, reset_mbo_params   },
        { "OCE", set_oce_params, reset_oce_params   },
#ifdef QUICK_TRACK_SUPPORT
        { "QT", NULL, reset_qt_params               },
#endif
        { NULL,        NULL,           NULL         }
};

param_table_t vht_param_table [] =
{
        { "addba_reject",    enable_vht_addba_reject, disable_addba_reject },
        { "ampdu",           enable_vht_ampdu,        disable_vht_ampdu    },
        { "amsdu",           enable_vht_amsdu,        disable_vht_amsdu    },
        { "stbc_rx",         set_vht_stbc_rx,         NULL                 },
        { "width",           set_vht_channel_width,   NULL                 },
        { "smps",            set_vht_sm_power_save,   NULL                 },
        { "txsp_stream",     set_vht_txsp_stream,     NULL                 },
        { "rxsp_stream",     set_vht_rxsp_stream,     NULL                 },
        { "band",            set_vht_band,            NULL                 },
        { "dyn_bw_sgnl",     enable_vht_dyn_bw_sgnl,  disable_vht_bw_sgnl  },
        { "sgi80",           set_vht_sgi80,           NULL                 },
        { "txbf",            enable_vht_txbf,         disable_vht_txbf     },
        { "ldpc",            enable_vht_ldpc,         disable_vht_ldpc     },
        { "Opt_md_notif_ie", set_vht_opt_md_notif_ie, NULL                 },
        { "nss_mcs_cap",     set_vht_nss_mcs_cap,     NULL                 },
        { "tx_lgi_rate",     set_vht_tx_lgi_rate,     NULL                 },
        { "zero_crc",        set_vht_zero_crc,        NULL                 },
        { "vht_tkip",        enable_vht_tkip,         disable_vht_tkip     },
        { "vht_wep",         enable_vht_wep,          disable_vht_wep      },
        { "bw_sgnl",         enable_vht_bw_sgnl,      disable_vht_bw_sgnl  },
        {  NULL,             NULL,                    NULL                 }
};

param_table_t he_param_table [] =
{
        { "ldpc",            enable_vht_ldpc,         disable_vht_ldpc     },
        { "bcc",             enable_he_bcc,           NULL                 },
        { "mcs_fixedrate",   set_he_mcs_fixedrate,    NULL                 },
        { "amsdu",           enable_vht_amsdu,        disable_vht_amsdu    },
        { "txsp_stream",     set_he_txsp_stream,      NULL                 },
        { "rxsp_stream",     set_he_rxsp_stream,      NULL                 },
        { "OMControl",       NULL,                    NULL                 },
        { "UPH",             NULL,                    NULL                 },
        {  NULL,             NULL,                    NULL                 }
};

int sta_get_ip_config ( int argc, char *argv[], tlv_buffer_t** data )
{
    printf( "\nstatus,COMPLETE,dhcp,%s,", sigmadut_get_string(SIGMADUT_DHCP) );

	if ( cywifi_is_interface_connected() == CMD_SUCCESS )
	{
	   printf( "ip,%s,",   sigmadut_get_string(SIGMADUT_IPADDRESS) );
	   printf( "mask,%s,", sigmadut_get_string(SIGMADUT_NETMASK));
	   printf( "primary-dns,%u.%u.%u.%u,", 0, 0, 0, 0 );
	   printf("secondary-dns,%u.%u.%u.%u\n", 0, 0, 0, 0 );
	   printf("\n");

	}
    else
	{
       printf( "ip,%s,",   sigmadut_get_string(SIGMADUT_IPADDRESS));
       printf( "mask,%s,", sigmadut_get_string(SIGMADUT_NETMASK));
       printf( "primary-dns,%s,", sigmadut_get_string(SIGMADUT_PRIMARY_DNS));
       printf("secondary-dns,%s\n", sigmadut_get_string(SIGMADUT_SECONDARY_DNS));
       printf("\n");
	}
    return 0;
}

int sta_dump_wpa3_h2e_aps ( int argc, char *argv[], tlv_buffer_t** data)
{
    cy_wpa3_dump_h2e_ap_list();
    return 0;
}

int sta_set_power_save(int argc, char *argv[], tlv_buffer_t** data)
{
    int i = 1;

    while ( i <= (argc - 1 ))
    {
        if (strcasecmp ( argv[i], "powersave") == 0 )
        {
            if( strcasecmp( argv[i + 1], "on") == 0 )
            {
                if (he_enabled == 1)
                {
                    /* power save mode on PM 2 */
                    cywifi_set_ioctl_value( WLC_SET_PM, PM2_POWERSAVE_MODE);
                }
                else
                {
                    /* power save mode on PM 1 */
                    cywifi_enable_wifi_powersave();
                }
            }
            else
            {
                /* power save mode off */
                cywifi_disable_wifi_powersave();
            }
        }
        i = i + 2;
    }
    printf("\nstatus,COMPLETE\n");
    return CY_RSLT_SUCCESS;
}

int sta_get_parameter(int argc, char *argv[], tlv_buffer_t** data)
{
    int i = 1;
    int32_t rssi = 0;

    while ( i <= (argc - 1 ))
    {
        if (strcasecmp ( argv[i], "parameter") == 0 )
        {
            if( strcasecmp( argv[i + 1], "rssi") == 0 )
            {
                cywifi_get_rssi(&rssi);
            }
        }
        i = i + 2;
    }
    printf("\nstatus,COMPLETE,rssi,%ld\n", (long)rssi);
    return CY_RSLT_SUCCESS;
}

int sta_set_rfeature(int argc, char *argv[], tlv_buffer_t** data)
{
    int i = 1;
    char  ppdutxtype[10];
    uint32_t rualloctone = 0;
    uint32_t do_action = 0;

    sigmadut_set_twt_int(SIGMADUT_FLOWID, 1);
    while ( i <= (argc - 1 ))
    {
        if (strcasecmp ( argv[i], "ltf") == 0 )
        {
            sigmadut_set_ltf_gi_str(SIGMADUT_LTF, argv[i + 1]);
            do_action |= LTF_GI;
        }
        if (strcasecmp ( argv[i], "gi") == 0 )
        {
            sigmadut_set_ltf_gi_str(SIGMADUT_GI, argv[i + 1]);
            do_action |= LTF_GI;
        }
        if (strcasecmp ( argv[i], "NegotiationType") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_NEGOTIATIONTYPE, atoi(argv[i + 1]));
            if (strcasecmp( argv[i + 1], "0") == 0)
            {
                do_action |= TWT_ITWT;
            }
            else if (strcasecmp( argv[i + 1], "3") == 0)
            {
               do_action |= TWT_BTWT;
            }
            else
            {
               // Do Nothing
            }
        }
        if (strcasecmp ( argv[i], "TWT_Setup") == 0 )
        {
            if (strcasecmp( argv[i + 1], "request") == 0)
            {
                do_action |= TWT_SETUP;
            }
            else if (strcasecmp( argv[i + 1], "teardown") == 0)
            {
               do_action |= TWT_TEARDOWN;
            }
            else
            {
               // Do Nothing
            }
        }
        if (strcasecmp ( argv[i], "TWT_Operation") == 0 )
        {
            if (strcasecmp( argv[i + 1], "suspend") == 0)
            {
                do_action |= TWT_SUSPEND;
            }
            else if (strcasecmp( argv[i + 1], "resume") == 0)
            {
               do_action |= TWT_RESUME;
            }
            else
            {
               // Do Nothing
            }
        }
        if (strcasecmp ( argv[i], "SetupCommand") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_SETUPCOMMAND, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "TWT_Trigger") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_TWT_TRIGGER, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "FlowType") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_FLOWTYPE, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "WakeIntervalExp") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_WAKEINTERVALEXP, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "NominalMinWakeDur") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_NOMINALMINWAKEDUR, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "WakeIntervalMantissa") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_WAKEINTERVALMANTISSA, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "FlowID") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_FLOWID, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "TWT_ResumeDuration") == 0)
        {
            sigmadut_set_twt_int(SIGMADUT_RESUME_DURATION, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "BTWT_ID") == 0 )
        {
            sigmadut_set_twt_int(SIGMADUT_BTWT_ID, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "PPDUTXTYPE") == 0 )
        {
            do_action |= PPDUTXTYPE;
	    memcpy(ppdutxtype, argv[i + 1], strlen(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "RUAllocTone") == 0 )
        {
            do_action |= RUALLOCTONE;
            rualloctone = atoi(argv[i + 1]);
        }
        if (strcasecmp ( argv[i], "transmitOMI") == 0 )
        {
            do_action |= HE_OMI;
        }
        if (strcasecmp ( argv[i], "OMCtrl_TxNSTS") == 0 )
        {
            sigmadut_set_tx_omi_int(SIGMADUT_TXNSTS, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "OMCtrl_ChnlWidth") == 0 )
        {
            sigmadut_set_tx_omi_int(SIGMADUT_CHNLWIDTH, atoi(argv[i + 1]));
        }
        if (strcasecmp ( argv[i], "OMCtrl_ULMUDisable") == 0 )
        {
            sigmadut_set_tx_omi_int(SIGMADUT_ULMUDISABLE, atoi(argv[i + 1]));
        }
        if (strcasecmp( argv[i], "Ch_Pref_Num" ) == 0 )
        {
            sigmadut_set_mbo_int( SIGMADUT_MBO_CHANNEL, (uint8_t)atoi(argv[i+1]));
        }
        if (strcasecmp( argv[i], "Ch_Pref" ) == 0 )
        {
            if (strcasecmp( argv[i+1], "Clear" ) == 0 )
            {
                do_action |= MBO_CLEAR;
            }
            else
            {
                do_action |= MBO_ADD;
                sigmadut_set_mbo_int( SIGMADUT_MBO_PREFERENCE, (uint8_t)atoi(argv[i+1]));
            }
        }
        if (strcasecmp( argv[i], "Ch_Op_Class" ) == 0 )
        {
            sigmadut_set_mbo_int( SIGMADUT_MBO_OPCLASS, (uint8_t)atoi(argv[i+1]));
        }
        if (strcasecmp( argv[i], "Ch_Reason_Code" ) == 0 )
        {
            sigmadut_set_mbo_int( SIGMADUT_MBO_REASON, (uint8_t)atoi(argv[i+1]));
        }

        i = i + 2;
    }

    if ((do_action & TWT_SETUP) && (do_action & TWT_ITWT))
    {
        cywifi_itwt_setup();
    }
    else if ((do_action & TWT_TEARDOWN) && (do_action & TWT_ITWT))
    {
        cywifi_twt_teardown();
    }
    else if ((do_action & TWT_SETUP) && (do_action & TWT_BTWT))
    {
        cywifi_btwt_setup();
    }
    else if ((do_action & TWT_TEARDOWN) && (do_action & TWT_BTWT))
    {
        cywifi_twt_teardown();
    }
    else if (do_action & LTF_GI)
    {
        cywifi_set_ltf_gi();
    }
    else if (do_action & HE_OMI)
    {
        cywifi_he_omi();
    }
    else if (do_action & MBO_ADD)
    {
        cywifi_mbo_add_chan_pref();
    }
    else if (do_action & MBO_CLEAR)
    {
        cywifi_mbo_clear_chan_pref();
    }
    else if (do_action & TWT_SUSPEND)
    {
        cywifi_twt_operation(1);
    }
    else if (do_action & TWT_RESUME)
    {
        cywifi_twt_operation(0);
    }
    else if ( (do_action & PPDUTXTYPE) && (do_action & RUALLOCTONE) )
    {
        if (rualloctone == 242 && strcasecmp(ppdutxtype, "ER-SU"))
        {
            cywifi_set_iovar_value( IOVAR_STR_2G_RATE, HE_ERSU_PPDU_RUALLOCTONE_242 );
            cywifi_set_iovar_value( IOVAR_STR_5G_RATE, HE_ERSU_PPDU_RUALLOCTONE_242 );
            cywifi_set_iovar_value( IOVAR_STR_6G_RATE, HE_ERSU_PPDU_RUALLOCTONE_242 );
        }
    }
    else
    {
        printf("\nstatus,ERROR,errmsg,no action happens\n");
        return CY_RSLT_SUCCESS;
    }
    printf("\nstatus,COMPLETE\n");
    return CY_RSLT_SUCCESS;
}

int sta_set_ip_config ( int argc, char *argv[], tlv_buffer_t** data )
{
	int i = 1;
	char *ip_addr = NULL;
	char *gw = NULL;
	char *netmask = NULL;
    char *dhcp_str = NULL;

	while (i <= (argc - 1))
	{
       if ( strcmp( argv[i], "dhcp" ) == 0 )
       {
    	   sigmadut_set_string ( SIGMADUT_DHCP, argv[i+1]);
       }
       else if (strcmp( argv[i], "interface" ) == 0 )
       {
    	   sigmadut_set_string ( SIGMADUT_INTERFACE, argv[i+1]);
       }
       else if (strcmp( argv[i], "ip" ) == 0 )
       {
           sigmadut_set_string ( SIGMADUT_IPADDRESS, argv[i+1]);
       }
       else if (strcmp( argv[i], "mask" ) == 0 )
       {
    	   sigmadut_set_string ( SIGMADUT_NETMASK, argv[i+1]);
       }
       else if ( ( strcmp( argv[i], "defaultgateway" ) == 0 ) || ( strcmp( argv[i], "defaultGateway" ) == 0 ) )
       {
    	   sigmadut_set_string ( SIGMADUT_GATEWAY, argv[i+1]);
       }
       else if (strcmp( argv[i], "primary-dns" ) == 0 )
       {
           sigmadut_set_string( SIGMADUT_PRIMARY_DNS, argv[i+1]);
       }
       else if (strcmp( argv[i], "secondary-dns" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_SECONDARY_DNS, argv[i+1]);
       }
       i = i + 2;
	} /* end of while */

	ip_addr = sigmadut_get_string(SIGMADUT_IPADDRESS );
	gw = sigmadut_get_string(SIGMADUT_GATEWAY);
	netmask = sigmadut_get_string(SIGMADUT_NETMASK);
	dhcp_str = sigmadut_get_string(SIGMADUT_DHCP);

	if ( strcmp( dhcp_str, "1" ) == 0 )
	{
		cywifi_set_dhcp(1);
	}
	else
	{
		cywifi_set_dhcp(0);
		cywifi_set_network(ip_addr, netmask, gw);
	}

	printf("\nstatus,COMPLETE\n");

	return 0;
}

int sta_get_info ( int argc, char *argv[], tlv_buffer_t** data )
{
	cy_rslt_t result = CY_RSLT_SUCCESS;
	uint8_t mac[6] = {0};
	char buf[64] = {0};

    result = cywifi_get_macaddr(mac);
    if ( result != CY_RSLT_SUCCESS )
    {
    	printf(" STA MAC Address failed result:%u\n", (unsigned int)result );
    }
    cywifi_get_sw_version(buf, sizeof(buf));
	printf("\nstatus,COMPLETE,vendor,%s,model,%s,version,%s,firmware,%s,mac,%02X:%02X:%02X:%02X:%02X:%02X\n",
	        VENDOR, PLATFORM, SIGMA_DUT_VERSION, buf, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5] );
	return 0;
}

int sta_get_mac_address ( int argc, char *argv[], tlv_buffer_t** data )
{
	uint8_t mac[6] = {0};
	cy_rslt_t result;
	result = cywifi_get_macaddr(mac);
	printf("\nstatus,COMPLETE,mac,%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5] );
	return result;
}

int sta_is_connected  ( int argc, char *argv[], tlv_buffer_t** data )
{
	 if ( cywifi_is_interface_connected() == CY_RSLT_SUCCESS)
	 {
	     printf("\nstatus,COMPLETE,connected,1\n");
	 }
	 else
	 {
	     printf("\nstatus,COMPLETE,connected,0\n");
	 }
	return 0;
}

int sta_verify_ip_connection ( int argc, char *argv[], tlv_buffer_t** data )
{
   return 0;
}

int sta_get_bssid ( int argc, char *argv[], tlv_buffer_t** data )
{
    wl_bss_info_109_t *new_bssinfo = NULL;
    int res;

    if ( cywifi_is_interface_connected() == CY_RSLT_SUCCESS)
    {
        res = cywifi_get_bss_info( wlan_ioctl_buffer );
        if ( res == CY_RSLT_SUCCESS)
        {
            new_bssinfo = (wl_bss_info_109_t *)wlan_ioctl_buffer;
            printf("\nstatus,COMPLETE,bssid,%02X:%02X:%02X:%02X:%02X:%02X\n", new_bssinfo->BSSID.octet[0], new_bssinfo->BSSID.octet[1],
			new_bssinfo->BSSID.octet[2], new_bssinfo->BSSID.octet[3], new_bssinfo->BSSID.octet[4], new_bssinfo->BSSID.octet[5]);
        }
        else
        {
            printf("cywifi_get_ioctl_buffer WLC_GET_BSS_INFO Failed CY_RSLT TYPE = %lx MODULE = %lx CODE= %lx\n",
                    CY_RSLT_GET_TYPE(res), CY_RSLT_GET_MODULE(res), CY_RSLT_GET_CODE(res));
        }
    }
    else
    {
        printf("\nstatus,COMPLETE,bssid,00:00:00:00:00:00\n");
    }
    return 0;
}

int ca_get_version ( int argc, char *argv[], tlv_buffer_t** data )
{
	char buf[64] = {0};
	cywifi_get_sw_version(buf, sizeof(buf));
	printf( "\nstatus,COMPLETE,version,%s_%s\n", VENDOR, buf );
	return 0;
}

int device_get_info  ( int argc, char *argv[], tlv_buffer_t** data )
{
	uint32_t mfp = 0;
	char mfp_str[32] = {0};
	char *version;

	/* get MFP value */
	cywifi_get_iovar_value (IOVAR_STR_MFP, &mfp );
	set_mfptype(mfp_str, mfp);

	cywifi_get_wlan_version(wlan_version);
	version = strstr(wlan_version, "version ");
	version = strtok(version, " ");
	version = strtok(NULL, " ");

#ifdef COMPONENT_CAT5
	uint16_t wlan_platform;
    	wlan_platform = cywifi_get_wlan_platform();
    	printf("\nstatus,COMPLETE,vendor,%s,model,%d,version,%s\n", VENDOR, wlan_platform, version);
#else
	printf("\nstatus,COMPLETE,vendor,%s,model,%s,version,%s\n", VENDOR, PLATFORM, version);
#endif
	return 0;
}
/* sta_set_wpa3_pfn_network,SSID,SSIDa,passphrase,12345678123456781234567812345678*/
int sta_set_wpa3_pfn_network  ( int argc, char *argv[], tlv_buffer_t** data  )
{
    int i = 1;
    while (i <= (argc - 1))
    {
       if ( strcasecmp( argv[i], "SSID" ) == 0 )
       {
           cy_wpa3_set_ssid( argv[i+1]);
       }
       else if ( strcasecmp( argv[i], "passphrase" ) == 0 )
       {
           cy_wpa3_set_ssid_passphrase( argv[i+1]);
       }
       i = i + 2;
    }
    printf("\nstatus,COMPLETE\n");
    return 0;
}

/* sta_commit_wpa3_pfn_network */
int sta_commit_wpa3_pfn_network   ( int argc, char *argv[], tlv_buffer_t** data  )
{
    cy_wpa3_commit_wp3_pfn_network();
    return 0;
}

int sta_set_security   ( int argc, char *argv[], tlv_buffer_t** data )
{
	int i = 1;
    wiced_wep_key_t wep_key;

	while (i <= (argc - 1))
	{
       if ( strcasecmp( argv[i], "SSID" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_SSID, argv[i+1]);
       }
       else if ( strcasecmp( argv[i], "Type" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_SECURITY_TYPE, argv[i+1]);
    	   if( strcasecmp( argv[i+1], "eaptls") == 0 )
    	   {
    	       sta_set_eaptls ((argc-(i+1)), &argv[i+1], data);
    	       return 0;
    	   }
    	   else if( strcasecmp( argv[i+1], "eapttls") == 0 )
    	   {
    	       sta_set_eapttls ((argc-(i+1)), &argv[i+1], data);
    	       return 0;
    	   }
    	   else if( strcasecmp( argv[i+1], "eappeap") == 0)
    	   {
    	       sta_set_peap((argc-(i+1)), &argv[i+1], data);
    	       return 0;
    	   }
       }
       else if ( strcasecmp( argv[i], "KeyMgmtType" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, argv[i+1]);
       }
       else if ( strcasecmp( argv[i], "EncpType" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
       }
       else if ( strcasecmp( argv[i], "PairwiseCipher" ) == 0 )
       {
            sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
       }
       else if ( strcasecmp( argv[i], "passphrase" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_PASSPHRASE, argv[i+1]);
       }
       else if ( ( strcasecmp( argv[i], "key1" ) == 0 ) ||
                 ( strcasecmp( argv[i], "key2" ) == 0 ) ||
                 ( strcasecmp( argv[i], "key3" ) == 0 ) ||
                 ( strcasecmp( argv[i], "key4" ) == 0 ) )
       {
            int j = 0;
            wep_key.index = argv[i][3] - 0x31; // Index for key1 is 0, for key2 is 1, etc
            wep_key.length = ( strlen( argv[i+1] ) ) / 2;
            /* Convert key from hex characters to hex value */
            while ( j < ( wep_key.length * 2 ) )
            {
                wep_key.data[j/2] = ascii_to_hex(argv[i+1][j]) << 4;
                wep_key.data[j/2] = wep_key.data[j/2] | ascii_to_hex(argv[i+1][j+1]);
                j = j + 2;
            }
            sigmadut_set_wepkey(&wep_key);
       }
        else if ( strcasecmp( argv[i], "ProfileConnect" ) == 0 )
        {
        }

	   i = i + 2;
   }
   printf("\nstatus,COMPLETE\n");
   return 0;
}

int sta_set_eapttls   ( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1;

    while (i <= (argc - 1))
    {
        if ( strcasecmp( argv[i], "interface" ) == 0 )
        {
            sigmadut_set_string ( SIGMADUT_INTERFACE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "ssid" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_SSID, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "username" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_USERNAME, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "password" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_PASSWORD, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "trustedRootCA" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_TRUSTEDROOTCA, argv[i+1]);
            if ( strcasecmp(argv[i+1], "cas") == 0)
            {
                enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_HOSTAPD;
            }
            else if ( strcasecmp(argv[i+1], "caswin") == 0)
            {
                enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_MSFT;
            }
        }
        else if ( strcasecmp( argv[i], "EncpType" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "KeyMgmtType" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, argv[i+1]);
        }
        i = i + 2;
   }
   is_wpa_ent_security = true;
   sigmadut_set_eap_type(WPA_ENT_EAP_TYPE_TTLS);

   printf("\nstatus,COMPLETE\n");
   return 0;
}

int sta_set_eaptls   ( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1;

    while (i <= (argc - 1))
    {
        if ( strcasecmp( argv[i], "interface" ) == 0 )
        {
            sigmadut_set_string ( SIGMADUT_INTERFACE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "ssid" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_SSID, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "username" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_USERNAME, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "trustedRootCA" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_TRUSTEDROOTCA, argv[i+1]);

            if ( strcasecmp(argv[i+1], "cas") == 0)
            {
                enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_HOSTAPD;
            }
            else if ( strcasecmp(argv[i+1], "caswin") == 0)
            {
                enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_MSFT;
            }
        }
        else if ( strcasecmp( argv[i], "clientCertificate" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_CLIENTCERT, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "EncpType" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "KeyMgmtType" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, argv[i+1]);
        }
        i = i + 2;
    }
    is_wpa_ent_security = true;
    sigmadut_set_eap_type(WPA_ENT_EAP_TYPE_TLS);

    printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_set_peap    ( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1;

    while (i <= (argc - 1))
    {
        if ( strcasecmp( argv[i], "interface" ) == 0 )
        {
            sigmadut_set_string ( SIGMADUT_INTERFACE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "ssid" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_SSID, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "username" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_USERNAME, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "password" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_PASSWORD, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "trustedRootCA" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_TRUSTEDROOTCA, argv[i+1]);
            if ( strcasecmp(argv[i+1], "cas") == 0)
            {
                enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_HOSTAPD;
            }
            else if ( strcasecmp(argv[i+1], "caswin") == 0)
            {
                enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_MSFT;
            }
        }
        else if ( strcasecmp( argv[i], "clientCertificate" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_CLIENTCERT, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "EncpType" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "KeyMgmtType" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "innerEAP" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_INNEREAP, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "peapVersion") == 0 )
        {
            sigmadut_set_string( SIGMADUT_PEAPVERSION, argv[i+1]);
        }
        i = i + 2;
    }
    is_wpa_ent_security = true;
    sigmadut_set_eap_type(WPA_ENT_EAP_TYPE_PEAP);

    printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_set_eapaka   ( int argc, char *argv[], tlv_buffer_t** data )
{
    printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_set_eapsim   ( int argc, char *argv[], tlv_buffer_t** data )
{
    printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_set_eapfast   ( int argc, char *argv[], tlv_buffer_t** data )
{
    printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_set_systime ( int argc, char *argv[], tlv_buffer_t** data )
{
    cy_rslt_t result;
    wifi_cert_time_t new_date = {0};
    int i = 1;

    while (i <= (argc - 1))
    {
        if ( strcasecmp( argv[i], "month" ) == 0 )
        {
            sigmadut_set_time_date_int( SIGMADUT_MONTH, atoi(argv[i+1]));
            new_date.tm_month = atoi(argv[i+1]) - 1;
        }
        else if ( strcasecmp( argv[i], "date" ) == 0 )
        {
            sigmadut_set_time_date_int( SIGMADUT_DATE, atoi(argv[i+1]));
            new_date.tm_mday = atoi(argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "year" ) == 0 )
        {
            sigmadut_set_time_date_int( SIGMADUT_YEAR, atoi(argv[i+1]));
            new_date.tm_year = atoi(argv[i+1]) - WIFI_CERT_TIME_YEAR_BASE;
        }
        else if ( strcasecmp( argv[i], "hours" ) == 0 )
        {
            sigmadut_set_time_date_int( SIGMADUT_HOURS, atoi(argv[i+1]));
            new_date.tm_hour = atoi(argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "minutes" ) == 0 )
        {
            sigmadut_set_time_date_int( SIGMADUT_MINUTES, atoi(argv[i+1]));
            new_date.tm_min = atoi(argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "seconds" ) == 0 )
        {
            sigmadut_set_time_date_int( SIGMADUT_SECONDS, atoi(argv[i+1]));
            new_date.tm_sec = atoi(argv[i+1]);
        }
        i = i + 2;
    }
    result = cywifi_set_system_time(&new_date);
    if ( result != CY_RSLT_SUCCESS)
    {
        printf("cywifi_set_system_time failed :%u\n", (unsigned int)result);
    }

    if ( cywifi_is_interface_connected() == CY_RSLT_SUCCESS)
    {
        sta_disconnect(0, NULL, NULL);
    }
    printf("\nstatus,COMPLETE\n");
    return 0;
}

int device_list_interfaces  ( int argc, char *argv[], tlv_buffer_t** data )
{
	printf("\nstatus,COMPLETE,interfaceType,802.11,interfaceID,wlan0\n");
	return 0;
}

int sta_set_encryption   ( int argc, char *argv[],  tlv_buffer_t** data )
{
	int i = 1;
	wiced_wep_key_t wep_key;

	sigmadut_set_string( SIGMADUT_SECURITY_TYPE, TEST_SECTYPE_DEFAULT);

	while (i <= (argc - 1))
	{
		if ( strcmp( argv[i], "ssid" ) == 0 )
	    {
			sigmadut_set_string( SIGMADUT_SSID, argv[i+1]);
	    }
	    else if ( ( strcmp( argv[i], "encptype" ) == 0 ) || ( strcmp( argv[i], "encpType" ) == 0 ) )
	    {
	    	sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
	    }
	    else if ( ( strcmp( argv[i], "key1" ) == 0 ) ||
	              ( strcmp( argv[i], "key2" ) == 0 ) ||
	              ( strcmp( argv[i], "key3" ) == 0 ) ||
	              ( strcmp( argv[i], "key4" ) == 0 ) )
	    {
	          int j = 0;
	          wep_key.index = argv[i][3] - 0x31; // Index for key1 is 0, for key2 is 1, etc
	          wep_key.length = ( strlen( argv[i+1] ) ) / 2;
	         /* Convert key from hex characters to hex value */
	         while ( j < ( wep_key.length * 2 ) )
	         {
	             wep_key.data[j/2] = ascii_to_hex(argv[i+1][j]) << 4;
	             wep_key.data[j/2] = wep_key.data[j/2] | ascii_to_hex(argv[i+1][j+1]);
	            j = j + 2;
	         }
	         sigmadut_set_wepkey(&wep_key);
	    }

	    i = i + 2;
	}

	printf("\nstatus,COMPLETE\n");

	return 0;
}

/*!
******************************************************************************
Convert an ascii hex representation of 0..F (or f) to a hex number in the range 0x00 to 0x0f.

\return hex value of the ascii hex representation

\param[in] value  ascii value of a hexadecimal digit
*/

uint8_t ascii_to_hex(uint8_t value)
{
    if ((value >= 0x30) && (value <= 0x39)) // Hex number between 0 and 9
    {
        return (value - 0x30);
    }

    if ((value >= 0x41) && (value <= 0x46)) // Hex number between A and F
    {
        return (value - 0x37);
    }

    if ((value >= 0x61) && (value <= 0x66))
    {
        return (value - 0x57);
    }
    return CY_RSLT_SUCCESS;
}

int sta_set_pwrsave  ( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1;
    char *pwrsave = NULL;

    while (i <= (argc - 1))
    {
       if ( strcmp( argv[i], "mode" ) == 0 )
       {
           sigmadut_set_string( SIGMADUT_PWRSAVE, argv[i+1]);
       }
       i = i + 2;
    }/* end of while */

    pwrsave = sigmadut_get_string(SIGMADUT_PWRSAVE);
    if (strcasecmp( pwrsave, "off" ) == 0 )
    {
       /* power save mode off */
       cywifi_disable_wifi_powersave();
    }
    else if ( strcasecmp(pwrsave, "on") == 0 )
    {
       /* power save mode on */
       cywifi_enable_wifi_powersave();
    }
    else
    {
        printf("\nUknown Power save mode\n");
    }
    printf("\nstatus,COMPLETE\n");
    return CY_RSLT_SUCCESS;
}


int sta_set_psk  ( int argc, char *argv[], tlv_buffer_t** data )
{
	int i = 1;

	sigmadut_set_string( SIGMADUT_SECURITY_TYPE, TEST_SECTYPE_DEFAULT);
    while (i <= (argc - 1))
    {
       if ( strcmp( argv[i], "ssid" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_SSID, argv[i+1]);
       }
       else if (strcmp( argv[i], "passphrase" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_PASSPHRASE, argv[i+1]);
       }
       else if ( ( strcmp( argv[i], "encptype" ) == 0 ) || ( strcmp( argv[i], "encpType" ) == 0 ) )
       {
    	   sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
       }
       else if (strcmp( argv[i], "keymgmttype" ) == 0 )
       {
    	   sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, argv[i+1]);
       }
       else if (strcasecmp( argv[i], "PMF" ) == 0 )
       {
    	   sigmadut_set_string(SIGMADUT_PMF, argv[i+1]);
       }
       i = i + 2;
   }/* end of while */

   printf("\nstatus,COMPLETE\n");
   return CY_RSLT_SUCCESS;
}

int sta_get_wlan_status ( int argc, char *argv[], tlv_buffer_t** data )
{
    int ret = CY_RSLT_SUCCESS;
    int rssi = 0, snr = 0, noise = 0, flags = 0;
    uint32_t pwrsave = 0;

    if ( cywifi_is_interface_connected() == CY_RSLT_SUCCESS)
    {
        ret = cywifi_get_bss_info( wlan_ioctl_buffer );
        if ( ret == CY_RSLT_SUCCESS)
        {
            cywifi_dump_bssinfo(wlan_ioctl_buffer);
        }
        else
        {
            printf("cywifi_get_ioctl_buffer WLC_GET_BSS_INFO Failed CY_RSLT TYPE = %lx MODULE = %lx CODE= %lx\n",
                    CY_RSLT_GET_TYPE(ret), CY_RSLT_GET_MODULE(ret), CY_RSLT_GET_CODE(ret));
        }
    }
    else
    {
       printf("\nSSID:\"%s\"", "" );
       printf("\nMode: <unknown> ");
       printf("RSSI: :%d dBm SNR: %d dB noise: %d dBm Flags: %d",
               rssi, snr, noise, flags);
       printf("\nBSSID: 00:00:00:00:00:00");
       printf(" Capability: \n");
       printf("Supported Rates: \n");
       printf("Extended Capabilities: \n");
       printf("VHT Capable: \n");
       printf("Chanspec: \n");
       printf("Primary channel:    \n");
       printf("HT Capabilities:    \n");
       printf("Supported HT MCS:   \n");
       printf("Negotiated VHT MCS: \n");
       printf("NSS1 :              \n");
    }
    cywifi_get_wifi_powersave(&pwrsave);
    printf("WiFi-power save:%d    \n", (int)pwrsave);

    return CY_RSLT_SUCCESS;
}

int sta_associate  ( int argc, char *argv[], tlv_buffer_t** data )
{
	int auth_type;
	uint8_t  wep_key_buffer[64] = { 0 };
    uint32_t mfp_value = WL_MFP_NONE;
	char *encptype = NULL;
    char *keymgmt_type = NULL;
    char *sec_type = NULL;
    char *passphrase = NULL;
    char *ssid = NULL;
    char *pmf = NULL;
    cy_rslt_t ret;
    char *dhcp_str = NULL;
#ifndef H1CP_SUPPORT
    int retry = 0;
#endif

    int i = 1; /* to skip the first argv "sta_associate" */

    while (i <= (argc - 1))
    {
        if ( strcasecmp( argv[i], "bssid" ) == 0 )
        {
            sigmadut_set_string( SIGMADUT_BSSID, argv[i+1]);
            break;
        }
        i = i + 2;
    }

	encptype = sigmadut_get_string(SIGMADUT_ENCRYPTION_TYPE);
	keymgmt_type = sigmadut_get_string(SIGMADUT_KEYMGMT_TYPE);
	sec_type = sigmadut_get_string(SIGMADUT_SECURITY_TYPE);
	ssid = sigmadut_get_string(SIGMADUT_SSID);
	pmf = sigmadut_get_string(SIGMADUT_PMF);
	passphrase = sigmadut_get_string(SIGMADUT_PASSPHRASE);
#ifdef QUICK_TRACK_SUPPORT
	auth_type = cywifi_get_qt_authtype( encptype, keymgmt_type, sec_type, pmf );
#else
	auth_type = cywifi_get_authtype( encptype, keymgmt_type, sec_type, pmf );
#endif
	ret = cywifi_set_auth_credentials ( &auth_type, wep_key_buffer, argc );

	if ( ret != CY_RSLT_SUCCESS )
	{
	   printf("failed to set auth ret:%u\n", (unsigned int)ret);
	}

    /* get MFP value and set it */
	if (get_mfptype( pmf, &mfp_value ))
	{
	    cywifi_set_iovar_value( IOVAR_STR_MFP, mfp_value );
	}

	/* WAR for cert N 5.2.52 */
	if (strcmp( encptype, "wep" ) == 0)
	{
		cywifi_scan ( NULL, 0);
	}
	if ( is_wpa_ent_security == true)
	{
#ifndef H1CP_SUPPORT
	   cywifi_set_enterprise_security_cert(enterprise_security_client_cert_type);
#ifndef H1CP_SUPPORT
	   ret = cywifi_update_enterpise_security_params(&ent_params, sigmadut_enterprise_sec_handle);
	   if ( ret != CY_RSLT_SUCCESS)
	   {
	       /* return CY_RSLT_SUCCESS as WFA cert have negative tests which will not have
	        * AP with SSID and returning -1 causes command console to not respond to
	        * next commands
	        */
	      return CY_RSLT_SUCCESS;
	   }
#endif
	   sigmadut_get_enterprise_security_handle(&sigmadut_enterprise_sec_handle);
	   cywifi_update_staticip_settings();
	   while (retry < 1)
	   {
	       ret = cy_enterprise_security_join(sigmadut_enterprise_sec_handle);
	       if (ret != CY_RSLT_SUCCESS)
	       {
	           retry++;
	       }
	       else
	       {
	           break;
	       }
	   }
	   if (ret != CY_RSLT_SUCCESS)
       {
           // printf("\nConnection error: %d\n", ret);
           /* return CY_RSLT_SUCCESS as WFA cert have negative tests which will not have
            * AP with SSID and returning -1 causes command console to not respond to
            * next commands
            */
           printf("Enterprise Join Failed CY_RSLT TYPE = %lx MODULE = %lx CODE= %lx\n",
                  CY_RSLT_GET_TYPE(ret), CY_RSLT_GET_MODULE(ret), CY_RSLT_GET_CODE(ret));

           return CY_RSLT_SUCCESS;
       }
#endif
	}
	else
	{
#ifdef H1CP_SUPPORT
	    ret = cywifi_set_iovar_value(IOVAR_STR_TXOP11N_WAR, 1);

	    if (ret != CY_RSLT_SUCCESS)
	    {
	        printf("txop_11n_cert_war enable failed \n");
	    }

	    ret = cywifi_set_iovar_value(IOVAR_STR_STBC_RX, 0);

	    if (ret != CY_RSLT_SUCCESS)
	    {
	        printf("stbc_rx disable failed \n");
	    }
#endif /* H1CP_SUPPORT */

	    ret = cywifi_connect_ap( ssid, (const char *)passphrase, auth_type );

	    if (ret != CY_RSLT_SUCCESS)
	    {
	       // printf("\nConnection error: %d\n", ret);
	        /* return CY_RSLT_SUCCESS as WFA cert have negative tests which will not have
	         * AP with SSID and returning -1 causes command console to not repsond to
	         * next commands
	         */
#ifdef QUICK_TRACK_SUPPORT
            printf( "\nstatus,COMPLETE,assoc,NG,END\n" );
#else
            printf( "\nstatus,COMPLETE\n" );
#endif
			return CY_RSLT_SUCCESS;
	    }
	    is_sta_up = true;
	}
#ifdef QUICK_TRACK_SUPPORT
    printf( "\nstatus,COMPLETE,assoc,OK,END\n" );
#else
    printf( "\nstatus,COMPLETE\n" );
#endif
	dhcp_str = sigmadut_get_string(SIGMADUT_DHCP);

	if ( dhcp_str == NULL )
	{
	    return CY_RSLT_MW_ERROR;
	}

    /* get IP settings and store into sigmadut object */
    cywifi_get_ip_settings();

    if ( wifi_get_ac_params_sta(ac_params) != CMD_SUCCESS)
	{
	    printf( "Error reading EDCF AC Parameters\n");
	}
	wifi_prioritize_acparams( ac_params, ac_priority ); // Re-prioritize access categories to match AP configuration

	//for ( i = 0; i < AC_COUNT; i++ )
	//{
	   // printf("index i:%d ac_priority[i]=%d\n", i, ac_priority[i]);
	//}
	return CY_RSLT_SUCCESS;
}


int wifi_get_ac_params_sta(edcf_acparam_t *ac_param )
{
    uint8_t buffer[64] = {0};
	int ret = CY_RSLT_SUCCESS;

	memcpy((char *)buffer, IOVAR_STR_AC_PARAMS_STA, strlen(IOVAR_STR_AC_PARAMS_STA));

	ret = cywifi_get_ioctl_buffer( WLC_GET_VAR, buffer, sizeof(buffer));

	if ( ret != CY_RSLT_SUCCESS)
	{
	    printf( "Error reading EDCF AC Parameters\n");
	}
	memcpy(ac_param, buffer, sizeof(edcf_acparam_t) * AC_COUNT);
	//printf("ac_param values ACI:%d ECW:%d TXOP:%d\n", ac_param->ACI, ac_param->ECW, ac_param->TXOP);
	return ret;
}

void wifi_prioritize_acparams ( edcf_acparam_t * acp, int * priority )
{
	 int aci;
	 int aifsn;
	 int ecwmin;
	 int ecwmax;
	 int acm;
	 int ranking_basis[AC_COUNT];
	 int *p;
	 int i;

	 p = priority;

	 for (aci = 0; aci < AC_COUNT; aci++, acp++, p++)
	 {
	    aifsn  = acp->ACI & EDCF_AIFSN_MASK;
	    acm = (acp->ACI & EDCF_ACM_MASK) ? 1 : 0;
	    ecwmin = acp->ECW & EDCF_ECWMIN_MASK;
	    ecwmax = (acp->ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
	    ranking_basis[aci] = aifsn + ecwmin + ecwmax; /* Default AC_VO will be the littlest ranking value */

	    /* If ACM is set, STA can't use this AC as per 802.11. Change the ranking to BE */
	    if ((aci > 0) && (acm == 1))
	         ranking_basis[aci] = ranking_basis[0];
	    *p = 1; /* Initialise priority starting at 1 */
	 }

	 p = priority;

	 /* Primitive ranking method which works for AC priority swapping when values for cwmin, cwmax and aifsn are varied */
	 for (aci = 0; aci < AC_COUNT; aci++, p++) /* Compare each ACI against each other ACI */
	 {
	     for (i = 0; i < AC_COUNT; i++)
	      {
	         if ( i != aci )
	          {
	              /* Smaller ranking value has higher priority, so increment priority for each ACI which has a higher ranking value */
	              if (ranking_basis[aci] < ranking_basis[i])
	              {
	                  *p = *p + 1;
	              }
	           }
	      }
	 }

#ifndef OS_THREADX
	// For FreeRTOS, Now invert so that highest priority is the lowest number, e.g. for default settings voice will be priority 1 and background will be priority 4
	// For ThreadX, this inverse is not needed, in ThreadX lower number, gets higher priority
	p = priority;
	for (i = 0; i < AC_COUNT; i++, p++)
	{
	    *p = AC_COUNT + 1 - *p;
	}
#endif /* OS_THREADX */
}

int sta_preset_testparameters  ( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1;
    int prog = 0;
    cy_rslt_t result = CY_RSLT_SUCCESS;

    is_wpa_ent_security = false;

    while (i <= (argc - 1))
    {
        if ( strcasecmp( argv[i], "program" ) == 0 )
        {
            if ( ( strcasecmp( argv[i+1], "MBO" ) == 0 ) || ( strcasecmp( argv[i+1], "HE" ) == 0 ) )
            {
                prog = MBO;
            }
        }
        else if (strcasecmp( argv[i], "Ch_Pref_Num" ) == 0 )
        {
            sigmadut_set_mbo_int( SIGMADUT_MBO_CHANNEL, (uint8_t)atoi(argv[i+1]));
        }
        else if (strcasecmp( argv[i], "Ch_Pref" ) == 0 )
        {
            sigmadut_set_mbo_int( SIGMADUT_MBO_PREFERENCE, (uint8_t)atoi(argv[i+1]));
        }
        else if (strcasecmp( argv[i], "Ch_Op_Class" ) == 0 )
        {
            sigmadut_set_mbo_int( SIGMADUT_MBO_OPCLASS, (uint8_t)atoi(argv[i+1]));
        }
        else if (strcasecmp( argv[i], "Ch_Reason_Code" ) == 0 )
        {
            sigmadut_set_mbo_int( SIGMADUT_MBO_REASON, (uint8_t)atoi(argv[i+1]));
        }

        i = i + 2;
    } /* end of while */

    switch(prog)
    {
        case MBO:
            result = cywifi_mbo_add_chan_pref();
            break;
        default:
            break;
    }

    if ( result != CY_RSLT_SUCCESS)
    {
        printf("sta_preset_testparameters failed :%u\n", (unsigned int)result);
    }

	printf("\nstatus,COMPLETE\n");
	return 0;
}

int traffic_send_ping   ( int argc, char *argv[], tlv_buffer_t** data )
{
	++stream_id;

	spawn_ping_thread( wifi_traffic_send_ping, argv );

	cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_10MS ); /* Delay so the arguments don't get wiped out otherwise we need to copy argv */

	printf("\nstatus,COMPLETE,streamID,%u\n", ( unsigned int )stream_id);

	return 0;
}

int traffic_stop_ping  ( int argc, char *argv[], tlv_buffer_t** data )
{
	wifi_traffic_stop_ping();
	wifi_handle_ping_thread_exit();
	return 0;
}

int traffic_agent_config  ( int argc, char *argv[], tlv_buffer_t** data )
{
	int i = 1;

	++stream_id;

	int stream_index = sigmadut_find_unallocated_stream_table_entry();
	if ( stream_index == -1 )
	{
	     printf("\nstatus,ERROR\n");
	     return 0;
	}

	sigmadut_set_streamid (stream_index, stream_id );
	while (i <= (argc - 1))
	{
		if ( strcmp( argv[i], "profile" ) == 0 )
	    {
	         if ( strcmp( argv[i+1], "IPTV" ) == 0 )
	         {
	             sigmadut_set_traffic_profile(stream_index, PROFILE_IPTV);
	         }
	         else if ( strcmp( argv[i+1], "Transaction" ) == 0 )
	         {
	        	 sigmadut_set_traffic_profile(stream_index, PROFILE_TRANSACTION);
	         }
	         else if ( strcmp( argv[i+1], "Multicast" ) == 0 )
	         {
	        	 sigmadut_set_traffic_profile(stream_index, PROFILE_MULTICAST);
	         }
	      // printf("\nprofile %u\n", sigmadut_get_traffic_profile(stream_index));
	    }
	    else if (strcmp( argv[i], "direction" ) == 0 )
	    {
	        if ( strcmp( argv[i+1], "send" ) == 0 )
	        {
	        	 sigmadut_set_traffic_direction (stream_index, TRAFFIC_SEND);
	        }
	        else if ( strcmp( argv[i+1], "receive" ) == 0 )
	        {
	        	 sigmadut_set_traffic_direction (stream_index, TRAFFIC_RECV);
	        }
	      //printf("\ndirection %u\n", sigmadut_get_traffic_direction(stream_index));
	     }
	     else if (strcmp( argv[i], "source" ) == 0 )
	     {
	    	 sigmadut_set_traffic_src_ipaddress(stream_index, argv[i+1]);
	         //printf("\nsource ip:%s\n", sigmadut_get_traffic_ip_address(SIGMADUT_TRAFFIC_SRC_IPADDRESS, stream_index));
	     }
	     else if ( strcmp( argv[i], "sourcePort" ) == 0 )
	     {
	    	 sigmadut_set_traffic_src_port ( stream_index, (uint16_t)atoi(argv[i+1]));
	         // printf("\nsource port: %u\n", sigmadut_get_traffic_port(SIGMADUT_TRAFFIC_SRC_PORT, stream_index));
	     }
	     else if ( strcmp( argv[i], "destination" ) == 0 )
	     {
	    	 sigmadut_set_traffic_dst_ipaddress(stream_index, argv[i+1]);
	         //printf("\ndestination ip:%s\n", sigmadut_get_traffic_ip_address(SIGMADUT_TRAFFIC_DST_IPADDRESS, stream_index));
	     }
	     else if ( strcmp( argv[i], "destinationPort" ) == 0 )
	     {
	    	 sigmadut_set_traffic_dst_port ( stream_index, (uint16_t)atoi(argv[i+1]));
	        // printf("\ndestination port: %u\n", sigmadut_get_traffic_port(SIGMADUT_TRAFFIC_DST_PORT, stream_index));
	     }
	     else if ( strcmp( argv[i], "trafficClass" ) == 0 )
	     {
	         if ( strcmp( argv[i+1], "BestEffort" ) == 0 )
	         {
	        	 sigmadut_set_traffic_class(stream_index, WMM_AC_BE);
	         }
	         if ( strcmp( argv[i+1], "Background" ) == 0 )
	         {
	        	 sigmadut_set_traffic_class(stream_index, WMM_AC_BK);
	         }
	         else if ( strcmp( argv[i+1], "Video" ) == 0 )
	         {
	        	 sigmadut_set_traffic_class(stream_index, WMM_AC_VI);
	         }
	        else if ( strcmp( argv[i+1], "Voice" ) == 0 )
	        {
	        	 sigmadut_set_traffic_class(stream_index, WMM_AC_VO);
	        }
	   //      printf("\ntraffic class: %u\n", (int)sigmadut_get_traffic_class(stream_index));
	    }
	   else if ( strcmp( argv[i], "payloadSize" ) == 0 )
	   {
		     sigmadut_set_traffic_payloadsize(stream_index, (int)atoi(argv[i+1]));
	         //printf("\npayload size: %d\n", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_PAYLOAD_SIZE, stream_index));
	   }
	   else if ( strcmp( argv[i], "duration" ) == 0 )
	   {
		     sigmadut_set_traffic_duration(stream_index, (int)atoi(argv[i+1]));
		     //printf("\nduration: %d\n", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_DURATION, stream_index));
	   }
	   else if ( strcmp( argv[i], "frameRate" ) == 0 )
	   {
		     sigmadut_set_traffic_framerate(stream_index, (int)atoi(argv[i+1]));
	        //printf("\nframe rate %d\n", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_FRAME_RATE, stream_index));
	   }
	   else if ( strcmp( argv[i], "HTI") == 0 )
	   {
	        if ( strcmp (argv[i+1], "on") == 0 )
	        {
	            /* For 11 AC MAX throughput set the frame burst and MPDU per AMPDU
	             *
	             */
	        	cywifi_set_ioctl_value( WLC_SET_FAKEFRAG, 1);
	        	cywifi_set_iovar_value( IOVAR_STR_MPDU_PER_AMPDU, 64);
	        }
	   }
	   i = i + 2;
    }

    printf("\nstatus,COMPLETE,streamID,%u\n", (unsigned int)stream_id );
	return 0;
}

int traffic_agent_reset   ( int argc, char *argv[], tlv_buffer_t** data )
{
    cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_10MS);// Delay in case IP stack messages have not been delivered
	sigmadut_init_stream_table();
	if(sigmadut_ping_thread_running == true) {
		wifi_handle_ping_thread_exit();
	}
	printf("status,COMPLETE\n" );
	return 0;
}


void wifi_handle_ping_thread_exit()
{
	(void)cy_rtos_join_thread((cy_thread_t *)&ping_thread_details.thread_handle);
	memset(&ping_thread_details, 0, sizeof(sigmadut_ping_thread_details_t));
}

int traffic_agent_send  ( int argc, char *argv[], tlv_buffer_t** data )
{
	uint32_t current_time = 0;
	char* p;
	int ret;
	int idx = 0;
	uint32_t stoptime = 0;
    int arr[5];
    int arridx = 0;

	current_time = GET_CURRENT_TIME;

	// Select which streams are to be started
	p = strtok( argv[2], "' '" ); // StreamIDs are delimited by spaces, not commas
	while ( p )
	{
        arr[arridx++] = (int) atoi( p );
        p = strtok( NULL, " ");
	}

	while(arridx-- > 0)
	{
	    //printf("TS %d send\n", arr[arridx]);
	    idx = sigmadut_find_stream_table_entry(arr[arridx]); // Find stream table entry
	    if ( idx == -1 )
	    {
	         printf("status,ERROR\n");
	         return 0;
	    }
	    sigmadut_set_traffic_stream_enable(idx, 1);
	    stoptime = current_time + ( sigmadut_get_traffic_duration(idx) * 1000 );
        sigmadut_set_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, idx, (int)stoptime );

	    if ( (ret = spawn_ts_thread( udp_tx, sigmadut_get_traffic_stream_instance(idx), idx )) != 0 )
	    {
	         printf( "spawn thread failed ret:%d status,ERROR", ret );
	    }
	   // p = strtok( NULL, " "); // Get the next StreamID
	} /* end of while */
	return 0;
}

int spawn_ts_thread( int (*ts_function)(traffic_stream_t *ts ), traffic_stream_t *ts, int idx)
{
    int ret = 0;
    cy_rslt_t result = CY_RSLT_SUCCESS;
    int native_prio = 0; /* native priority based on SDK */

	detail[idx].ts_function = ts_function;
	detail[idx].ts = ts;
	detail[idx].ts->thread_ptr = &detail;
	detail[idx].ts->sigmadut = (void *)NULL; //&sigmadut;
	detail[idx].ts->networkinteface = NULL; //wifi_intf;
	detail[idx].stack_mem = NULL;

    if ( ts->direction == TRAFFIC_SEND )
    {

        result = cywifi_get_native_priority( ts, ac_priority[ts->ac], tx_ac_priority_num[ts->ac], &native_prio );
        tx_ac_priority_num[ts->ac]++;
        //printf("\n ac_priority[ts->ac]:%d ts->ac:%d native priority of TX thread:%d\n", ac_priority[ts->ac], ts->ac, native_prio);
    }
    else if ( ts->direction == TRAFFIC_RECV )
    {
        result = cywifi_get_native_priority( ts, ac_priority[ts->ac], rx_ac_priority_num[ts->ac], &native_prio );
        rx_ac_priority_num[ts->ac]++;
        //printf("\n ac_priority[ts->ac]:%d [ts->ac]:%d native priority of RX thread:%d\n", ac_priority[ts->ac], ts->ac, native_prio);
    }

    result = cy_rtos_create_thread((cy_thread_t *)&detail[idx].thread_handle, (cy_thread_entry_fn_t)ts_thread_wrapper, "UDP Thread", detail[idx].stack_mem,
    				TS_THREAD_STACK_SIZE, (cy_thread_priority_t)native_prio, (cy_thread_arg_t)&detail[idx]);

    if( result != CY_RSLT_SUCCESS)
    {
       printf(" cy_rtos_create_thread failed %lu \n", (unsigned long)result);
       return -1;
    }
    cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_10MS);
    return ret;
}


int spawn_ping_thread( int (*ping_function)(void *arg ), void *arg )
{
	cy_rslt_t result = CY_RSLT_SUCCESS;

	if (sigmadut_ping_thread_running == true )
	{
	    /* ping thread is already running return */
	    return 0;
	}

	memset(&ping_thread_details, 0, sizeof(sigmadut_ping_thread_details_t));

	ping_thread_details.ping_function = ping_function;
	ping_thread_details.thread_details = &ping_thread_details;
	ping_thread_details.sigmadut = (void *)NULL; //&sigmadut;
	ping_thread_details.network_interface = NULL; //wifi_intf;
	ping_thread_details.arg = arg;
	ping_thread_details.stack_mem = NULL;

	result = cy_rtos_create_thread(&ping_thread_details.thread_handle, (cy_thread_entry_fn_t)ping_thread_wrapper, "Ping Thread", ping_thread_details.stack_mem,
				TS_THREAD_STACK_SIZE, CY_RTOS_PRIORITY_NORMAL, (cy_thread_arg_t)&ping_thread_details);

	if( result != CY_RSLT_SUCCESS)
	{
	    printf(" cy_rtos_create_thread failed %lu \n", (unsigned long)result);
		memset(&ping_thread_details, 0, sizeof(sigmadut_ping_thread_details_t));
	    return -1;
	}
	return 0;
}

void ping_thread_wrapper( void *arg )
{
    sigmadut_ping_thread_details_t* detail = (sigmadut_ping_thread_details_t*) arg;
    sigmadut_ping_thread_running = true;
	detail->ping_function( detail );
	sigmadut_ping_thread_running = false;
	TRAFFIC_END_OF_THREAD(NULL);
}

void ts_thread_wrapper( void *arg )
{
    thread_details_t* detail = (thread_details_t*) arg;

    if ( ( detail->ts->direction == TRAFFIC_RECV ) || ( detail->ts->direction == TRAFFIC_ANY ))
    {
        printf("\nstatus,COMPLETE\n" );
    }
    detail->ts_function( detail->ts );

    // All threads will exit to here. We use a mutex to ensure only one tx thread prints results for all tx threads that ran concurrently
    //printf( "Thread 0x%08X exited with return value %d\n", (unsigned int)detail, result );
    cy_rtos_delay_milliseconds( SIGMA_AGENT_DELAY_1S ); // Delay to ensure the linux endpoint is ready and all local transmit streams are finished
    traffic_stream_t *ts = detail->ts;

    if ( ts->direction == TRAFFIC_SEND )
    {
    	cy_rtos_get_mutex(&tx_done_mutex, CY_RTOS_NEVER_TIMEOUT);
    }

    if ( ts->direction == TRAFFIC_SEND && ts->enabled ) // Print results from all tx streams terminating at the same time
    {
        int i;

        // Disable all transmit streams that finish at the same time so we only print one set of results
        for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
        {
            if ( ts->stop_time == sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, i ) && sigmadut_get_traffic_direction(i) == TRAFFIC_SEND)
            {
            	sigmadut_set_traffic_stream_enable ( i, 0 );
            }
        }
        cy_rtos_delay_milliseconds( SIGMA_AGENT_DELAY_1S ); // Delay to ensure the linux endpoint is ready and all local transmit streams are finished

      	// Print results for all transmit streams that finished at the same time
        printf("status,COMPLETE,streamID," );

        for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++)
        {
            if ( ts->stop_time == sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, i )  && sigmadut_get_traffic_direction(i) == TRAFFIC_SEND )
            {
               printf( "%u ", (unsigned int)sigmadut_get_traffic_streamid(i) );
            }
        }

       	printf( ",txFrames," );
        for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
       	{
        	if ( ts->stop_time == sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, i )  && sigmadut_get_traffic_direction(i) == TRAFFIC_SEND )
            {
        		if ( sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_FRAME_SENT, i) == 0)
                {
        			sigmadut_set_traffic_int(SIGMADUT_TRAFFIC_FRAME_SENT, i, 1); // If it's not set to a positive number the Sigma script locks up
          	    }
                printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_FRAME_SENT, i) );
            }
        }

       	printf( ",rxFrames," );
        for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
        {
        	if ( ts->stop_time == sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, i )  && sigmadut_get_traffic_direction(i) == TRAFFIC_SEND )
            {
                printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_FRAME_RECVD, i));
            }
        }

       	printf( ",txPayloadBytes," );
        for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
        {
        	if ( ts->stop_time == sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, i )  && sigmadut_get_traffic_direction(i) == TRAFFIC_SEND )
            {
               printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_BYTES_SENT, i) );
            }
        }

       	printf( ",rxPayloadBytes," );
        for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
        {
        	if ( ts->stop_time == sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, i )  && sigmadut_get_traffic_direction(i) == TRAFFIC_SEND )
            {
               printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_BYTES_RECVD, i) );
            }
        }

       	printf( ",outOfSequenceFrames," );
        for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
        {
        	if ( ts->stop_time == sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_STOP_TIME, i )  && sigmadut_get_traffic_direction(i) == TRAFFIC_SEND )
            {
               printf( "%d ", sigmadut_get_traffic_int( SIGMADUT_TRAFFIC_OUT_OF_SEQ, i) );
            }
        }

        printf( "\n" );
        printf( "> " );
    }

    if ( ts->direction == TRAFFIC_SEND )
    {
        tx_ac_priority_num[ts->ac] = 0;
        cy_rtos_set_mutex(&tx_done_mutex);

        // Free the socket so we can reuse the port
        if ( ts->tx_socket != NULL)
        {
            cywifi_close_socket(ts->tx_socket);
        }
    }
    else
    	rx_ac_priority_num[ts->ac] = 0;

    cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_10MS);
    TRAFFIC_END_OF_THREAD(NULL);
}


int traffic_agent_receive_start ( int argc, char *argv[], tlv_buffer_t** data )
{
	traffic_stream_t *ts = NULL;
	int idx = sigmadut_find_stream_table_entry((int) atoi(argv[2])); // Allocate a stream table entry

	if ( idx == -1 )
	{
	    printf("\nstatus,ERROR\n");
	    return 0;
	}

	ts = sigmadut_get_traffic_stream_instance(idx);
    sigmadut_set_traffic_stream_enable(idx, 1 );

	if ( ts->profile == PROFILE_IPTV )
	{
	    spawn_ts_thread( udp_rx, ts, idx);
	}
	else if ( ts->profile == PROFILE_TRANSACTION )
	{
	   //printf("transactional\n");
	   spawn_ts_thread ( udp_transactional, ts, idx);
	}
	else if ( ts->profile == PROFILE_MULTICAST )
	{
	   //printf("transactional\n");
	   spawn_ts_thread ( udp_rx, ts, idx);
	}

	return 0;
}

int traffic_agent_receive_stop  ( int argc, char *argv[], tlv_buffer_t** data )
{
	int idx = 0;
	int num_rx_streams = 0;
	int idx_table[NUM_STREAM_TABLE_ENTRIES]; // Need an index table to record the order in which to report results
	char* p = NULL;

	//printf("highest rx tos %u\n", (unsigned int)sdpcm_highest_rx_tos);

	 printf( "\nstatus,COMPLETE,streamID,%s,txFrames,", argv[2] ); // Print the first part of the return message

	 // Select which stream results are to be printed and which streams are to be stopped
	 p = strtok( argv[2], "' '" ); // StreamIDs are delimited by spaces, not commas
	 while ( p && ( num_rx_streams < NUM_STREAM_TABLE_ENTRIES ) )
	 {
	      idx = sigmadut_find_stream_table_entry( (int) atoi( p ) ); // Find stream table entry
	      if ( idx == -1 ) // Error check just for the first while loop
	      {
	            printf("status,ERROR\n");
	            return 0;
	      }
	      idx_table[num_rx_streams] = idx;
	      //stream_table[idx].stop_flag = 1;
	      p = strtok( NULL, " "); // Get the next StreamID
	      num_rx_streams++;
	  }/* end of while */

	 int i = 0;
	 while ( i < num_rx_streams ) // Disable the streams
	 {
	    sigmadut_set_traffic_stream_enable(idx_table[i], 0);
	    i++;
	 }

	 cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_100MS);

	 // Print the txFrames value for each stream
	 i = 0;
	 while ( i <  num_rx_streams )
	 {
	    printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_FRAME_SENT, idx_table[i]) );
	    i++;
	 }

	 // Print the rxFrames value for each stream
	 printf( ",rxFrames," );
	 i = 0;
	 while ( i < num_rx_streams )
	 {
	    printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_FRAME_RECVD, idx_table[i]) );
	    i++;
	 }

	 // Print the txPayloadBytes value for each stream
	 printf( ",txPayloadBytes," );
	 i = 0;
	 while ( i < num_rx_streams )
	 {
	     printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_BYTES_SENT, idx_table[i]) );
	     i++;
	 }

	 // Print the rxPayloadBytes value for each stream
	 printf( ",rxPayloadBytes," );
	 i = 0;
	 while ( i < num_rx_streams )
	 {
	     printf( "%d ", sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_BYTES_RECVD, idx_table[i]) );
	     i++;
	 }

	 // Print the outOfSequenceFrames value for each stream
	 printf( ",outOfSequenceFrames," );
	 i = 0;
	 while ( i < num_rx_streams )
	 {
	     printf( "%d ",  sigmadut_get_traffic_int(SIGMADUT_TRAFFIC_OUT_OF_SEQ, idx_table[i]));
	     i++;
	 }
	 printf( "\n" );

	 return 0;
}

int sta_set_11n  ( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1;

    if ( ( strcasecmp(argv[i+2], "reset_default" ) == 0 ) && ( strcasecmp(argv[i+3], "11n") == 0 ) )
    {
	if ( is_wpa_ent_security == true)
        {
#ifndef H1CP_SUPPORT
            sigmadut_get_enterprise_security_handle(&sigmadut_enterprise_sec_handle);
            if ( sigmadut_enterprise_sec_handle != NULL )
            {
                cy_enterprise_security_leave(sigmadut_enterprise_sec_handle);
                cy_enterprise_security_delete(&sigmadut_enterprise_sec_handle);
                sigmadut_set_enterprise_security_handle(NULL);
            }
#endif
        }
        else
        {
	    cywifi_disconnect();
        }
	is_wpa_ent_security = false;
    }
    printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_disconnect  ( int argc, char *argv[], tlv_buffer_t** data )
{
    if ( is_wpa_ent_security == true)
    {
#ifndef H1CP_SUPPORT
       sigmadut_get_enterprise_security_handle(&sigmadut_enterprise_sec_handle);
       if ( sigmadut_enterprise_sec_handle != NULL )
       {
           cy_enterprise_security_leave(sigmadut_enterprise_sec_handle);
           cy_enterprise_security_delete(&sigmadut_enterprise_sec_handle);
           sigmadut_set_enterprise_security_handle(NULL);
       }
#endif
    }
    else
    {
	    cywifi_disconnect();
    }
	is_wpa_ent_security = false;
	printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_scan  ( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1, res = 0;
    char buf[2048];
    memset(buf, 0, 2048);
    while (i <= (argc - 1))
    {
        if (strcasecmp(argv[i], "GetParameter") == 0 && strcasecmp(argv[i + 1], "SSID_BSSID") == 0)
        {
            /* WAR for jira:SWWLAN-150441 (Cert test case HE-5.75.1_6G) scan was failing as fw was
             * sending WLC_E_STATUS_NEWASSOC event becuase last connected AP got switched off
             * without sending disassoc.
             */
            cywifi_disconnect();
            cy_rtos_delay_milliseconds(3000);
            res = cywifi_set_iovar_value(IOVAR_STR_ENABLE_GIANTRX, 1);
            if (res)
            {
                printf("giatrx failed\n");
            }
            cywifi_scan(buf, sizeof(buf));
            printf("\nstatus,COMPLETE%s\n", buf);
            return 0;
        }
        i = i + 2;
    } /* end of while */
    cywifi_scan(NULL, 0);
    printf("\nstatus,COMPLETE\n");
    return 0;
}

int sta_send_addba ( int argc, char *argv[], tlv_buffer_t** data )
{
	int ret, i = 1;
	uint8_t dst_bssid[6] = {0};
	struct ampdu_ea_tid addba_info;
	uint8_t buffer[256] = {0};
	int len;

	memset(&addba_info, 0, sizeof(struct ampdu_ea_tid));
	ret = cywifi_get_bssid ((uint8_t *)dst_bssid  );

	while (i <= (argc - 1))
	{
		if ( strcasecmp( argv[i], "tid" ) == 0 )
		{
			addba_info.tid = atoi(argv[i+1]);
			if ( addba_info.tid > MAX_TID_PRIORITY )
			{
				printf("Wrong Tid > %d\n", MAX_TID_PRIORITY);
			}
		}
		if ( strcasecmp( argv[i], "Dest_mac" ) == 0 )
		{
			sscanf(argv[i+1], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dst_bssid[0], &dst_bssid[1], &dst_bssid[2], &dst_bssid[3], &dst_bssid[4], &dst_bssid[5]);
		}
		i = i + 2;
	} /* end of while */

	memcpy(addba_info.ea.octet, dst_bssid, sizeof(addba_info.ea.octet));

	strcpy((char *)buffer, IOVAR_STR_SEND_ADDBA);

	/* include the null */
	len = strlen(IOVAR_STR_SEND_ADDBA) + 1;

	memcpy(&buffer[len], &addba_info, sizeof(addba_info));

	len += sizeof(addba_info);

	ret = cywifi_set_ioctl_buffer(WLC_SET_VAR, buffer, len);

	if ( ret != 0 )
	{
		printf("\nwifi_set_ioctl_buffer failed %d\n", ret);
	}

	printf("status,COMPLETE\n");
	return ret;
}

int sta_reassoc ( int argc, char *argv[], tlv_buffer_t** data )
{
	int auth_type;
	int ret, i = 1;
    char *chan_str = NULL;
#if 0
    /* Not clear why call cywifi_set_network in sta_reassoc, first remove it for avoiding unexpected error.
     * Remove ip_addr, gw, nmask that only required by cywifi_set_network which is removed as the above reason.
     */
    char *ip_addr = NULL;
    char *gw = NULL;
    char *nmask = NULL;
#endif
    char *ssid = NULL;
    char *encptype = NULL;
    char *keymgmt_type = NULL;
    char *sec_type = NULL;
    char *passphrase = NULL;
    char *pmf = NULL;

    while (i <= (argc - 1))
    {
    	if ( strcasecmp( argv[i], "BSSID" ) == 0 )
	    {
	         sigmadut_set_string(SIGMADUT_BSSID, argv[i+1]);
	    }
	    if ( strcasecmp( argv[i], "channel" ) == 0 )
	    {
	    	sigmadut_set_string(SIGMADUT_CHANNEL, argv[i+1]);
	    }

	    i = i + 2;
	}/* end of while */

    chan_str = sigmadut_get_string(SIGMADUT_CHANNEL);
#if 0
    /* Not clear why call cywifi_set_network in sta_reassoc, first remove it for avoiding unexpected error.
     * Remove ip_addr, gw, nmask that only required by cywifi_set_network which is removed as the above reason.
     */
    ip_addr = sigmadut_get_string(SIGMADUT_IPADDRESS);
    gw = sigmadut_get_string(SIGMADUT_GATEWAY);
    nmask = sigmadut_get_string(SIGMADUT_NETMASK);
#endif

    ssid = sigmadut_get_string(SIGMADUT_SSID);
    keymgmt_type = sigmadut_get_string(SIGMADUT_KEYMGMT_TYPE);
    sec_type = sigmadut_get_string(SIGMADUT_SECURITY_TYPE);
    encptype = sigmadut_get_string(SIGMADUT_ENCRYPTION_TYPE);
    passphrase = sigmadut_get_string(SIGMADUT_PASSPHRASE);
    pmf = sigmadut_get_string(SIGMADUT_PMF);

    cywifi_set_channel((int)atoi(chan_str));
#if 0
    /* Not clear why call cywifi_set_network here, first remove it for avoiding unexpected error.
     */
    cywifi_set_network(ip_addr, nmask, gw);
#endif
#ifdef QUICK_TRACK_SUPPORT
    auth_type = cywifi_get_qt_authtype( encptype, keymgmt_type, sec_type, pmf );
#else
    auth_type = cywifi_get_authtype( encptype, keymgmt_type, sec_type, pmf );
#endif

    ret = cywifi_connect_ap((const char *)ssid, (const char *)passphrase, auth_type);
	if (ret != 0)
	{
	    // printf("\nConnection error: %d\n", ret);
             /* return 0 as WFA cert have negative tests which will not have
              * AP with SSID and returning -1 causes command console to not repsond to
              * next commands
              */
#ifdef QUICK_TRACK_SUPPORT
		printf( "\nstatus,COMPLETE,reassoc,NG,END\n");
#else
        printf("\nstatus,COMPLETE\n");
#endif
		return 0;
	}

#ifdef QUICK_TRACK_SUPPORT
    printf( "\nstatus,COMPLETE,reassoc,OK,END\n");
#else
    printf("\nstatus,COMPLETE\n");
#endif
	return 0;
}

int sta_reset_default ( int argc, char *argv[], tlv_buffer_t** data )
{
	int i = 1;
	uint32_t j = 0;

	while ( i <= (argc - 1 ))
	{
		if (strcasecmp ( argv[i], "prog") == 0 )
	    {
	       i++;
	       for (j = 0; j < sizeof (dot_prog_table)/sizeof(dot_prog_table[j]) && (dot_prog_table[j].prog != NULL); j++ )
	       {
	            if ( (strcasecmp ( argv[i], dot_prog_table[j].prog) == 0 ) )
	            {
	               /* reset program parameters */
	                dot_prog_table[j].resetcb();
	            }
	       }
	    }
	    i = i + 1;
	}
	is_wpa_ent_security = false;
	printf("\nstatus,COMPLETE\n");
	return 0;
}

int sta_set_wireless  ( int argc, char *argv[], tlv_buffer_t** data )
{
	int i = 1;
	uint32_t j = 0;

	while ( i <= (argc - 1 ))
	{
		if (strcmp ( argv[i], "program") == 0 )
	    {
	        i++;
	        for (j = 0; j < sizeof (dot_prog_table)/sizeof(dot_prog_table[j]) && (dot_prog_table[j].prog != NULL); j++ )
	        {
	            if ( (strcasecmp ( argv[i], dot_prog_table[j].prog) == 0 ) )
	            {
	                /* set program parameters */
	               i++;
	               dot_prog_table[j].setcb(argc - i, &argv[i]);
	            }
	       } /*  for */
	    } /* if */
	    i = i + 1;
	} /* while */
	printf("status,COMPLETE\n");
	return 0;
}

int dev_send_frame( int argc, char *argv[], tlv_buffer_t** data )
{
    int i = 1;

    while ( i < argc )
    {
        if ( (strcasecmp(argv[i], "FrameName") == 0) && ((i+1) < argc) )
        {
            i++;
            /* For MBO */
            if ( (strcasecmp(argv[i], "WNM_Notify") == 0) && ((i+2) < argc) )
            {
                i++;
                if ( strcasecmp(argv[i], "WNM_Notify_Element") == 0 )
                {
                    i++;
                    if (strcasecmp ( argv[i], "NonPrefChanReport") == 0 )
                    {
                        i++;
                        cywifi_mbo_send_notif(SIGMA_MBO_ATTR_NON_PREF_CHAN_REPORT);
                    }
                    else if (strcasecmp ( argv[i], "CellularCapabilities") == 0 )
                    {
                        i++;
                        cywifi_mbo_send_notif(SIGMA_MBO_ATTR_CELL_DATA_CAP);
                    }
                }
            }
        }
        else
        {
            i++;
        } 
    }
    printf("status,COMPLETE\n");
    return 0;
}

int sta_client_cert  ( int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    if ( argc == 1)
    {
        switch(enterprise_security_client_cert_type)
        {
            case WIFI_CLIENT_CERTIFICATE_CRED_HOSTAPD:
                printf("Client Certificate Type: HOSTAPD \n");
                break;
            case WIFI_CLIENT_CERTIFICATE_CRED_MSFT:
                printf("Client Certificate Type: MSFT \n");
                break;
            case WIFI_CLIENT_CERTIFICATE_CRED_PMF:
                printf("Client Certificate Type: PMF \n");
                break;
            default:
                printf("Client Certificate Type: None \n");
                break;
        }
    }
    else
    {
        printf("Bad argument\n");
    }
    return result;
}

int whdlog_read  ( int argc, char *argv[], tlv_buffer_t** data )
{
    cy_rslt_t result;
    char buffer[1024] = {0};
    char *bufptr = buffer;
    int buffer_size = (int)sizeof(buffer);

    result = cywifi_get_wifilog((uint8_t *)bufptr, buffer_size );
    printf("\n %s", bufptr );
    return result;
}

int print_whd_stats ( int argc, char *argv[], tlv_buffer_t** data )
{
    cy_rslt_t result;
    result = cywifi_print_whd_stats();
    return result;
}
int wicedlog_read ( int argc, char *argv[], tlv_buffer_t** data )
{
    return 0;
}

int sta_get_systime ( int argc, char *argv[], tlv_buffer_t** data )
{
    cy_rslt_t result;
    result = cywifi_print_system_time();
    if (result != CY_RSLT_SUCCESS)
    {
        printf("cywifi_print_system_time failed %u\n", (unsigned int)result);
    }
    return 0;
}

int reboot_sigma  ( int argc, char *argv[], tlv_buffer_t** data )
{
	cy_rtos_delay_milliseconds( SIGMA_AGENT_DELAY_1S);
    cywifi_system_reset();
    return 0;
}

/* Set VHT parameters and call the functions in the vht_param table matching the string */
int set_vht_params ( int argc, char* params[] )
{
   uint32_t i;
   int j;

   j = 0;

   for (i = 0; i < sizeof (vht_param_table)/sizeof(vht_param_table[i]) && (vht_param_table[i].param != NULL); i++ )
   {
        if ( (strcasecmp ( params[j], vht_param_table[i].param) == 0 ) )
        {
               j++;
               if ( strcasecmp ( params[j], "Disable") == 0 )
               {
                   if ( vht_param_table[i].disable_cb != NULL )
                   {
                      /* call the parameter disable function */
                      vht_param_table[i].disable_cb();
                   }
               }
               else
               {
                  /* call the parameter enable or set function */
                  vht_param_table[i].setparam_cb( &params[j] );
               }
        }
   }

    return 0;
}

int set_pmf_params ( int argc, char* params[] )
{
    return 0;
}

int set_nan_params ( int argc, char* params[] )
{
    return 0;
}

int set_p2p_params ( int argc, char* params[] )
{
    return 0;
}

int set_wfd_params ( int argc, char* params[] )
{
    return 0;
}

int set_wpa3_params ( int argc, char* params[] )
{
    return 0;
}

int set_he_params( int argc, char* params[] )
{
    int i = 0;
    uint32_t j = 0;

    while (i <= (argc - 1))
    {
        for (j = 0; j < sizeof (he_param_table)/sizeof(he_param_table[j]) && (he_param_table[j].param != NULL); j++ )
        {
            if ( (strcasecmp ( params[i], he_param_table[j].param) == 0 ) )
            {
                i++;
                if ( strcasecmp ( params[i], "Disable") == 0 )
                {
                    if ( he_param_table[j].disable_cb != NULL )
                    {
                        /* call the parameter disable function */
                        he_param_table[j].disable_cb();
                    }
                }
                else
                {
                    if ( he_param_table[j].setparam_cb != NULL )
                    {
                        /* call the parameter enable or set function */
                        he_param_table[j].setparam_cb( &params[i] );
                    }
                }
            }
        }
        i++;
    } /* while */

    return 0;
}

int set_mbo_params ( int argc, char* params[] )
{
    return 0;
}

cy_rslt_t start_ap_sigma(const char *ssid, const char *key, uint8_t channel, cy_wcm_security_t security_type, cy_wcm_custom_ie_info_t *custom_ie, uint8_t band)
{
    cy_rslt_t result;
    cy_wcm_ap_config_t ap_params;
    cy_wcm_ip_address_t ipv4_addr;

    memset(&ap_params, 0, sizeof(cy_wcm_ap_config_t));
    memcpy(&ap_params.ap_credentials.SSID, ssid, strlen(ssid) + 1);
    memcpy(&ap_params.ap_credentials.password, key, strlen(key) + 1);
    ap_params.ap_credentials.security = security_type;
    ap_params.channel = channel;
    ap_params.band = band;
    ap_params.ip_settings.ip_address = ap_ip_settings.ip_address;
    ap_params.ip_settings.gateway = ap_ip_settings.gateway;
    ap_params.ip_settings.netmask = ap_ip_settings.netmask;
    if (custom_ie)
    {
        ap_params.ie_info = custom_ie;
    }

    result = cy_wcm_start_ap(&ap_params);

    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    result = cy_wcm_get_ip_addr(CY_WCM_INTERFACE_TYPE_AP, &ipv4_addr);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }
    return CY_RSLT_SUCCESS;
}

int set_oce_params ( int argc, char* argv[] )
{
    char *ssid;
    char *security_key;
    cy_wcm_security_t auth_type;
    uint8_t channel = 6;
    uint8_t band = 2;
    char *encptype = "none", *keymgmt_type = "none", *sec_type = "none", *pmf = "none";
    int i = 0, res = 0;

    while (i < (argc - 1))
    {
        if ( strcasecmp( argv[i], "DevRole" ) == 0 )
        {
            if (strcasecmp( argv[i+1], "STA-CFON" ) == 0)
            {
                oce_stacfon = 1;
            } 
        }
        else if ( strcasecmp( argv[i], "SSID" ) == 0 )
        {
            sigmadut_set_string(SIGMADUT_SSID, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "Channel" ) == 0 )
        {
            sigmadut_set_string(SIGMADUT_CHANNEL, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "KeyMgmtType" ) == 0 )
        {
            sigmadut_set_string(SIGMADUT_KEYMGMT_TYPE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "EncpType" ) == 0 )
        {
            sigmadut_set_string(SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "passphrase" ) == 0 )
        {
            sigmadut_set_string(SIGMADUT_PASSPHRASE, argv[i+1]);
        }
        else if ( strcasecmp( argv[i], "Security" ) == 0 )
       {
            sigmadut_set_string(SIGMADUT_SECURITY_TYPE, argv[i+1]);
       }
       else if ( strcasecmp( argv[i], "pmf" ) == 0 )
       {
            sigmadut_set_string(SIGMADUT_PMF, argv[i+1]);
       }
	   i = i + 2;
    }

    if (oce_stacfon == 1)
    {
        ssid = sigmadut_get_string(SIGMADUT_SSID);
        keymgmt_type = sigmadut_get_string(SIGMADUT_KEYMGMT_TYPE);
        encptype = sigmadut_get_string(SIGMADUT_ENCRYPTION_TYPE);
        security_key = sigmadut_get_string(SIGMADUT_PASSPHRASE);
        sec_type = sigmadut_get_string(SIGMADUT_SECURITY_TYPE);
        pmf = sigmadut_get_string(SIGMADUT_PMF);
        channel = atoi(sigmadut_get_string(SIGMADUT_CHANNEL));

        if (channel <= 14 && channel >= 1)
        {
            cywifi_set_iovar_value (IOVAR_STR_FORCE_PRB_RESPEC, FORCE_PRB_RESPEC_TO_5_5_MBPS);
        }
	else
	{
	    cywifi_set_iovar_value (IOVAR_STR_FORCE_PRB_RESPEC, 0);
	}

        if (channel == 36)
            band = 1;

        auth_type = cywifi_get_authtype( encptype, keymgmt_type, sec_type, pmf );
        res = start_ap_sigma(ssid, security_key, channel, auth_type, NULL, band);
        if(res != CY_RSLT_SUCCESS)
        {
            printf("Failed to start_ap_sigma. Res:0x%x\n", (unsigned int)res);
            return -1;
        }
        is_ap_up = 1;
    }
    return res;
}

int reset_vht_params ( void )
{
    /* enable VHT rates */
	cywifi_set_down();
    cywifi_set_iovar_value ( IOVAR_STR_VHT_FEATURES, VHT_FEATURES_PROPRATES_ENAB );
    cywifi_set_up();

    return 0;
}

int reset_pmf_params (void )
{
    uint32_t mfp;
    char *pmf_str = NULL;

    /* always disconnect to make WCM/WHD/FW in clean state */
    cywifi_disconnect();

    /* Disable Wi-Fi power save mode as this required to pass PMF tests
     * 5.4.3.1 and 5.4.3.2
     */
    cywifi_disable_wifi_powersave();

    pmf_str = sigmadut_get_string(SIGMADUT_PMF);
    get_mfptype( pmf_str, &mfp );

    cywifi_set_iovar_value (IOVAR_STR_MFP, mfp );

    enterprise_security_client_cert_type = WIFI_CLIENT_CERTIFICATE_CRED_PMF;
    return 0;
}

int reset_nan_params (void )
{
    return 0;
}

int reset_p2p_params (void )
{
   return 0;
}

int reset_wfd_params (void )
{
   return 0;
}

int reset_wpa3_params (void )
{
   /* Don't directly call sta_disconnect for avoiding the redundant response "status,COMPLETE" [DRIVERS-11874] */
   //sta_disconnect(0, NULL, NULL);
   if ( is_wpa_ent_security == true)
   {
#ifndef H1CP_SUPPORT
	  sigmadut_get_enterprise_security_handle(&sigmadut_enterprise_sec_handle);
	  if ( sigmadut_enterprise_sec_handle != NULL )
	  {
		  cy_enterprise_security_leave(sigmadut_enterprise_sec_handle);
		  cy_enterprise_security_delete(&sigmadut_enterprise_sec_handle);
		  sigmadut_set_enterprise_security_handle(NULL);
	  }
#endif
   }
   else
   {
	   cywifi_disconnect();
   }
   is_wpa_ent_security = false;
   return 0;
}

int reset_he_params(void )
{
    static int set_once = 0;
    /* always disconnect to make WCM/WHD/FW in clean state */
    cywifi_disconnect();

    /* disable power-saving */
    cywifi_disable_wifi_powersave();

    if (!set_once)
    {
        cywifi_set_down();
        cywifi_set_ioctl_value( WLC_SET_SPECT_MANAGMENT, 0 );
        cywifi_set_up();
        set_once=1;
    }

    he_enabled = 1;
    /* enable MUEDCA_CERT_WAR in FW */
    cywifi_he_muedca_war_enable();

    /* set WL_RSPEC_HE_2x_LTF_GI_0_8us */
    cywifi_set_iovar_value ( IOVAR_STR_HE_LTF_GI_SEL, WL_RSPEC_HE_2x_LTF_GI_0_8us );

    /* disable "AMSDU"/"ADSDU in AMPDU" */
    cywifi_set_iovar_value ( IOVAR_STR_AMSDU, 0 );
    cywifi_set_iovar_value ( IOVAR_STR_AMSDU_IN_AMPDU, 0 );

    cywifi_set_randmac( WL_RANDMAC_SUBCMD_DISABLE );
    cywifi_set_ioctl_value( WLC_SET_FAKEFRAG, 0 );

    /* enable LDPC capability */
    cywifi_set_iovar_value( IOVAR_STR_LDPC_CAP, 1 );
    //cywifi_set_iovar_value( IOVAR_STR_LDPC_TX, 1 );

    /* enable auto rate */
    cywifi_set_iovar_value( IOVAR_STR_2G_RATE, 0 );
    cywifi_set_iovar_value( IOVAR_STR_5G_RATE, 0 );
    cywifi_set_iovar_value( IOVAR_STR_6G_RATE, 0 );
   cywifi_set_iovar_value( "wnm", DISABLE_WNM_MAXIDLE );

    /* clean all PMKID */
    cywifi_clear_pmkid();

    return 0;
}

int reset_mbo_params(void )
{
    /* always disconnect to make WCM/WHD/FW in clean state */
    cywifi_disconnect();

    return 0;
}

int reset_oce_params(void )
{
    int res = 0;
    /* always disconnect to make WCM/WHD/FW in clean state */
    cywifi_disconnect();

    /* disable power-saving */
    cywifi_disable_wifi_powersave();

    cywifi_set_iovar_value(IOVAR_STR_WNM, 0x505);
    res = cy_wcm_stop_ap();
    oce_stacfon = 0;
    if(res != CY_RSLT_SUCCESS)
    {
        printf("Failed to stop_ap. Res:0x%x", (unsigned int)res);
        return -1;
    }
    /* clean all PMKID */
    cywifi_clear_pmkid();

    return 0;
}

#ifdef QUICK_TRACK_SUPPORT
int reset_qt_params(void )
{
#ifdef H1CP_SUPPORT
    uint32_t data;
    uint32_t buf[2];

    memset(&buf, 0 ,sizeof(buf));

    /* always disconnect to make WCM/WHD/FW in clean state */
    cywifi_disconnect();

    /* De-initialize supplicant variable */
    cywifi_set_iovar_buffer("bsscfg:" IOVAR_STR_SUP_WPA, (uint8_t *)&buf, 8);

    /* disable power-saving */
    cywifi_disable_wifi_powersave();

    /* clear transition disabled AKM */
    /*  WAR from F/W. F/W is using get to clear akm transitions */
    cywifi_get_iovar_value(IOVAR_STR_AKM_TRANS_CLEAR, &data);

    /* clean all PMKID */
    cywifi_clear_pmkid();

    /* clear sae password and disable sae*/
    cywifi_clear_saepassword();
    cywifi_set_iovar_value( "sae", 0);
#else
    cy_rtos_delay_milliseconds( SIGMA_AGENT_DELAY_1S);
    cywifi_system_reset();
#endif /* H1CP_SUPPORT */
    return 0;
}
#endif /* QUICK_TRACK_SUPPORT */

int disable_addba_reject    ( void )
{
   uint32_t disable = false;
   int result = CMD_SUCCESS;

   /* Do not reject any ADD BA request */
   result = cywifi_set_iovar_value( IOVAR_STR_AMPDU_RX, (uint32_t)disable );

   return result;
}

int disable_vht_ampdu       ( void )
{
    return 0;
}


int disable_vht_amsdu       ( void )
{
    return 0;
}


int disable_vht_tkip        ( void )
{
    return 0;
}

int disable_vht_wep         ( void )
{
    return 0;
}

int disable_vht_ldpc        ( void )
{
    int enable = false;
    int result = 0;

    /* Disable receive of LDPC coded packets on RX */
    result = cywifi_set_iovar_value( IOVAR_STR_LDPC_CAP, (uint32_t)enable );

    /* Disable the support of sending LDPC coded packets */
    result = cywifi_set_iovar_value( IOVAR_STR_LDPC_TX, (uint32_t)enable  );

    return result;
}


int disable_vht_txbf        ( void )
{
    return 0;
}

int disable_vht_bw_sgnl     ( void )
{
    return 0;
}


int enable_vht_addba_reject ( char *string [] )
{
    uint32_t enable = false;
    int result = CMD_SUCCESS;

    /* Reject any ADD BA request */
    result = cywifi_set_iovar_value( IOVAR_STR_AMPDU_RX, (uint32_t)enable );

    return result;
}


int enable_vht_ampdu        ( char *string [] )
{
    return 0;
}

/* Enable AMSDU Aggregation on the transmit side */
int enable_vht_amsdu        ( char *string [] )
{
    cywifi_set_iovar_value ( IOVAR_STR_AMSDU, 1 );
    cywifi_set_iovar_value ( IOVAR_STR_AMSDU_IN_AMPDU, 1 );
    return 0;
}

/* Set STBC(Space-Time Block Coding) Receive Streams */
int set_vht_stbc_rx         ( char *string [] )
{
    return 0;
}

/* Set channel Width String (20/40/80/160/Auto) */
int set_vht_channel_width   ( char* string [] )
{
    return 0;
}

/* Set SMPS (Spatial-Multiplex) Power Save mode  (Dynamic/0, Static/1, No Limit/2) */
int set_vht_sm_power_save   ( char* string [] )
{
    return 0;
}

/* Set TX spatial stream Value range (1/2/3) */
int set_vht_txsp_stream     ( char* string [] )
{
    return 0;
}

/* Set RX spatial stream Value range ( 1/2/3) */
int set_vht_rxsp_stream     ( char* string [] )
{
    return 0;
}

/* Set VHT band 2.4Ghz/5Ghz */
int set_vht_band            ( char* string [] )
{
    return 0;
}

/* when set the STA sends the RTS frame with dynamic bandwidth signaling */
int enable_vht_dyn_bw_sgnl  ( char* string [] )
{
    return 0;
}

/* Enable Short guard interval at 80 Mhz */
int set_vht_sgi80           ( char* string [] )
{
    int enable = true;
    int result = CMD_SUCCESS;

    /*
     * 000b: No support
     * 001b: 20MHz only
     * 010b: 40Mhz only
     * 011b: both 20MHz and 40MHz
     * 100b: 80MHz (advertised in VHT IE)
     * 101b: 20/80 MHz
     * 110b: 40/80 MHz
     * 111b: 20/40/80 MHz
     *
     */
    uint8_t value = 4;/* 0x100 binary  80Mhz Enable */

    result = cywifi_set_iovar_value( IOVAR_STR_SGI_RX, (uint32_t)value );

    /* Enable SGI irrespective of whether the peer is advertising or not */
    result = cywifi_set_iovar_value( IOVAR_STR_SGI_TX, (uint32_t)enable );

    return result;
}

/* Enable SU(single-user) TxBF beamformee capability with explicit feedback */
int enable_vht_txbf         ( char* string [] )
{
    return 0;
}

/* Enable LDPC code at the physical layer for both TX and RX side */
int enable_vht_ldpc         ( char* string [] )
{
    uint32_t enable = true;
    int result = CMD_SUCCESS;

    /* Enable receive of LDPC coded packets on RX */
    result = cywifi_set_iovar_value( IOVAR_STR_LDPC_CAP, (uint32_t)enable );

    /* Enable support of sending LDPC coded packets */
    result = cywifi_set_iovar_value( IOVAR_STR_LDPC_TX, (uint32_t)enable );

    return result;
}

/* To set the operating mode notification element for 2 values
 * � NSS (number of spatial streams) and channel width.
 * Example - For setting the operating mode notification element
 * with NSS=1 & BW=20Mhz - Opt_md_notif_ie,1;20
 */
int set_vht_opt_md_notif_ie ( char* string [] )
{
    return 0;
}

/*
 * nss_capabilty;mcs_capability. This parameter gives  a description
 * of the supported spatial streams and MCS rate capabilities of the STA
 * For example � If a STA supports 2SS with MCS 0-9, then nss_mcs_cap,2;0-9
 */
int set_vht_nss_mcs_cap     ( char* string []  )
{
    return 0;
}

/* set the TX Highest Supported Long Gi Data Rate subfield */
int set_vht_tx_lgi_rate     ( char* string [] )
{
    return 0;
}

/* set the CRC field to all 0�s */
int set_vht_zero_crc        ( char* string [] )
{
    return 0;
}

/* Enable TKIP in VHT mode */
int enable_vht_tkip         ( char* string [] )
{
    return 0;
}

/* Enable WEP in VHT mode */
int enable_vht_wep          ( char* string [] )
{
    return 0;
}

/* Enable the ability to send out RTS with bandwidth signaling */
int enable_vht_bw_sgnl      ( char* string [] )
{
    return 0;
}

/* Enable BCC code at the physical layer for both TX and RX side */
int enable_he_bcc(char* string [])
{
    int enable = false;
    int result = 0;

    /* Disable receive of LDPC coded packets on RX */
    result = cywifi_set_iovar_value( IOVAR_STR_LDPC_CAP, (uint32_t)enable );

    /* Disable the support of sending LDPC coded packets */
    //result = cywifi_set_iovar_value( IOVAR_STR_LDPC_TX, (uint32_t)enable  );

    return result;
}

/* HE MCS Fixed Rate */
int set_he_mcs_fixedrate(char *string [])
{
    int result = 0;
    /* Be care of LDPC setting in FW side, If testcase have to enable BCC.
       LDPC depends on bandwidth and MCS rate, so it may be enabled in FW */

    /* We remove below is because in certificate environment,
       we should be able to keep highest MCS based on AP's supported MCS.
       So we may not have to set them. */
#if 0
    uint32_t rspec = (uint32_t)(WL_RSPEC_ENCODE_HE |
        (WL_RSPEC_BW_20MHZ) |
        (WL_RSPEC_HE_NSS_UNSPECIFIED << WL_RSPEC_HE_NSS_SHIFT) |
        (atoi(string[0]) & WL_RSPEC_HE_MCS_MASK));


    result = cywifi_set_iovar_value( IOVAR_STR_2G_RATE, rspec );
    result = cywifi_set_iovar_value( IOVAR_STR_5G_RATE, rspec );
#endif
    return result;
}

/* Set HE tx stream */
int set_he_txsp_stream(char *string [])
{
    int result = 0;
/* it is unstable? if change it frequently.      *
 * Actually we may not need this based on cert,  *
 * because we will follow AP supported MCS rate. *
 * So we leave record here                       */
#if 0
    uint32_t txchain = WL_STF_CONFIG_CHAINS_DISABLED;

    if ( strcasecmp ( string[0], "1SS") == 0 )
    {
        txchain = 2;
    }
    else if ( strcasecmp ( string[0], "2SS") == 0 )
    {
        txchain = 3;
    }

    result = cywifi_set_iovar_value( IOVAR_STR_STF_TXCHAIN, txchain );
#endif

    return result;
}

/* Set HE rx stream */
int set_he_rxsp_stream(char *string [])
{
    int result = 0;
/* it is unstable? if change it frequently.      *
 * Actually we may not need this based on cert,  *
 * because we will follow AP supported MCS rate. *
 * So we leave record here                       */
#if 0
    uint32_t rxchain = WL_STF_CONFIG_CHAINS_DISABLED;

    if ( strcasecmp ( string[0], "1SS") == 0 )
    {
        rxchain = 2;
    }
    else if ( strcasecmp ( string[0], "2SS") == 0 )
    {
        rxchain = 3;
    }

    result = cywifi_set_iovar_value( IOVAR_STR_STF_RXCHAIN, rxchain );
#endif
    return result;
}


bool get_mfptype ( char *mfp_string, uint32_t *mfp )
{
    if ( mfp_string != NULL )
    {
        if ( strcasecmp ( mfp_string, "Optional") == 0 )
        {
            *mfp = WL_MFP_CAPABLE;
        }
        else if ( strcasecmp ( mfp_string, "Required") == 0 )
        {
            *mfp = WL_MFP_REQUIRED;
        }
        else if ( strcasecmp ( mfp_string, "None") == 0 )
        {
            *mfp = WL_MFP_NONE;
        }
#ifdef QUICK_TRACK_SUPPORT
        if ( strcasecmp ( mfp_string, "1") == 0 )
        {
            *mfp = WL_MFP_CAPABLE;
        }
        else if ( strcasecmp ( mfp_string, "2") == 0 )
        {
            *mfp = WL_MFP_REQUIRED;
        }
        else if ( strcasecmp ( mfp_string, "0") == 0 )
        {
            *mfp = WL_MFP_NONE;
        }
#endif
        else
        {
             return false;
        }
    }
    return true;
}

bool set_mfptype ( char *mfp_string, uint32_t mfp )
{
    switch (mfp )
    {
    	case WL_MFP_CAPABLE:
    		strncpy(mfp_string, "Optional", strlen("Optional") + 1 );
    		break;
    	case WL_MFP_REQUIRED:
    		strncpy(mfp_string, "Required", strlen("Required") + 1 );
    		break;
    	case WL_MFP_NONE:
    		strncpy(mfp_string, "None", strlen ("None") + 1);
    		break;
    	default:
    		return false;
    }
    return true;
}

cy_time_t get_time (void)
{
	cy_time_t  timeval = 0;
	cy_rtos_get_time(&timeval);

	/* get time in milliseconds */
    return timeval;
}

cy_rslt_t cywifi_init_sigmadut (void )
{
	cy_rslt_t result;
	wifi_cert_time_t curr_date = {0};
	uint8_t wlan_mac[ETH_ADDR_LEN] = {0};
	char buf[64] = {0};
	int i;

	/* Initialize sigma-dut class/object */
	sigmadut_init();

	result = cysigma_wifi_init();
	if ( result != CY_RSLT_SUCCESS )
	{
	    printf("initialization of WiFi failed result:%u\n", (unsigned int)result);
	}

	result = cywifi_get_macaddr(wlan_mac);
	if ( result != CY_RSLT_SUCCESS )
	{
	    printf(" STA MAC Address failed result:%u\n", (unsigned int)result );
	}

	result = cysigma_socket_init();
	if ( result != CY_RSLT_SUCCESS )
	{
		printf("initialization of socket failed result:%u\n", (unsigned int)result);
	}

	result = cy_rtos_init_mutex(&tx_done_mutex);
	if(CY_RSLT_SUCCESS != result)
	{
	   printf("initialization of tx_done_mutex failed result:%u\n", (unsigned int)result);
	   return result;
	}

	/* Add command console table */
	result = cy_command_console_add_table(wifi_cert_command_table);
	if (result != CY_RSLT_SUCCESS)
	{
	    printf("%s: cy_command_console_init FAILED %u\n", __func__, (unsigned int)result);
	}
	cywifi_get_sw_version(buf, sizeof(buf));
	printf("\nPlatform OS      : %s\n", buf);
    printf("WLAN MAC Address : ");

    for ( i = 0; i < ETH_ADDR_LEN; i++ )
    {
        if( wlan_mac[i] == 0x00 )
        {
            i == ( ETH_ADDR_LEN - 1) ? printf("00"):printf("00:");
        }
        else
        {
            i == ( ETH_ADDR_LEN - 1) ? printf("%02x", wlan_mac[i]):printf("%02x:", wlan_mac[i]);
        }
    }
    cywifi_get_wlan_version(wlan_version);
    printf("\nWLAN Firmware    : %s", wlan_version);
    cywifi_get_wlan_clm_version(wlan_version);
    printf("WLAN CLM         : %s\n", wlan_version);
    cywifi_print_whd_version();
    printf("WiFi Cert Tester : %s\n", SIGMA_DUT_VERSION);
    printf("Build Time       : %s  ", BUILD_TIME_STAMP_VALUE);

    result = cywifi_get_system_date_time(BUILD_TIME_STAMP_VALUE, &curr_date);
    if (result != CY_RSLT_SUCCESS)
    {

       /* return CY_RSLT_SUCCESS as non-zero
        * value fails initialization and new line
        * for formatting log
        */
       printf("\n");
       return CY_RSLT_SUCCESS;
    }

    curr_date.tm_wday = cywifi_get_day_of_week(&curr_date);

    result = cywifi_set_system_time(&curr_date);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("cywifi_set_system_time failed %u\n", (unsigned int)result);
    }

    result = cywifi_print_system_time();
    if (result != CY_RSLT_SUCCESS)
    {
        printf("cywifi_print_system_time failed %u\n", (unsigned int)result);
    }
    result = cywifi_start_pt_scan(NULL, MAX_H2E_AP_RESULTS);
    return CY_RSLT_SUCCESS;
}

#ifdef QUICK_TRACK_SUPPORT
int sta_configure     ( int argc, char *argv[], tlv_buffer_t** data )
{
	 int i = 1;
	 wiced_wep_key_t wep_key;

	 memset(&wep_key, 0, sizeof(wiced_wep_key_t));
	 sigmadut_set_string( SIGMADUT_PASSPHRASE, QT_PASSPHRASE_DEFAULT);
	 sigmadut_set_string( SIGMADUT_SECURITY_TYPE, QT_SECTYPE_DEFAULT);
	 sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, QT_ENCPTYPE_DEFAULT);
	 sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, QT_KEYMGMTTYPE_DEFAULT);
	 while (i <= (argc - 1))
	 {
		if ( strcmp( argv[i], "ssid" ) == 0 )
		{
			sigmadut_set_string( SIGMADUT_SSID, argv[i+1]);
		}
		else if (strcmp( argv[i], "psk" ) == 0 )
		{
			sigmadut_set_string( SIGMADUT_PASSPHRASE, argv[i+1]);
		}
		else if (strcmp( argv[i], "key_mgmt" ) == 0 )
		{
			if (strcasecmp( argv[i+1], "SAE WPA-PSK" ) == 0 || strcasecmp( argv[i+1], "SAE" ) == 0)
			{
				/* enable sae */
				cywifi_set_iovar_value( "sae", 1);
			}
			else if (strcasecmp( argv[i+1], "OWE" ) == 0)
			{
				sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, "CCMP");
				sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, "OWE");
			}
			sigmadut_set_string( SIGMADUT_SECURITY_TYPE, argv[i+1]);
		}
		else if (strcmp( argv[i], "pairwise" ) == 0 )
		{
			sigmadut_set_string( SIGMADUT_ENCRYPTION_TYPE, argv[i+1]);
		}
		else if (strcmp( argv[i], "proto" ) == 0 )
		{
			sigmadut_set_string( SIGMADUT_KEYMGMT_TYPE, argv[i+1]);
		}
		else if (strcasecmp( argv[i], "ieee80211w" ) == 0 )
		{
			sigmadut_set_string(SIGMADUT_PMF, argv[i+1]);
		}
		else if (strcasecmp( argv[i], "wep_key0" ) == 0 )
		{
            /* T.b.d. */
		    //sigmadut_set_wepkey(&wep_key);
		}
		i = i + 2;
	}/* end of while */
	
	printf("\nstatus,COMPLETE\n");
	return CY_RSLT_SUCCESS;
}
#else
int sta_configure     ( int argc, char *argv[], tlv_buffer_t** data )
{    
	return CY_RSLT_SUCCESS;
}
#endif //#ifdef QUICK_TRACK_SUPPORT

