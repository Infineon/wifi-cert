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

#include "wifi_cert_commands.h"
#include "cy_result.h"

/** This function sets up RX UDP traffic stream
 *
 * @param ts        : The pointer to the traffic stream structure @ref traffic_stream_t.
 * @return          : 0 = CY_RSLT_SUCCESS
 *                        CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_setup_rx_traffic_stream( traffic_stream_t* ts );


/** This function sets up TX UDP traffic stream
 *
 * @param ts        : The pointer to the traffic stream structure @ref traffic_stream_t.
 * @return          : 0 = CY_RSLT_SUCCESS
 *                        CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_setup_tx_traffic_stream ( traffic_stream_t* ts );


/** This function sets up TX and RX transaction UDP traffic stream
 *
 * @param ts        : The pointer to the traffic stream structure @ref traffic_stream_t.
 * @return          : 0 = CY_RSLT_SUCCESS
 *                        CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_setup_transactional_traffic_stream( traffic_stream_t * ts);


/** This function sets up ICMP(Ping) traffic
 *
*
 * @param arg         : The pointer to the argument
 * @param ping_stats  : The pointer to the ping statistics
 * @return  cy_rslt_t : 0 = CY_RSLT_SUCCESS
 *                        CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_setup_ping_traffic( void *arg, ping_stats_t *ping_stats );


/** This function closes the socket
 *
 * @param socket_handle :  The pointer to socket handle
 * @return   cy_rslt_t  :  0 = CY_RSLT_SUCCESS
 *                             CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/

cy_rslt_t cywifi_close_socket ( void *socket_handle);

/** This function initializes the Secure-Socket layer
 *
 * @return   cy_rslt_t  :  0 = CY_RSLT_SUCCESS
 *                             CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cysigma_socket_init ( void );

/** This function receives ICMP ping reply
 *
 * @param   sock_hnd   :  The pointer to the socket handle
 * @param   sockaddr   :  The pointer to the socket address
 * @param   ping_stats :  The Pointer to the ping_stats structure
 * @return  cy_rslt_t  :  0 = CY_RSLT_SUCCESS
 *                            CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t wifi_ping_recv( void *sock_hnd, void *sockaddr, ping_stats_t *ping_stats );


/** This function gets native thread priority.
 *
 * @param   ts               :  The pointer to the traffic stream
 * @param   ac_priority      :  The WMM AC priority
 * @param   ac_priority_num  :  The AC priority number (TX/RX)
 * @param   native_priority  :  The pointer to the value of native priority to be returned
 * @return  cy_rslt_t        :  0 = CY_RSLT_SUCCESS
 *                                  CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_get_native_priority( traffic_stream_t *ts , int ac_priority, int ac_priority_num, int *native_priority);
