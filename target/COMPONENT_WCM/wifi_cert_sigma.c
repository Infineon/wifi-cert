/*
 * Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software"), is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
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


	result = cy_rtos_init_mutex(&(sigmadut_obj.sigmadut_mutex));
	if(CY_RSLT_SUCCESS != result)
	{
	    printf("initialization of tx_done_mutex failed result:%ld\n", result);
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

        default:
        	break;
    }
    return 0;
}

void sigmadut_init_stream_table (void )
{
	int i;
	cy_rslt_t err;
	thread_details_t* detail = NULL;

	for( i = 0; i < NUM_STREAM_TABLE_ENTRIES; i++ )
	{
	    detail = (thread_details_t *)sigmadut_obj.stream_table[i].thread_ptr;
		if( detail != NULL )
		{
		    if ( detail->thread_handle != NULL )
		    {
		        err = cy_rtos_join_thread(&detail->thread_handle);
		        if ( err != CY_RSLT_SUCCESS )
		        {
		            printf( "Error in Join thread.\n" );
		        }
		    }
		    free(detail->stack_mem);
		    free(detail);
		}
	}
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

int sigmadut_set_traffic_int ( SIGMADUT_TRAFFIC_DATA_INT_TYPE_T type, int stream_id, int data )
{
	if ( ( stream_id  < 0 ) || ( stream_id > NUM_STREAM_TABLE_ENTRIES) )
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

	if ( ( stream_id  < 0 ) || ( stream_id > NUM_STREAM_TABLE_ENTRIES) )
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
	if ( ( stream_id  < 0 ) || ( stream_id > NUM_STREAM_TABLE_ENTRIES) )
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
	if ( ( stream_id  < 0 ) || ( stream_id > NUM_STREAM_TABLE_ENTRIES) )
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
	if ( ( stream_id  < 0 ) || ( stream_id > NUM_STREAM_TABLE_ENTRIES) )
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

	if ( ( stream_id  < 0 ) || ( stream_id > NUM_STREAM_TABLE_ENTRIES) )
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
