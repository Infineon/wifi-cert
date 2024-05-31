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

#include "wifi_cert_sigma_api.h"
#include "wifi_cert_commands.h"
#include "wifi_cert_sigma.h"
#include <stdlib.h>


sigmadut_t sigmadut_obj;
static const char *wfa_root_ca_cert = WIFI_WFA_ENT_ROOT_CERT_STRING;
static const char *wfa_wifi_client_priv_key = WIFI_WFA_ENT_CLIENT_PRIVATE_KEY;
static const char *wfa_wifi_client_cert = WIFI_WFA_ENT_CLIENT_CERT_STRING;

extern thread_details_t detail[4];

void sigmadut_init ( void )
{
    cy_rslt_t result;
	memset(&sigmadut_obj, 0, sizeof(sigmadut_obj));

	/* set default test config values */
	memcpy(sigmadut_obj._interface, TEST_INTERFACE, strlen(TEST_INTERFACE));
	memcpy(sigmadut_obj._passphrase, TEST_PASSPHRASE_DEFAULT, strlen(TEST_PASSPHRASE_DEFAULT));
	memcpy(sigmadut_obj._sectype, TEST_SECTYPE_DEFAULT, strlen(TEST_SECTYPE_DEFAULT));
	memcpy(sigmadut_obj._keymgmttype, TEST_KEYMGMTTYPE_DEFAULT, strlen(TEST_KEYMGMTTYPE_DEFAULT));
	memcpy(sigmadut_obj._pmf, TEST_PMF_DEFAULT, strlen(TEST_PMF_DEFAULT));
	memcpy(sigmadut_obj._ssid, TEST_SSID_DEFAULT, strlen(TEST_SSID_DEFAULT));
	memcpy(sigmadut_obj._bssid, TEST_BSSID_DEFAULT, strlen(TEST_BSSID_DEFAULT));
	memcpy(sigmadut_obj._channel, TEST_CHANNEL_DEFAULT, strlen(TEST_CHANNEL_DEFAULT));
	memcpy(sigmadut_obj._using_dhcp, TEST_USING_DHCP_DEFAULT, strlen(TEST_USING_DHCP_DEFAULT));
	memcpy(sigmadut_obj.dut_ip_addr, DUT_IP_ADDR_DEFAULT, strlen(DUT_IP_ADDR_DEFAULT));
	memcpy(sigmadut_obj.dut_netmask, DUT_NETMASK_DEFAULT, strlen(DUT_NETMASK_DEFAULT));
	memcpy(sigmadut_obj.dut_gateway, DUT_GATEWAY_DEFAULT, strlen(DUT_GATEWAY_DEFAULT));
	memcpy(sigmadut_obj.dut_primary_dns, DUT_PRIMARY_DNS_DEFAULT, strlen(DUT_PRIMARY_DNS_DEFAULT));
	memcpy(sigmadut_obj.dut_secondary_dns, DUT_SECONDARY_DNS_DEFAULT, strlen(DUT_SECONDARY_DNS_DEFAULT));
	memcpy(sigmadut_obj._pwrsave, TEST_PWRSAVE_DEFAULT, strlen(TEST_PWRSAVE_DEFAULT));

	result = cy_rtos_init_mutex(&(sigmadut_obj.sigmadut_mutex));
	if(CY_RSLT_SUCCESS != result)
	{
	    printf("initialization of tx_done_mutex failed result:%u\n", (unsigned int)result);
	}
}

void sigmadut_set_traffic_class ( int stream_id, qos_access_category_t qos_cat)
{
   sigmadut_obj.stream_table[stream_id].ac = qos_cat;
}

void sigmadut_set_traffic_dst_port ( int stream_id, uint16_t port)
{
   sigmadut_obj.stream_table[stream_id].dest_port = port;
}

void sigmadut_set_traffic_src_port ( int stream_id, uint16_t port)
{
   sigmadut_obj.stream_table[stream_id].src_port = port;
}

void sigmadut_set_traffic_dst_ipaddress(int stream_id, char *ip_addr)
{
   memcpy(sigmadut_obj.stream_table[stream_id].dest_ipaddr, ip_addr, sizeof(sigmadut_obj.stream_table[stream_id].dest_ipaddr) - 1 );
}

void sigmadut_set_traffic_src_ipaddress(int stream_id, char *ip_addr)
{
   memcpy(sigmadut_obj.stream_table[stream_id].src_ipaddr, ip_addr, sizeof(sigmadut_obj.stream_table[stream_id].src_ipaddr) - 1 );
}

void sigmadut_set_traffic_direction (int stream_id, traffic_direction_t dir)
{
   sigmadut_obj.stream_table[stream_id].direction = dir;
}

void sigmadut_set_traffic_profile(int stream_id, traffic_profile_t profile)
{
   sigmadut_obj.stream_table[stream_id].profile = profile;
}

void sigmadut_set_traffic_payloadsize(int stream_id, int payload_size)
{
   sigmadut_obj.stream_table[stream_id].payload_size = payload_size;
}

void sigmadut_set_traffic_duration(int stream_id, int duration)
{
   sigmadut_obj.stream_table[stream_id].duration = duration;
}

void sigmadut_set_traffic_framerate( int stream_id, int frame_rate )
{
   sigmadut_obj.stream_table[stream_id].frame_rate = frame_rate;
}

int sigmadut_set_string ( SIGMADUT_CONFIG_DATA_STR_TYPE_T type, char *str)
{

	if ( str == NULL )
	{
		return -1;
	}
	if ( strlen(str) == 0 )
	{
		return -1;
	}

    switch(type)
    {
        case  SIGMADUT_INTERFACE:
        	strncpy(sigmadut_obj._interface, str, sizeof(sigmadut_obj._interface) - 1);
        	break;

        case SIGMADUT_DHCP:
        	strncpy(sigmadut_obj._using_dhcp, str, sizeof(sigmadut_obj._using_dhcp ) - 1 );
        	break;

        case SIGMADUT_IPADDRESS:
        	strncpy(sigmadut_obj.dut_ip_addr, str, sizeof(sigmadut_obj.dut_ip_addr ) - 1 );
        	break;

        case SIGMADUT_GATEWAY:
        	strncpy(sigmadut_obj.dut_gateway, str, sizeof( sigmadut_obj.dut_gateway ) - 1 );
        	break;

        case SIGMADUT_NETMASK:
        	strncpy(sigmadut_obj.dut_netmask, str, sizeof(sigmadut_obj.dut_netmask ) - 1 );
        	break;

        case SIGMADUT_PRIMARY_DNS:
        	strncpy(sigmadut_obj.dut_primary_dns, str, sizeof(sigmadut_obj.dut_primary_dns) - 1);
        	break;

        case SIGMADUT_SECONDARY_DNS:
        	strncpy(sigmadut_obj.dut_secondary_dns, str, sizeof(sigmadut_obj.dut_secondary_dns) - 1);
        	break;

        case SIGMADUT_SSID:
        	strncpy(sigmadut_obj._ssid, str, (sizeof(sigmadut_obj._ssid) - 1));
        	break;

        case SIGMADUT_SECURITY_TYPE:
        	strncpy(sigmadut_obj._sectype, str, (sizeof(sigmadut_obj._sectype) - 1));
        	break;

        case SIGMADUT_KEYMGMT_TYPE:
        	strncpy(sigmadut_obj._keymgmttype, str, (sizeof(sigmadut_obj._keymgmttype) - 1));
        	break;

        case SIGMADUT_ENCRYPTION_TYPE:
        	strncpy ( sigmadut_obj._encptype, str, (sizeof(sigmadut_obj._encptype) -1 ));
        	break;

        case SIGMADUT_PASSPHRASE:
        	strncpy(sigmadut_obj._passphrase, str, (sizeof(sigmadut_obj._passphrase) - 1));
        	break;

        case SIGMADUT_BSSID:
        	strncpy(sigmadut_obj._bssid, str, sizeof(sigmadut_obj._bssid) - 1 );
           	break;

         case SIGMADUT_CHANNEL:
        	 strncpy(sigmadut_obj._channel, str, sizeof(sigmadut_obj._channel ) - 1 );
             break;

         case SIGMADUT_USERNAME:
             strncpy(sigmadut_obj.username, str, sizeof(sigmadut_obj.username ) - 1 );
             break;

         case SIGMADUT_PASSWORD:
             strncpy(sigmadut_obj.password, str, sizeof(sigmadut_obj.password ) - 1 );
             break;

         case SIGMADUT_TRUSTEDROOTCA:
             strncpy(sigmadut_obj.trustedrootcrt, str, sizeof(sigmadut_obj.trustedrootcrt ) - 1 );
             break;

         case SIGMADUT_CLIENTCERT:
             strncpy(sigmadut_obj.clientcert, str, sizeof(sigmadut_obj.clientcert ) - 1 );
             break;

         case SIGMADUT_INNEREAP:
             strncpy(sigmadut_obj.innereap, str, sizeof(sigmadut_obj.innereap) - 1);
             break;

         case SIGMADUT_PEAPVERSION:
             strncpy(sigmadut_obj.peapver, str, sizeof(sigmadut_obj.peapver) - 1);
             break;

         case SIGMADUT_PWRSAVE:
             strncpy(sigmadut_obj._pwrsave, str, sizeof(sigmadut_obj._pwrsave) - 1);
             break;

         case SIGMADUT_PMF:
             strncpy(sigmadut_obj._pmf, str, sizeof(sigmadut_obj._pmf) - 1);
             break;

        default:
        	break;
    }
    return 0;
}

void sigmadut_init_stream_table (void )
{
	int i;

	for( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
	{
		if( sigmadut_obj.stream_table[i].allocated == 1 )
		{
			(void)cy_rtos_join_thread((cy_thread_t *)&detail[i].thread_handle);
		}
	}
	memset(&detail, 0x00, sizeof(detail));
	memset( sigmadut_obj.stream_table, 0x00, sizeof( sigmadut_obj.stream_table ) );
}

int sigmadut_find_stream_table_entry (int id)
{
	int i;

	for( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
	{
		if( ( sigmadut_obj.stream_table[i].stream_id == (uint32_t) id ) && ( sigmadut_obj.stream_table[i].allocated == 1 ) )
		{
		   return( i );
		}
	}
	return -1;
}

void sigmadut_set_traffic_stream_enable ( int stream_id, int enable )
{
	sigmadut_obj.stream_table[stream_id].enabled = enable;
}

char *sigmadut_get_string ( SIGMADUT_CONFIG_DATA_STR_TYPE_T type)
{
	char *str = NULL;

	 switch(type)
	 {
	        case  SIGMADUT_INTERFACE:
	        	str = sigmadut_obj._interface;
	        	break;

	        case SIGMADUT_DHCP:
	        	str = sigmadut_obj._using_dhcp;
	        	break;

	        case SIGMADUT_IPADDRESS:
	        	str = sigmadut_obj.dut_ip_addr;
	        	break;

	        case SIGMADUT_GATEWAY:
	        	str = sigmadut_obj.dut_gateway;
	        	break;

	        case SIGMADUT_NETMASK:
	        	str = sigmadut_obj.dut_netmask;
	        	break;

	        case SIGMADUT_PRIMARY_DNS:
	        	str = sigmadut_obj.dut_primary_dns;
	        	break;

	        case SIGMADUT_SECONDARY_DNS:
	        	str = sigmadut_obj.dut_secondary_dns;
	        	break;

	        case SIGMADUT_SSID:
	        	str = sigmadut_obj._ssid;
	           	break;

	        case SIGMADUT_SECURITY_TYPE:
	        	str = sigmadut_obj._sectype;
	           	break;

	        case SIGMADUT_KEYMGMT_TYPE:
	        	str = sigmadut_obj._keymgmttype;
	           	break;

	        case SIGMADUT_ENCRYPTION_TYPE:
	        	str = sigmadut_obj._encptype;
	           	break;

	        case SIGMADUT_PASSPHRASE:
	        	str = sigmadut_obj._passphrase;
	           	break;

	        case SIGMADUT_BSSID:
	        	 str = sigmadut_obj._bssid;
	        	break;

	        case SIGMADUT_CHANNEL:
	            str = sigmadut_obj._channel;
	        	break;

	        case SIGMADUT_USERNAME:
	            str = sigmadut_obj.username;
	            break;

	        case SIGMADUT_PASSWORD:
	            str = sigmadut_obj.password;
	            break;

	        case SIGMADUT_CLIENTCERT:
	            str = sigmadut_obj.clientcert;
	            break;

	        case SIGMADUT_TRUSTEDROOTCA:
	            str = sigmadut_obj.trustedrootcrt;
	            break;

	        case SIGMADUT_INNEREAP:
	            str = sigmadut_obj.innereap;
	            break;

	        case SIGMADUT_PEAPVERSION:
	            str = sigmadut_obj.peapver;
	            break;

	        case SIGMADUT_PWRSAVE:
	            str = sigmadut_obj._pwrsave;
	            break;

		case SIGMADUT_PMF:
		    str = sigmadut_obj._pmf;
		    break;

	        default:
	        	break;
	 }
	 return str;
}

int sigmadut_get_traffic_duration ( int stream_id )
{
	return sigmadut_obj.stream_table[stream_id].duration;
}

int sigmadut_find_num_active_streams ( void )
{
	int i, num_active_streams = 0;

	for ( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
	{
			if ( sigmadut_obj.stream_table[i].allocated == 1 )
			{
				num_active_streams++;
			}
	}
	return num_active_streams;
}

traffic_stream_t *sigmadut_get_traffic_stream_instance (int stream_id )
{
   return &sigmadut_obj.stream_table[stream_id];
}

int sigmadut_set_time_date_int ( SIGMADUT_DATE_TIME_INT_TYPE_T type, int value)
{
    switch(type)
    {
        case SIGMADUT_DATE:
             if ( ( value  < 0 ) || ( value > MAX_DAYS) )
             {
                return -1;
             }
             sigmadut_obj.time.tm_mday = value;
             break;

        case SIGMADUT_MONTH:
             if ( ( value  < 0 ) || ( value > MAX_MONTHS) )
             {
                return -1;
             }
             sigmadut_obj.time.tm_month = value;
             break;

        case SIGMADUT_YEAR:
             sigmadut_obj.time.tm_year = value;
             break;

        case SIGMADUT_HOURS:
             if ( ( value  < 0 ) || ( value > MAX_HOURS) )
             {
                return -1;
             }
             sigmadut_obj.time.tm_hour = value;
             break;

        case SIGMADUT_MINUTES:
             if ( ( value  < 0 ) || ( value > MAX_MINUTES) )
             {
                return -1;
             }
             sigmadut_obj.time.tm_min = value;
             break;

        case SIGMADUT_SECONDS:
             if ( ( value  < 0 ) || ( value > MAX_SECONDS) )
             {
                return -1;
             }
             sigmadut_obj.time.tm_sec = value;
             break;
        default:
             break;
    }
    return 0;
}

int sigmadut_get_time_date_int ( SIGMADUT_DATE_TIME_INT_TYPE_T type )
{
   int data = 0;
   switch(type)
   {
       case SIGMADUT_DATE:
           data = sigmadut_obj.time.tm_mday;
           break;

       case SIGMADUT_MONTH:
           data = sigmadut_obj.time.tm_month;
           break;

       case SIGMADUT_YEAR:
           data = sigmadut_obj.time.tm_year;
           break;

       case SIGMADUT_HOURS:
           data = sigmadut_obj.time.tm_hour;
           break;

       case SIGMADUT_MINUTES:
           data = sigmadut_obj.time.tm_min;
           break;

       case SIGMADUT_SECONDS:
           data = sigmadut_obj.time.tm_sec;
           break;

       default:
           break;
   }
   return data;
}

int sigmadut_set_twt_int(SIGMADUT_TWT_INT_TYPE_T type, uint32_t value)
{
    switch(type)
    {
        case SIGMADUT_NEGOTIATIONTYPE:
            sigmadut_obj.twt_param.negotiationtype = (uint8_t)value;
            break;
        case SIGMADUT_TWT_SETUP:
            sigmadut_obj.twt_param.twt_setup = (uint8_t)value;
            break;
         case SIGMADUT_SETUPCOMMAND:
            sigmadut_obj.twt_param.setupcommand = (uint8_t)value;
            break;
        case SIGMADUT_TWT_TRIGGER:
            sigmadut_obj.twt_param.twt_trigger = (uint8_t)value;
            break;
        case SIGMADUT_FLOWTYPE:
            sigmadut_obj.twt_param.flowtype = (uint8_t)value;
            break;
        case SIGMADUT_WAKEINTERVALEXP:
            sigmadut_obj.twt_param.wakeintervalexp = (uint8_t)value;
            break;
        case SIGMADUT_NOMINALMINWAKEDUR:
            sigmadut_obj.twt_param.nominalminwakedur = (uint8_t)value;
            break;
        case SIGMADUT_WAKEINTERVALMANTISSA:
            sigmadut_obj.twt_param.wakeintervalmantissa = (uint16_t)value;
            break;
        case SIGMADUT_FLOWID:
            sigmadut_obj.twt_param.flowid = (uint8_t)value;
            break;
        case SIGMADUT_BTWT_ID:
            sigmadut_obj.twt_param.btwt_id = (uint8_t)value;
            break;
        case SIGMADUT_RESUME_DURATION:
            sigmadut_obj.twt_param.resume_duration = (uint8_t)value;
            break;
        default:
            break;
    }
    return 0;
}

int sigmadut_get_twt_int(SIGMADUT_TWT_INT_TYPE_T type)
{
    int data = 0;
    switch(type)
    {
        case SIGMADUT_NEGOTIATIONTYPE:
            data = sigmadut_obj.twt_param.negotiationtype;
            break;
        case SIGMADUT_TWT_SETUP:
            data = sigmadut_obj.twt_param.twt_setup;
            break;
        case SIGMADUT_SETUPCOMMAND:
            data = sigmadut_obj.twt_param.setupcommand;
            break;
        case SIGMADUT_TWT_TRIGGER:
            data = sigmadut_obj.twt_param.twt_trigger;
            break;
        case SIGMADUT_FLOWTYPE:
            data = sigmadut_obj.twt_param.flowtype;
            break;
        case SIGMADUT_WAKEINTERVALEXP:
            data = sigmadut_obj.twt_param.wakeintervalexp;
            break;
        case SIGMADUT_NOMINALMINWAKEDUR:
            data = sigmadut_obj.twt_param.nominalminwakedur;
            break;
        case SIGMADUT_WAKEINTERVALMANTISSA:
            data = sigmadut_obj.twt_param.wakeintervalmantissa;
            break;
        case SIGMADUT_FLOWID:
            data = sigmadut_obj.twt_param.flowid;
            break;
        case SIGMADUT_BTWT_ID:
            data = sigmadut_obj.twt_param.btwt_id;
            break;
        case SIGMADUT_RESUME_DURATION:
            data = sigmadut_obj.twt_param.resume_duration;
            break;
        default:
            break;
    }
    return data;
}

int sigmadut_set_ltf_gi_str(SIGMADUT_LTF_GI_STR_TYPE_T type, char *str)
{
    if ( str == NULL )
    {
        return -1;
    }
    if ( strlen(str) == 0 )
    {
        return -1;
    }

    switch(type)
    {
        case SIGMADUT_LTF:
            strncpy(sigmadut_obj.ltf_gi.ltf, str, sizeof(sigmadut_obj.ltf_gi.ltf) - 1);
            break;
        case SIGMADUT_GI:
            strncpy(sigmadut_obj.ltf_gi.gi, str, sizeof(sigmadut_obj.ltf_gi.gi) - 1 );
            break;
        default:
            break;
    }
    return 0;
}

char *sigmadut_get_ltf_gi_str(SIGMADUT_LTF_GI_STR_TYPE_T type)
{
    char *str = NULL;
    switch(type)
    {
        case SIGMADUT_LTF:
            str = sigmadut_obj.ltf_gi.ltf;
            break;
        case SIGMADUT_GI:
            str = sigmadut_obj.ltf_gi.gi;
            break;
        default:
            break;
    }
    return str;
}

int sigmadut_set_tx_omi_int(SIGMADUT_TX_OMI_INT_TYPE_T type, uint32_t value)
{
    switch(type)
    {
        case SIGMADUT_TXNSTS:
            sigmadut_obj.tx_omi.txnsts = (uint8_t)value;
            break;
         case SIGMADUT_CHNLWIDTH:
            sigmadut_obj.tx_omi.chnlwidth = (uint8_t)value;
            break;
        case SIGMADUT_ULMUDISABLE:
            sigmadut_obj.tx_omi.ulmudisable = (uint8_t)value;
            break;
        default:
            break;
    }
    return 0;
}

int sigmadut_get_tx_omi_int(SIGMADUT_TX_OMI_INT_TYPE_T type)
{
    int data = 0;
    switch(type)
    {
        case SIGMADUT_TXNSTS:
            data = sigmadut_obj.tx_omi.txnsts;
            break;
        case SIGMADUT_CHNLWIDTH:
            data = sigmadut_obj.tx_omi.chnlwidth;
            break;
        case SIGMADUT_ULMUDISABLE:
            data = sigmadut_obj.tx_omi.ulmudisable;
            break;
        default:
            break;
    }
    return data;
}

int sigmadut_set_mbo_int(SIGMADUT_MBO_INT_TYPE_T type, uint32_t value)
{
    switch(type)
    {
        case SIGMADUT_MBO_OPCLASS:
            sigmadut_obj.mbo_param.opclass = (uint8_t)value;
            break;
        case SIGMADUT_MBO_CHANNEL:
            sigmadut_obj.mbo_param.chan = (uint8_t)value;
            break;
        case SIGMADUT_MBO_PREFERENCE:
            sigmadut_obj.mbo_param.pref = (uint8_t)value;
            break;
        case SIGMADUT_MBO_REASON:
            sigmadut_obj.mbo_param.reason = (uint8_t)value;
            break;
        default:
            break;
    }
    return 0;
}

int sigmadut_get_mbo_int(SIGMADUT_MBO_INT_TYPE_T type)
{
    int data = 0;
    switch(type)
    {
        case SIGMADUT_MBO_OPCLASS:
            data = sigmadut_obj.mbo_param.opclass;
            break;
        case SIGMADUT_MBO_CHANNEL:
            data = sigmadut_obj.mbo_param.chan;
            break;
        case SIGMADUT_MBO_PREFERENCE:
            data = sigmadut_obj.mbo_param.pref;
            break;
        case SIGMADUT_MBO_REASON:
            data = sigmadut_obj.mbo_param.reason;
            break;
        default:
            break;
    }
    return data;
}

int sigmadut_set_traffic_int ( SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type, int stream_id, int data )
{
	if ( ( stream_id  < 0 ) || ( stream_id >= NUM_STREAM_TABLE_ENTRIES) )
	{
       return -1;
	}

	 switch(type)
	 {
	 	 case SIGMADUT_TRAFFIC_PAYLOAD_SIZE:
	 		  sigmadut_obj.stream_table[stream_id].payload_size = data;
	 		  break;

	 	 case SIGMADUT_TRAFFIC_FRAME_RATE:
	 		  sigmadut_obj.stream_table[stream_id].frame_rate = data;
	 		  break;

	 	 case SIGMADUT_TRAFFIC_DURATION:
	 		  sigmadut_obj.stream_table[stream_id].duration = data;
	 		  break;

	 	 case SIGMADUT_TRAFFIC_STOP_TIME:
	 		  sigmadut_obj.stream_table[stream_id].stop_time = data;
	 		  break;

	 	 case SIGMADUT_TRAFFIC_FRAME_SENT:
	 		  sigmadut_obj.stream_table[stream_id].frames_sent = data;
	 		  break;

	 	 default:
	 		  break;
	}
    return 0;
}

int sigmadut_get_traffic_int ( SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type, int stream_id )
{
	int data = 0;

	if ( ( stream_id  < 0 ) || ( stream_id >= NUM_STREAM_TABLE_ENTRIES) )
	{
	    return -1;
	}

	 switch(type)
	 {
	     case SIGMADUT_TRAFFIC_PAYLOAD_SIZE:
	    	 data = sigmadut_obj.stream_table[stream_id].payload_size;
		 	 break;

		 case SIGMADUT_TRAFFIC_FRAME_RATE:
			 data = sigmadut_obj.stream_table[stream_id].frame_rate;
		 	 break;

		 case SIGMADUT_TRAFFIC_DURATION:
			 data = sigmadut_obj.stream_table[stream_id].duration;
			 break;

		 case SIGMADUT_TRAFFIC_STOP_TIME:
			 data = sigmadut_obj.stream_table[stream_id].stop_time;
		 	 break;

		 case SIGMADUT_TRAFFIC_FRAME_SENT:
			 data = sigmadut_obj.stream_table[stream_id].frames_sent;
		 	 break;

		 case SIGMADUT_TRAFFIC_FRAME_RECVD:
			 data = sigmadut_obj.stream_table[stream_id].frames_received;
		 	 break;

		 case SIGMADUT_TRAFFIC_BYTES_SENT:
			 data = sigmadut_obj.stream_table[stream_id].bytes_sent;
		 	 break;

		 case SIGMADUT_TRAFFIC_BYTES_RECVD:
			 data = sigmadut_obj.stream_table[stream_id].bytes_received;
		 	 break;

		 case SIGMADUT_TRAFFIC_OUT_OF_SEQ:
			 data = sigmadut_obj.stream_table[stream_id].out_of_sequence_frames;
		     break;

		 default:
		 	 break;
	 }
	 return data;
}

int sigmadut_traffic_increment_data (SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type, traffic_stream_t *ts, int size )
{
	 if ( ts == NULL )
	 {
		 return -1;
	 }

	 switch(type)
	 {
	 	 case SIGMADUT_TRAFFIC_FRAME_SENT:
	 		 ts->frames_sent += size;
	 		 break;

	 	 case SIGMADUT_TRAFFIC_FRAME_RECVD:
	 		 ts->frames_received += size;
	 		 break;

	 	 case SIGMADUT_TRAFFIC_BYTES_SENT:
	 		 ts->bytes_sent += size;
	 		 break;

	 	 case SIGMADUT_TRAFFIC_BYTES_RECVD:
	 		 ts->bytes_received += size;
	 		 break;

	 	 default:
	 		 break;
    }
    return 0;
}

traffic_direction_t sigmadut_get_traffic_direction (int stream_id)
{
	return sigmadut_obj.stream_table[stream_id].direction;
}

uint32_t sigmadut_get_traffic_streamid ( int stream_id )
{
	return sigmadut_obj.stream_table[stream_id].stream_id;
}

void sigmadut_get_enterprise_security_handle( void **enteprise_handle)
{
    *enteprise_handle = sigmadut_obj.enterprise_sec_handle;
}

void sigmadut_set_enterprise_security_handle ( void *enterprise_handle)
{
    sigmadut_obj.enterprise_sec_handle = enterprise_handle;
}

int sigmadut_find_unallocated_stream_table_entry ( void )
{
	int i;

	for( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
	{
	    if( sigmadut_obj.stream_table[i].allocated == 0 )
	    {
	        memset(&sigmadut_obj.stream_table[i], 0, sizeof(traffic_stream_t));
	        sigmadut_obj.stream_table[i].allocated = 1;
	        return( i );
	    }
	}
	return -1;
}

void sigmadut_set_streamid ( int stream_idx, int stream_id )
{
	sigmadut_obj.stream_table[stream_idx].stream_id = (uint32_t)stream_id;
}

traffic_profile_t sigmadut_get_traffic_profile ( int stream_id)
{
   return sigmadut_obj.stream_table[stream_id].profile;
}

int sigmadut_set_traffic_ip_address(SIGMADUT_TRAFFIC_CONFIG_STR_TYPE_T type, int stream_id, char *ip_addr)
{
	if ( ( stream_id  < 0 ) || ( stream_id >= NUM_STREAM_TABLE_ENTRIES) )
	{
	    return -1;
	}
	switch (type)
	{
		case SIGMADUT_TRAFFIC_SRC_IPADDRESS:
			break;

		case SIGMADUT_TRAFFIC_DST_IPADDRESS:
			break;

		default:
			break;
	}
    return 0;
}

char *sigmadut_get_traffic_ip_address(SIGMADUT_TRAFFIC_CONFIG_STR_TYPE_T type, int stream_id )
{
	char *data = NULL;
	if ( ( stream_id  < 0 ) || ( stream_id >= NUM_STREAM_TABLE_ENTRIES) )
	{
		return data;
	}
	switch (type)
	{
		case SIGMADUT_TRAFFIC_SRC_IPADDRESS:
			data = sigmadut_obj.stream_table[stream_id].src_ipaddr;
			break;

		case SIGMADUT_TRAFFIC_DST_IPADDRESS:
			data = sigmadut_obj.stream_table[stream_id].dest_ipaddr;
			break;

		default:
			break;
	}
	return data;
}

int sigmadut_set_traffic_port ( SIGMADUT_TRAFFIC_DATA_UINT16_TYPE_T type, int stream_id, uint16_t port)
{
	if ( ( stream_id  < 0 ) || ( stream_id >= NUM_STREAM_TABLE_ENTRIES) )
	{
	   return -1;
	}
	switch (type)
	{
		case SIGMADUT_TRAFFIC_SRC_PORT:
			sigmadut_obj.stream_table[stream_id].src_port = port;
			break;

		case SIGMADUT_TRAFFIC_DST_PORT:
			sigmadut_obj.stream_table[stream_id].dest_port = port;
			break;

		default:
			break;
   }
   return 0;
}

uint16_t sigmadut_get_traffic_port ( SIGMADUT_TRAFFIC_DATA_UINT16_TYPE_T type, int stream_id )
{
	uint16_t port = 0;

	if ( ( stream_id  < 0 ) || ( stream_id >= NUM_STREAM_TABLE_ENTRIES) )
	{
	   return port;
	}
	switch (type)
	{
		case SIGMADUT_TRAFFIC_SRC_PORT:
			port = sigmadut_obj.stream_table[stream_id].src_port;
			break;

		case SIGMADUT_TRAFFIC_DST_PORT:
			port = sigmadut_obj.stream_table[stream_id].dest_port;
			break;

		default:
			break;
	}
	return port;
}

qos_access_category_t sigmadut_get_traffic_class ( int stream_id)
{
   return sigmadut_obj.stream_table[stream_id].ac;
}


void sigmadut_set_wepkey ( wiced_wep_key_t *key )
{
	memcpy(&sigmadut_obj.wepkey, key , sizeof(sigmadut_obj.wepkey));
}

wiced_wep_key_t *sigmadut_get_wepkey(void)
{
   return &sigmadut_obj.wepkey;
}

void sigmadut_set_wepkey_buffer ( uint8_t *wepkey_buffer )
{
	memcpy(sigmadut_obj._wepkey_buffer, wepkey_buffer, sizeof(sigmadut_obj._wepkey_buffer));
}

uint8_t *sigmadut_get_wepkey_buffer (void )
{
   	return sigmadut_obj._wepkey_buffer;
}

cy_mutex_t *sigmadut_get_mutex_instance (void )
{
    return &sigmadut_obj.sigmadut_mutex;
}

cy_rslt_t cywifi_set_enterprise_security_cert ( WIFI_CLIENT_CERT_TYPE_T enterprise_security_cert_type )
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    switch (enterprise_security_cert_type)
    {
        case WIFI_CLIENT_CERTIFICATE_CRED_HOSTAPD:
            wfa_root_ca_cert = WIFI_WFA_ENT_ROOT_CERT_STRING;
            wfa_wifi_client_priv_key = WIFI_WFA_ENT_CLIENT_PRIVATE_KEY;
            wfa_wifi_client_cert = WIFI_WFA_ENT_CLIENT_CERT_STRING;
            break;

        case WIFI_CLIENT_CERTIFICATE_CRED_MSFT:
            wfa_root_ca_cert = WIFI_WFA_ENT_ROOT_CERT_STRING_MSFT;
            wfa_wifi_client_priv_key = WIFI_WFA_ENT_CLIENT_PRIVATE_KEY_MSFT;
            wfa_wifi_client_cert = WIFI_WFA_ENT_CLIENT_CERT_STRING_MSFT;
            break;

        case WIFI_CLIENT_CERTIFICATE_CRED_PMF:
            wfa_root_ca_cert = WIFI_WFA_ENT_ROOT_CERT_STRING_PMF;
            wfa_wifi_client_priv_key = WIFI_WFA_ENT_CLIENT_PRIVATE_KEY_PMF;
            wfa_wifi_client_cert = WIFI_WFA_ENT_CLIENT_CERT_STRING_PMF;
            break;

        default:
            break;
    }
    return result;
}

#ifndef H1CP_SUPPORT
cy_rslt_t cywifi_update_enterpise_security_params( cy_enterprise_security_parameters_t *ent_params, void *handle)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    cy_enterprise_security_eap_type_t  eap_type;
    cy_enterprise_security_auth_t auth_type;
    char *username = NULL;
    char *password = NULL;
    char *ssid = NULL;

    eap_type = (cy_enterprise_security_eap_type_t )sigmadut_get_eap_type();
    auth_type = CY_ENTERPRISE_SECURITY_AUTH_TYPE_WPA2_AES; //(cy_enterprise_security_auth_t)cywifi_get_authtype( encptype, keymgmt_type, sec_type );

    if( ( eap_type == CY_ENTERPRISE_SECURITY_EAP_TYPE_PEAP ) ||
        ( eap_type == CY_ENTERPRISE_SECURITY_EAP_TYPE_TTLS ) ||
        ( eap_type == CY_ENTERPRISE_SECURITY_EAP_TYPE_TLS ))
    {
        username = sigmadut_get_string(SIGMADUT_USERNAME);

        if (username != NULL )
        {
            /* Copy phase2 identity */
            strncpy( ent_params->phase2.inner_identity, username, CY_ENTERPRISE_SECURITY_MAX_IDENTITY_LENGTH );
                     ent_params->phase2.inner_identity[ CY_ENTERPRISE_SECURITY_MAX_IDENTITY_LENGTH - 1 ] = '\0';
        }

        if( eap_type == CY_ENTERPRISE_SECURITY_EAP_TYPE_TLS )
        {
            ent_params->is_client_cert_required = 1;
        }
        else if( eap_type == CY_ENTERPRISE_SECURITY_EAP_TYPE_PEAP )
        {
             password = sigmadut_get_string(SIGMADUT_PASSWORD);
             ent_params->phase2.inner_eap_type = CY_ENTERPRISE_SECURITY_EAP_TYPE_MSCHAPV2;
             ent_params->phase2.tunnel_auth_type = CY_ENTERPRISE_SECURITY_TUNNEL_TYPE_MSCHAPV2;
             if ( password != NULL )
             {
                 strncpy( ent_params->phase2.inner_password, password, CY_ENTERPRISE_SECURITY_MAX_PASSWORD_LENGTH );
                          ent_params->phase2.inner_password[ CY_ENTERPRISE_SECURITY_MAX_PASSWORD_LENGTH - 1 ] = '\0';
             }
             ent_params->phase2.inner_password[ CY_ENTERPRISE_SECURITY_MAX_PASSWORD_LENGTH - 1 ] = '\0';
        }
        else if( eap_type == CY_ENTERPRISE_SECURITY_EAP_TYPE_TTLS )
        {
             password = sigmadut_get_string(SIGMADUT_PASSWORD);
             if ( password != NULL )
             {
                 strncpy( ent_params->phase2.inner_password, password, CY_ENTERPRISE_SECURITY_MAX_PASSWORD_LENGTH );
                          ent_params->phase2.inner_password[ CY_ENTERPRISE_SECURITY_MAX_PASSWORD_LENGTH - 1 ] = '\0';
             }
             ent_params->phase2.inner_password[ CY_ENTERPRISE_SECURITY_MAX_PASSWORD_LENGTH - 1 ] = '\0';
             ent_params->phase2.tunnel_auth_type = CY_ENTERPRISE_SECURITY_TUNNEL_TYPE_EAP;
             ent_params->phase2.inner_eap_type = CY_ENTERPRISE_SECURITY_EAP_TYPE_MSCHAPV2;
        }
    }
    if( ent_params->is_client_cert_required == 1 )
    {
        ent_params->client_cert = (char *)wfa_wifi_client_cert;
        ent_params->client_key =  (char *)wfa_wifi_client_priv_key;
    }
    else
    {
        ent_params->client_cert = NULL;
        ent_params->client_key = NULL;
    }
    ent_params->ca_cert = (char *)wfa_root_ca_cert;

    ssid = sigmadut_get_string(SIGMADUT_SSID);
    memcpy( ent_params->ssid, ssid, SSID_NAME_SIZE );
    ent_params->ssid[ SSID_NAME_SIZE - 1 ] = '\0';

    if (username != NULL )
    {
        strncpy( ent_params->outer_eap_identity, (char *)username, CY_ENTERPRISE_SECURITY_MAX_IDENTITY_LENGTH );
    }
    ent_params->outer_eap_identity[ CY_ENTERPRISE_SECURITY_MAX_IDENTITY_LENGTH - 1 ] = '\0';

    ent_params->eap_type = eap_type;
    ent_params->auth_type = auth_type;

    result = cy_enterprise_security_create( &handle, ent_params );
    if( result != CY_RSLT_SUCCESS)
    {
        printf( "Enterprise Security instance creation failed with error %u\n", (unsigned int)result );
        return result;
    }
    sigmadut_set_enterprise_security_handle(handle);
    return result;
}
#endif

void sigmadut_set_eap_type ( wpa2_ent_eap_type_t eap_type)
{
   sigmadut_obj.ent_eap_type = eap_type;
}

wpa2_ent_eap_type_t sigmadut_get_eap_type(void)
{
   return sigmadut_obj.ent_eap_type;
}
