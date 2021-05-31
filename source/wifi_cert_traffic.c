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

#ifndef WIFICERT_NO_HARDWARE
#include "lwipopts.h"
#include "lwip/sockets.h"
#include "lwip/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/icmp.h"
#include "lwip/prot/ip4.h"
#endif
#include "wifi_cert_commands.h"
#include "wifi_traffic_api.h"
#include <stdlib.h>

ping_stats_t ping_stats;

int udp_rx( traffic_stream_t* ts )
{   
    int retval;    
    retval = cywifi_setup_rx_traffic_stream(ts);    
    return retval;
}

int udp_tx ( traffic_stream_t* ts )
{
    int retval;
    cy_rtos_delay_milliseconds(SIGMA_AGENT_DELAY_1MS);
    retval = cywifi_setup_tx_traffic_stream(ts);    
    return retval;	
}

int udp_transactional( traffic_stream_t* ts )
{
    int retval;    
    retval = cywifi_setup_transactional_traffic_stream(ts);    
    return retval;	
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
int wifi_traffic_send_ping( void *arg )
{
    cywifi_setup_ping_traffic(arg, &ping_stats);
    return 0;
}

/*!
 ******************************************************************************
 * Return value of num_ping_requests and num_ping_replies
 * @return  0 for success, otherwise error
 */

int wifi_traffic_stop_ping( void )
{
    printf("\nstatus,COMPLETE,sent,%u,replies,%u\n", (unsigned int)ping_stats.num_ping_requests,(unsigned int)ping_stats.num_ping_replies);
    return 0;
}

/**
 *  Prepare the contents of an echo ICMP request packet
 *
 *  @param iecho          : Pointer to an icmp_echo_hdr structure in which the ICMP packet will be constructed
 *  @param len            : The length in bytes of the packet buffer passed to the iecho parameter
 *  @param ping_seqnum    : The Ping sequence number
 *
 */

void wifi_ping_prepare_echo( icmp_echo_hdr_t *iecho, int len, uint16_t ping_seqnum )
{
	int i;
    ICMPH_TYPE_SET( iecho, ICMP_ECHO );
    ICMPH_CODE_SET( iecho, 0 );
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = htons(ping_seqnum );

    /* fill the additional data buffer with some data */
    for ( i = 0; i < (len - (int)sizeof(struct icmp_echo_hdr)); i++ )
    {
        ( (char*) iecho )[sizeof(struct icmp_echo_hdr) + i] = i;
    }
    iecho->chksum = inet_chksum( iecho, len );
}
