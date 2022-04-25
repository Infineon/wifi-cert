/*
 * Copyright 2022, Cypress Semiconductor Corporation (an Infineon company) or
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
#include "lwipopts.h"
#include "lwip/sockets.h"
#include "lwip/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/api.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/ip4.h"
#include "lwip/prot/icmp.h"
#include "cy_result.h"
#include "wifi_cert_commands.h"
#include "wifi_traffic_api.h"
#include "wifi_cert_sigma.h"
#include "wifi_cert_sigma_api.h"
#include "cy_nw_helper.h"
#include "cy_lwip.h"

/* secure socket Header files */
#include "cy_secure_sockets.h"
#include "cy_tls.h"

uint8_t ping_data_buffer[MAX_PING_PAYLOAD_SIZE] = {0};
uint16_t   ping_seqnum; /* ping sequence number */

cy_rslt_t cywifi_setup_rx_traffic_stream( traffic_stream_t* ts )
{
    int response =    CY_RSLT_SUCCESS;
    cy_socket_t sock_handle;
    cy_socket_sockaddr_t sockaddr;
    cy_socket_sockaddr_t remote_addr = {0};
    cy_nw_ip_address_t nw_ip_addr;
    uint32_t data_length = UDP_RX_BUFSIZE;
    uint32_t timeout = TRAFFIC_AGENT_SOCKET_TIMEOUT; // Milliseconds
    struct netif *net = cy_lwip_get_interface( CY_LWIP_STA_NW_INTERFACE );
    cy_socket_ip_mreq_t multicast_addr;
    uint32_t bytes_recvd = 0;
    uint8_t *rx_packet = NULL;
    uint32_t sockaddr_size = sizeof(cy_socket_sockaddr_t);

    rx_packet = (uint8_t *)malloc ( TX_MAX_PAYLOAD_SIZE );
    if ( rx_packet == NULL )
    {
        printf("Failed to get tx packet\n");
        return CY_RSLT_SUCCESS;
    }
    memset(rx_packet, 0, TX_MAX_PAYLOAD_SIZE );

    /* Create UDP Socket */
    response = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_DGRAM, CY_SOCKET_IPPROTO_UDP, (cy_socket_t *)&sock_handle);
    if (response != CY_RSLT_SUCCESS)
    {
        printf( "UDP socket create failed ret:%d\n", response);
        return CY_RSLT_SUCCESS;
    }

    response = cy_socket_setsockopt(sock_handle, CY_SOCKET_SOL_SOCKET, CY_SOCKET_SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* Set the receive timeout on local socket so ping will time out. */
    if(response != CY_RSLT_SUCCESS)
    {
        printf("Error while setting socket timeout for UDP TRX \n");
        return CY_RSLT_SUCCESS;
    }

    memset(&multicast_addr, 0, sizeof(cy_socket_ip_mreq_t));

    ts->rx_socket = (void *)sock_handle;

    if ( ts->profile == PROFILE_MULTICAST)
    {
        cy_nw_str_to_ipv4((const char *)ts->dest_ipaddr, &nw_ip_addr);
    }
    else
    {
        cy_nw_str_to_ipv4((const char *)ts->src_ipaddr, &nw_ip_addr);
    }

    remote_addr.ip_address.version = CY_SOCKET_IP_VER_V4;
    remote_addr.ip_address.ip.v4 = nw_ip_addr.ip.v4;
    remote_addr.port = ts->dest_port;
    sockaddr.ip_address.version = CY_SOCKET_IP_VER_V4;
#if LWIP_IPV6
    sockaddr.ip_address.ip.v4 = net->ip_addr.u_addr.ip4.addr;
#else
    sockaddr.ip_address.ip.v4 = net->ip_addr.addr;
#endif
    sockaddr.port = ts->src_port;

    /* Bind socket to local port */
    response = cy_socket_bind(sock_handle, &sockaddr, (uint32_t)sizeof(cy_socket_sockaddr_t));
    if ( response != CY_RSLT_SUCCESS)
    {
        cy_socket_delete(sock_handle);
        printf( "UDP socket bind failed response: %d\n", response);
        free(rx_packet);
        ts->rx_socket = NULL;
        return CY_RSLT_SUCCESS;
    }

    if ( ts->profile == PROFILE_MULTICAST )
    {
        /* join multicast group */
    	multicast_addr.if_addr.version = CY_SOCKET_IP_VER_V4;
    	multicast_addr.multi_addr.version = CY_SOCKET_IP_VER_V4;
#if LWIP_IPV6
    	multicast_addr.if_addr.ip.v4 = net->ip_addr.u_addr.ip4.addr;
#else
    	multicast_addr.if_addr.ip.v4 = net->ip_addr.addr;
#endif
    	multicast_addr.multi_addr.ip.v4 = nw_ip_addr.ip.v4;

    	response = cy_socket_setsockopt(sock_handle, CY_SOCKET_SOL_IP, CY_SOCKET_SO_JOIN_MULTICAST_GROUP, &multicast_addr, sizeof(cy_socket_ip_mreq_t));

    	if ( response != CY_RSLT_SUCCESS )
    	{
    		 printf( "Could not join multicast group\n\n" );
    		 free(rx_packet);
    		 cy_socket_delete(sock_handle);
    		 ts->rx_socket = NULL;
    		 return CY_RSLT_SUCCESS;
    	}
    }

    while ( ts->enabled )
    {
        /* Wait for UDP packet */
    	response =  cy_socket_recvfrom(sock_handle, (void *)rx_packet, data_length, NETCONN_NOFLAG, &remote_addr, &sockaddr_size, &bytes_recvd);
        if ( response == CY_RSLT_MODULE_SECURE_SOCKETS_TIMEOUT )
        {
            cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_1MS);
            continue;
        }
        else if ( response != CY_RSLT_SUCCESS )
        {
    	   cy_socket_delete(sock_handle);
    	   free(rx_packet);
    	   ts->rx_socket = NULL;
    	   return CY_RSLT_SUCCESS;
        }
        if ( (bytes_recvd > 0) && (ts->enabled) ) // Also check the enabled flag since another thread may have set it to 0
        {
    	   sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_FRAME_RECVD, ts, 1);
    	   sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_BYTES_RECVD, ts, bytes_recvd);
        }

    }/* end of while */

    if ( ts->profile == PROFILE_MULTICAST )
    {
    	response = cy_socket_setsockopt(sock_handle, CY_SOCKET_SOL_IP, CY_SOCKET_SO_LEAVE_MULTICAST_GROUP, &multicast_addr, sizeof(cy_socket_ip_mreq_t));

    	if ( response != CY_RSLT_SUCCESS )
    	{
    		printf( "Could not leave multicast group\n\n" );
    		free(rx_packet);
    	    cy_socket_delete(sock_handle);
    	    ts->rx_socket = NULL;
    	    return CY_RSLT_SUCCESS;
    	}
    }
    free(rx_packet);
    cy_socket_delete(sock_handle);
    ts->rx_socket = NULL;
    return response;
}

cy_rslt_t cywifi_setup_tx_traffic_stream ( traffic_stream_t* ts )
{
    uint32_t  rate_check_time, current_time, rest_time;
	int frames_sent_this_period = 0;
	int frames_required_this_period = 0;
	int response = CY_RSLT_SUCCESS;
	cy_socket_t sock_handle;
	cy_socket_sockaddr_t sockaddr = {0};
	cy_socket_sockaddr_t remote_addr = {0};
	cy_nw_ip_address_t nw_ip_addr;
	struct netif *net = cy_lwip_get_interface( CY_LWIP_STA_NW_INTERFACE );
	cy_socket_ip_mreq_t multicast_addr;
	uint32_t bytes_sent = 0;
	uint8_t *tx_packet = NULL;
	int free_entries = 0;
	cy_mutex_t *sigmdut_mutex_ptr = NULL;
    // uint8_t tos_priority[4] = { 0x00, 0x40, 0xA0, 0xE0 }; // Best Effort, Background, Video, Voice
    uint8_t tos;
	tx_packet = (uint8_t *)malloc ( TX_MAX_PAYLOAD_SIZE );
	if ( tx_packet == NULL )
	{
	    printf("Failed to get tx packet\n");
	    return CY_RSLT_SUCCESS;
	}
	memset(tx_packet, 0, TX_MAX_PAYLOAD_SIZE );
    memset(&multicast_addr, 0, sizeof(cy_socket_ip_mreq_t));

    free_entries =  NUM_STREAM_TABLE_ENTRIES - sigmadut_find_num_active_streams();
    sigmdut_mutex_ptr = sigmadut_get_mutex_instance();
    if ( sigmdut_mutex_ptr == NULL )
    {
        printf("Failed to get mutex instance\n");
        return CY_RSLT_SUCCESS;
    }
   	/* Create UDP Socket */
	response = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_DGRAM, CY_SOCKET_IPPROTO_UDP, (cy_socket_t *)&sock_handle);
	if (response != CY_RSLT_SUCCESS)
	{
	    printf( "UDP socket create failed ret:%d\n", response);
	    return response;
	}
	ts->tx_socket = (void *)sock_handle;

	switch ( ts->ac)
	{
	    case WMM_AC_BE:
	         tos = 0x00;
	         break;
	    case WMM_AC_BK:
	         tos = 0x40;
	         break;
	    case WMM_AC_VI:
	         tos = 0xA0;
	         break;
	    case WMM_AC_VO:
	         tos = 0xE0;
	         break;
	    default:
	         break;
	}

	/* TODO This has to be implemented for 5.2.27 test case */
	response = cy_socket_setsockopt(sock_handle, CY_SOCKET_SOL_IP, CY_SOCKET_SO_IP_TOS, &tos, sizeof(tos));

	/* Set IP precedence on a local socket */
	if(response != CY_RSLT_SUCCESS)
	{
	    printf("Error while setting IPTOS for UDP TX \n");
	    return response;
	}

	cy_nw_str_to_ipv4((const char *)ts->dest_ipaddr, &nw_ip_addr);
	remote_addr.ip_address.version = CY_SOCKET_IP_VER_V4;
	remote_addr.ip_address.ip.v4 = nw_ip_addr.ip.v4;
	remote_addr.port = ts->dest_port;
	sockaddr.ip_address.version = CY_SOCKET_IP_VER_V4;
#if LWIP_IPV6
	sockaddr.ip_address.ip.v4 = net->ip_addr.u_addr.ip4.addr;
#else
	sockaddr.ip_address.ip.v4 = net->ip_addr.addr;
#endif
	sockaddr.port = ts->src_port;

	/* Bind socket to local port */
	response = cy_socket_bind((void * )sock_handle, &sockaddr, (uint32_t)sizeof(cy_socket_sockaddr_t));
	if ( response != CY_RSLT_SUCCESS)
	{
	   free(tx_packet);
	   cy_socket_delete((void *)sock_handle);
	   ts->tx_socket = NULL;
	   return response;
	}

	if ( ts->profile == PROFILE_MULTICAST )
	{
		 /* join multicast group */
		 multicast_addr.if_addr.version = CY_SOCKET_IP_VER_V4;
		 multicast_addr.multi_addr.version = CY_SOCKET_IP_VER_V4;
#if LWIP_IPV6
		 multicast_addr.if_addr.ip.v4= net->ip_addr.u_addr.ip4.addr;
#else
		 multicast_addr.if_addr.ip.v4= net->ip_addr.addr;
#endif
		 multicast_addr.multi_addr.ip.v4 = nw_ip_addr.ip.v4;

		 response = cy_socket_setsockopt((void *)sock_handle, CY_SOCKET_SOL_IP, CY_SOCKET_SO_JOIN_MULTICAST_GROUP, &multicast_addr, sizeof(cy_socket_ip_mreq_t));

		 if ( response != CY_RSLT_SUCCESS )
		 {
		     printf( "Could not join multicast group\n\n" );
		     free(tx_packet);
		     cy_socket_delete((void *)sock_handle);
		     ts->tx_socket = NULL;
		     return CY_RSLT_MW_ERROR;
		 }
	}

	frames_sent_this_period = 0;
	if ( ts->frame_rate > 0 )
	{
	    frames_required_this_period = ( ts->frame_rate * 1000 ) / ( 1000000 / RATE_CHECK_TIME_LIMIT );
	}

	rate_check_time = GET_CURRENT_TIME + RATE_CHECK_TIME_LIMIT; // Need to check periodically whether we have sent required frames and should rest
	while ( ts->enabled )
	{
	      if ( GET_CURRENT_TIME > (uint32_t)ts->stop_time )
	      {
	    	  if ( ts->profile == PROFILE_MULTICAST )
	    	  {
	    		  /* leave multicast group */
	    		  response = cy_socket_setsockopt((void *)sock_handle, CY_SOCKET_SOL_IP, CY_SOCKET_SO_LEAVE_MULTICAST_GROUP, &multicast_addr, sizeof(cy_socket_ip_mreq_t));
	    		  if ( response != CY_RSLT_SUCCESS )
	    		  {
	    		      printf( "Could not leave multicast group\n\n response=%d\n", response );
	    		      free(tx_packet);
	    		      cy_socket_delete((void *)sock_handle);
	    		      ts->tx_socket = NULL;
	    		      return CY_RSLT_MW_ERROR;
	    		  }
	    	 }
	    	 free(tx_packet);
	       	 cy_socket_delete((void *)sock_handle);
	    	 ts->tx_socket = NULL;
	         break;
	      }
	      response =  cy_socket_sendto((void *)sock_handle, tx_packet, ts->payload_size, NETCONN_NOFLAG, &remote_addr, sizeof(cy_socket_sockaddr_t), &bytes_sent);
	      if ( response != CY_RSLT_SUCCESS )
	      {
	          if ( response == CY_RSLT_MODULE_SECURE_SOCKETS_NOMEM) //&& ( free_entries == NUM_STREAM_TABLE_ENTRIES - 1))
	          {
                  if (free_entries == NUM_STREAM_TABLE_ENTRIES - 1)
	              {
	                  /* out of memory wait for 1ms */
	                   cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_1MS);
	                   continue;
	              }
	              else
	              {
	                  /* out of memory wait for SIGMA_AGENT_DELAY_MULTI_STREAM delay */
	                  cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_MULTI_STREAM);
	                  continue;
	              }
	          }
	          else
	          {
	              if ( ts->profile == PROFILE_MULTICAST )
	    	      {
	    	          /* leave multicast group */
	    		      response = cy_socket_setsockopt((void *)sock_handle, CY_SOCKET_SOL_IP, CY_SOCKET_SO_LEAVE_MULTICAST_GROUP, &multicast_addr, sizeof(cy_socket_ip_mreq_t));
	    		      if ( response != CY_RSLT_SUCCESS )
	    		      {
	    		          printf( "Could not leave multicast group\n\n response=%d\n", response );
	    		          free(tx_packet);
	    		  	      cy_socket_delete((void *)sock_handle);
	    		  	      ts->tx_socket = NULL;
	    		  	      return CY_RSLT_SUCCESS;
	    		      }
	    	      }

	    	      free(tx_packet);
	              cy_socket_delete((void *)sock_handle);
	    	      ts->tx_socket = NULL;
	              break;
	          }
	      }
	      else
	      {
	          sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_FRAME_SENT, ts, 1);
	    	  sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_BYTES_SENT, ts, bytes_sent);
	    	  ++frames_sent_this_period;
	      }
	      if ( ts->frame_rate > 0 )
	      {
	           if ( frames_sent_this_period >= frames_required_this_period )
	           {
	               frames_sent_this_period = 0;
	               current_time = GET_CURRENT_TIME;
	               if ( current_time < rate_check_time )
	               {
	                   rest_time = rate_check_time - current_time;
	               }
	               else
	               {
	                   rest_time = 0;
	               }
	               rate_check_time += RATE_CHECK_TIME_LIMIT;
	               if ( ( rest_time > 0 ) && ( rest_time <= RATE_CHECK_TIME_LIMIT ) )
	               {
	            	   cy_rtos_delay_milliseconds(rest_time);
	               }
	          }
	          else
	          {
	              if ( GET_CURRENT_TIME >= rate_check_time )
	              {
	                  rate_check_time += RATE_CHECK_TIME_LIMIT;
	                  frames_sent_this_period = 0;
	              }
	          }
	      } /* if */
 	} /* end of while */
	return 0;
}

cy_rslt_t cywifi_setup_transactional_traffic_stream( traffic_stream_t* ts )
{
    uint32_t data_length = MAX_PAYLOAD_SIZE;
    uint32_t timeout = 10; // Milliseconds
	cy_socket_sockaddr_t sockaddr = {0};
   	cy_socket_sockaddr_t remote_addr = {0};
 	int response = CY_RSLT_SUCCESS;
    uint32_t bytes_sent = 0;
    uint32_t bytes_recvd = 0;
    cy_socket_t sock_handle;
    cy_nw_ip_address_t nw_ip_addr;
    uint8_t *rx_packet = NULL;
    uint32_t sockaddr_size = sizeof(cy_socket_sockaddr_t);

    rx_packet = (uint8_t *)malloc ( TX_MAX_PAYLOAD_SIZE );
    if ( rx_packet == NULL )
    {
        printf("Failed to get tx packet\n");
        return CY_RSLT_SUCCESS;
    }
    memset(rx_packet, 0, TX_MAX_PAYLOAD_SIZE );


 	struct netif *net = cy_lwip_get_interface( CY_LWIP_STA_NW_INTERFACE );

 	/* Create UDP Socket */
 	response = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_DGRAM, CY_SOCKET_IPPROTO_UDP, (cy_socket_t *)&sock_handle);
 	if (response != CY_RSLT_SUCCESS)
 	{
 	    printf( "UDP socket create failed ret:%d\n", response);
 	    free(rx_packet);
 	    return response;
 	}

    ts->rx_socket = sock_handle;

    response = cy_socket_setsockopt(sock_handle, CY_SOCKET_SOL_SOCKET, CY_SOCKET_SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* Set the receive timeout on local socket so ping will time out. */
    if(response != CY_RSLT_SUCCESS)
    {
        printf("Error while setting socket timeout for UDP TRX \n");
        free(rx_packet);
    	return response;
    }

    cy_nw_str_to_ipv4((const char *)ts->dest_ipaddr, &nw_ip_addr);
   	remote_addr.ip_address.version = CY_SOCKET_IP_VER_V4;
   	remote_addr.ip_address.ip.v4 = nw_ip_addr.ip.v4;
   	remote_addr.port = ts->dest_port;
   	sockaddr.ip_address.version = CY_SOCKET_IP_VER_V4;
#if LWIP_IPV6
   	sockaddr.ip_address.ip.v4 = net->ip_addr.u_addr.ip4.addr;
#else
   	sockaddr.ip_address.ip.v4 = net->ip_addr.addr;
#endif
   	sockaddr.port = ts->src_port;

   	/* Bind socket to local port */
   	response = cy_socket_bind((void * )sock_handle, &sockaddr, (uint32_t)sizeof(cy_socket_sockaddr_t));
   	if ( response != CY_RSLT_SUCCESS)
   	{
       cy_socket_delete((void *)sock_handle);
       printf( "UDP socket bind failed response: %d\n", response);
       free(rx_packet);
       ts->rx_socket = NULL;
       return CY_RSLT_SUCCESS;
   	}

    while ( ts->enabled )
    {
        /* Wait for UDP packet */
    	response =  cy_socket_recvfrom((void *)sock_handle, (void *)rx_packet, data_length, NETCONN_NOFLAG, &remote_addr, &sockaddr_size, &bytes_recvd);
    	if ( response == CY_RSLT_MODULE_SECURE_SOCKETS_TIMEOUT )
    	{
    	    cy_rtos_delay_milliseconds(1);
    	    continue;
    	}
    	else if ( response != CY_RSLT_SUCCESS )
    	{
    	    printf (" cy_socket_recvfrom failed ret:%d\n", response );
    	    free(rx_packet);
    	    cy_socket_delete((void *)sock_handle);
    	    ts->rx_socket = NULL;
    	    return CY_RSLT_SUCCESS;
    	}

        if ( (bytes_recvd > 0)  && (ts->enabled)) // Also check the enabled flag since another thread may have set it to 0
        {
            sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_FRAME_RECVD, ts, 1);
            sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_BYTES_RECVD, ts, bytes_recvd);

            response =  cy_socket_sendto((void *)sock_handle, rx_packet, ts->payload_size, NETCONN_NOFLAG, &remote_addr, sizeof(cy_socket_sockaddr_t), &bytes_sent);
           	if ( response != CY_RSLT_SUCCESS )
           	{
                printf("udp send failure :%d\n", response );
                free(rx_packet);
                cy_socket_delete((void *)sock_handle);
                ts->rx_socket = NULL;
                break;
            }
            else
            {
            	sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_FRAME_SENT, ts, 1);
            	sigmadut_traffic_increment_data(SIGMADUT_TRAFFIC_BYTES_SENT, ts, bytes_sent);
            }

        } /* end of if */
    } /* end of while */

    free(rx_packet);
    cy_socket_delete((void *)sock_handle);
    ts->rx_socket = NULL;

    return response;
}

/*!
 ******************************************************************************
 * Implementation of traffic send ping (with stripped down set of arguments)
 * Sends ICMP pings to the indicated host or IP address
 * argv[0]: traffic_send_ping
 * argv[1]: IP address of target
 * argv[2]: frame size
 * argv[3]: frame rate
 * argv[4]: duration
 * @return  0 for success, otherwise error
 */
cy_rslt_t cywifi_setup_ping_traffic( void *arg, ping_stats_t *ping_stats  )
{
	int socket_handle = -1;
	ping_thread_details_t* ping_thread = (ping_thread_details_t *)arg;
	int timeout = 10; // Milliseconds
	char** argv = (char **)ping_thread->arg;
	char dstip[16] = {0};
	char nulladdr[16] = {0};
	int duration, frame_interval, wait_time;
	int num_frames,sleep_time;
	int num_timeouts;
	int frame_size,frame_rate, err;
	cy_time_t send_time;
	cy_time_t recv_time;
	struct timeval recv_timeout;
	struct sockaddr_in dst_addr;
	cy_nw_ip_address_t nw_ip_addr;
	cy_rslt_t result;

	socket_handle = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);

	if ( socket_handle < 0 )
	{
	    printf("socket open failed result=%d\n", socket_handle );
	    return CY_RSLT_SUCCESS;
	}

	/* convert the timeout into struct timeval */
	recv_timeout.tv_sec  = (long)(timeout / 1000U);
	recv_timeout.tv_usec = (long)((timeout % 1000U) * 1000U);

	/* Set the receive timeout on local socket so ping will time out. */
	if(lwip_setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(struct timeval)) != ERR_OK)
	{
		printf("Error while setting socket timeout for Ping \n");
	    return CY_RSLT_SUCCESS;
	}

	ping_stats->num_ping_requests = 0; // Global count of the number of ping requests for the latest call of this function
	ping_stats->num_ping_replies = 0; // Global count of the number of ping replies for the latest call of this function

	memcpy(dstip, argv[2], (sizeof(dstip) - 1 ));

	frame_size = (int)atoi(argv[4]);
	frame_rate = (int)atoi(argv[6]); // Frames per second

	// XXX Some scripts send a decimal frame rate even though the variable is defined as a short int. This fix may not work for all test plans.
	if ( frame_rate == 0 )
	{
	    frame_rate = 1;
	}

	duration = (int) atoi(argv[8]);  // How long to ping for in seconds

	frame_interval = 1000 / frame_rate;
	wait_time = 0;
	num_frames = frame_rate * duration;
	num_timeouts = num_frames;
	sleep_time = 0;

	if ( memcmp(dstip, nulladdr, sizeof(dstip)) == 0 )
	{
	    printf("host is null\n");

	    /* close the socket */
	    lwip_close(socket_handle);
	    return CY_RSLT_SUCCESS;
    }

	cy_nw_str_to_ipv4((const char *)dstip, &nw_ip_addr);
	wait_time = frame_interval - 5;

	recv_timeout.tv_sec = wait_time / 1000U;
	recv_timeout.tv_usec = (wait_time % 1000U) * 1000U;

	lwip_setsockopt( socket_handle, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof( recv_timeout ) );


	struct icmp_echo_hdr *iecho;
	int ping_size = (int)((int)sizeof(struct icmp_echo_hdr) + frame_size);

	/* Allocate memory for packet */
	if ( !( iecho =  (struct icmp_echo_hdr * )mem_malloc( ping_size ) ) )
	{
	     printf("malloc failed\n");
	     /* close the socket */
	     lwip_close(socket_handle);
	     return CY_RSLT_SUCCESS;
	}

	ping_seqnum = 1; /* ping sequence number */

	dst_addr.sin_len         = sizeof( dst_addr );
	dst_addr.sin_family      = AF_INET;
	dst_addr.sin_addr.s_addr = nw_ip_addr.ip.v4;

    while ( (  num_frames > 0  ) && ( num_timeouts > 0 ))
    {
    	/* Construct ping request */
    	wifi_ping_prepare_echo( iecho, ping_size, ping_seqnum );

    	err = lwip_sendto( socket_handle, iecho, ping_size, 0, (struct sockaddr*) &dst_addr, dst_addr.sin_len );

        /* WPA3 SAE-5.3 Negative test expects Ping Requests to be greater than 0
         * when association is not successful with AP hence increment ping request
         * and record time as network will return error
         */
    	++(ping_stats->num_ping_requests);

        /* Record time ping was sent */
        cy_rtos_get_time(&send_time);

    	if ( err <= ERR_OK)
    	{
    	    --num_timeouts;
    	    cy_rtos_delay_milliseconds(frame_interval);
    		continue;
    	}

      	/* Wait for ping reply */
    	result = wifi_ping_recv( &socket_handle,  &dst_addr, ping_stats);

    	if ( CY_RSLT_SUCCESS == result )
    	{
    	       // printf( "Ping Reply %dms\n", (int)( GET_CURRENT_TIME - send_time ) );
    	       ++(ping_stats->num_ping_replies);
    	}
    	else
    	{
    		;
    	    // printf( "Ping timeout\n" );

    	}
    	cy_rtos_get_time(&recv_time);

    	/* Sleep until time for next ping */
    	sleep_time = frame_interval - ( recv_time - send_time );

    	if ( sleep_time > 0 )
    	{
    		cy_rtos_delay_milliseconds(sleep_time);
    	}
    	--num_frames;
    	ping_seqnum++;
    	PING_SEQNO_INC(iecho,ping_seqnum);
    } /* end of while */

    /* close the socket */
   	lwip_close(socket_handle);

   	/* free packet */
	mem_free( iecho );

    return CY_RSLT_SUCCESS;
}

/**
 *  Receive a Ping reply
 *
 *  Waits for a ICMP echo reply (Ping reply) to be received using the specified socket. Compares the
 *  sequence number, and ID number to the last ping sent, and if they match, returns ERR_OK, indicating
 *  a valid ping response.
 *
 *  @param socket_hnd : The handle for the local socket through which the ping reply will be received
 *
 *  @return  CY_RSLT_SUCCESS if valid reply received, ERR_TIMEOUT otherwise
 */

cy_rslt_t wifi_ping_recv( void *sock_hnd, void *sockaddr, ping_stats_t *ping_stats )
{
    int len;
    int fromlen;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;
    int sock_handle = *(int *)sock_hnd;
    struct sockaddr_in *from = ( struct sockaddr_in *)sockaddr;

    memset(ping_data_buffer, 0, sizeof(ping_data_buffer ));
    while (( len = lwip_recvfrom(sock_handle, ping_data_buffer, sizeof(ping_data_buffer), 0, (struct sockaddr*) from, (socklen_t*) &fromlen)) > 0 )
    {
    	if ( len >= (int) ( sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr) ) )
        {
            iphdr = (struct ip_hdr *) ping_data_buffer;
            iecho = (struct icmp_echo_hdr *) ( ping_data_buffer + ( IPH_HL( iphdr ) * 4 ) );

            if ( ( iecho->id == PING_ID ) &&
                 ( iecho->seqno == htons( ping_seqnum ) ) &&
                 ( ICMPH_TYPE( iecho ) == ICMP_ER ) )
            {
                return CY_RSLT_SUCCESS; /* Echo reply received - return success */
            }
        }
    }
    return CY_RSLT_MODULE_SECURE_SOCKETS_TIMEOUT; /* No valid echo reply received before timeout */
}

cy_rslt_t cywifi_close_socket ( void *socket_handle)
{
	int sock_hnd;
	if ( socket_handle == NULL )
	{
		return CY_RSLT_MW_ERROR;
	}
    sock_hnd = *( int *)socket_handle;
    lwip_close(sock_hnd);

	return CY_RSLT_SUCCESS;
}

cy_rslt_t cysigma_socket_init ( void )
{
	cy_rslt_t ret = CY_RSLT_SUCCESS;

    /* Secure-Socket layer init */
	ret = cy_socket_init();
	if ( ret != CY_RSLT_SUCCESS )
	{
	  	printf("%s cy_socket_init failed ret:%u\n", __func__, (unsigned int)ret );
	}
	return ret;
}

cy_rslt_t cywifi_get_native_priority( traffic_stream_t *ts, int ac_priority, int ac_priority_num, int *native_priority)
{
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    int prio = 1; // Default + 1 priority for TRAFFIC_ANY direction
    int priority_calc = 0;
    int free_entries = 0;

    if ( ts == NULL )
    {
        printf("traffic stream is NULL\n");
        return CY_RSLT_MW_ERROR;
    }
    free_entries =  NUM_STREAM_TABLE_ENTRIES - sigmadut_find_num_active_streams();
    if ( ts->direction == TRAFFIC_SEND)
    {
        if ( free_entries == NUM_STREAM_TABLE_ENTRIES - 1)
        {
            /* priority adjustment of TX w.r.t RX thread is not needed as only one stream is active */
            prio =  ac_priority * PRIO_OFFSET_BETWEEN_AC + ac_priority_num;
            priority_calc = prio - free_entries + SINGLE_STREAM_PRIO_BOOST_OF_RX_THREAD_WRT_TX;
        }
        else
        {
            /* Priority of TX thread will be always be lower than the RX */
            prio =  PRIO_BOOST_OF_RX_THREAD_WRT_TX + ac_priority * PRIO_OFFSET_BETWEEN_AC + ac_priority_num;
            priority_calc = prio - FREERTOS_RTOS_TX_ADJUSTMENT;

            /* Low priority PRIO_LOW_THRESH causes TX thread not to run at all */
            if ( PRIORITY_TO_NATIVE_PRIORITY(priority_calc) <= PRIO_LOW_THRESH)
            {
                priority_calc = CY_RTOS_PRIORITY_HIGH - PRIO_LOW_THRESH - 1;
            }
            else if (PRIORITY_TO_NATIVE_PRIORITY(priority_calc) >= CY_RTOS_PRIORITY_HIGH)
            {
                priority_calc = PRIO_HIGH_THRESH;
            }
        }
       // printf("\nprio of tx thread prio:%d ac_priority:%d ac_priority_num:%d free_entries:%d priority_calc:%d native_prio:%d \n", prio, ac_priority, ac_priority_num, free_entries, priority_calc, PRIORITY_TO_NATIVE_PRIORITY(priority_calc) );
    }
    else if ( ts->direction == TRAFFIC_RECV )
    {
        prio =  ac_priority * PRIO_OFFSET_BETWEEN_AC;
        if ( free_entries != NUM_STREAM_TABLE_ENTRIES - 1)
        {
            /* more than one RX stream so get the priority_calc */
            priority_calc = prio - FREERTOS_RTOS_RX_ADJUSTMENT;
            if ( PRIORITY_TO_NATIVE_PRIORITY(priority_calc) >= CY_RTOS_PRIORITY_HIGH )
            {
                /* do not allow RX stream to be greater than CY_RTOS_PRIORITY_HIGH when
                 * multiple streams are running
                 */
                priority_calc = PRIO_RX_HIGH_THRESH;
            }
        }
       // printf("\nprio of rx thread prio:%d ac_priority:%d ac_priority_num:%d free_entries:%d  priority_calc:%d native_prio:%d\n", prio, ac_priority, ac_priority_num, free_entries, priority_calc, PRIORITY_TO_NATIVE_PRIORITY(priority_calc) );
    }
    *native_priority = PRIORITY_TO_NATIVE_PRIORITY(priority_calc);
    return ret;
}
#endif /*  WIFICERT_NO_HARDWARE */
