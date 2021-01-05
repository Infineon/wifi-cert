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

#define GET_TARGET_NAME_STR(tgt_name)   #tgt_name
#define GET_TARGET_NAME(tgt_name)       GET_TARGET_NAME_STR(tgt_name)
#define MODEL GET_TARGET_NAME(TARGET_NAME)
#define PLATFORM MODEL
#define PLATFORM_VERSION "1"

#define PRIO_OFFSET_BETWEEN_AC 1

/* All TX thread priority will be atleast this much lower than RX thread */
#define PRIO_BOOST_OF_RX_THREAD_WRT_TX 3

#define PRIORITY_TO_NATIVE_PRIORITY(priority) (osPriorityHigh - (priority ))
#define TRAFFIC_END_OF_THREAD( thread )

/** Sigma Certification Commands
 *  Class that represents a Sigma DUT Agent
 *  Common inteface to external test agent (UCC) and
 *  interface to host MBED Stack and WLAN Chip
 */
class SigmaCertCommands 
{
	public:

	SigmaCertCommands();
	SigmaCertCommands(nsapi_wifi_ap_t ap);

	/** Look up for an internal command in Command Table
	  *
	  *  @return The ssid of the access point
	  */
	CMD_STATUS_T command_lookup() const;

	/* show ip config */
	int show_ip_config( uint8_t interface, char* test_using_dhcp );
	
	/* initialize stream table */
	void init_stream_table( void );
	
	/* find unallocated stream table */
	int find_unallocated_stream_table_entry ( void );
	
	/* find number of active streams */
	int find_num_active_streams (void );

	/* find stream table entry */
    int find_stream_table_entry ( int id );

    /* find stream index */
    int find_stream_index ( int stream_id );

    /* get command table size */
    int get_command_table_size(void);

    void set_ssid ( char *ssid ) {
    	strncpy(_ssid, ssid, (sizeof(_ssid) - 1));
    }
    void set_secruity_type( char *type )
    {
    	strncpy(_sectype, type, (sizeof(_sectype) - 1));
    }

    void set_keymgmt_type ( char *keymgmt )
    {
    	strncpy(_keymgmttype, keymgmt, (sizeof(_keymgmttype) - 1));
    }
    void set_encryption_type ( char * enctype )
    {
    	strncpy ( _encptype, enctype, (sizeof(_encptype) -1 ));
    }
    void set_wepkey_buffer ( uint8_t *wepkey)
    {
    	memcpy(_wepkey_buffer, wepkey, sizeof(_wepkey_buffer));
    }
    void set_passphrase ( char  *passphrase )
    {
    	strncpy(_passphrase, passphrase, (sizeof(_passphrase) - 1));
    }
    void set_wepkey ( wiced_wep_key_t *key )
    {
    	memcpy(&wepkey, key , sizeof(wepkey));
    }
    void set_pmf ( char *pmf )
    {
    	strncpy(_pmf, pmf, (sizeof( _pmf ) - 1 ));
    }

    void set_dhcp ( char *dhcp )
    {
    	strncpy(_using_dhcp, dhcp, sizeof( _using_dhcp ) - 1 );
    }

    void set_bssid( char *bssid )
    {
    	strncpy(_bssid, bssid, sizeof(_bssid) - 1 );
    }

    void set_channel ( char *channel )
    {
    	strncpy(_channel, channel, sizeof(_channel ) - 1 );
    }

    void set_ipaddr ( char *ipaddr )
    {
    	strncpy(dut_ip_addr, ipaddr, sizeof( dut_ip_addr ) - 1 );
    }

    void set_netmask ( char *netmask)
    {
    	strncpy(dut_netmask, netmask, sizeof( dut_netmask ) - 1 );
    }
    void set_interface ( char *interface )
    {
        strncpy(_interface, interface, sizeof(_interface) - 1);
    }
    void set_dutgateway ( char *gw )
    {
    	strncpy(dut_gateway, gw, sizeof( dut_gateway ) - 1 );
    }
    void set_primarydns ( char *pdns)
    {
    	strncpy(dut_primary_dns, pdns, sizeof(dut_primary_dns) - 1);
    }
    void set_secondarydns ( char *sdns )
    {
    	strncpy(dut_secondary_dns, sdns, sizeof(dut_secondary_dns) - 1);
    }

    void set_traffic_stream_enable ( int stream_index, int enable )
    {
    	stream_table[stream_index].enabled = enable;
    }

    void set_traffic_stream_stoptime ( int stream_index, int stoptime )
    {
    	stream_table[stream_index].stop_time = stoptime;
    }

    void set_streamid ( int stream_idx, int stream_id )
    {
    	stream_table[stream_idx].stream_id = (uint32_t)stream_id;
    }

    void set_traffic_profile (int stream_idx, traffic_profile_t profile )
    {
        stream_table[stream_idx].profile = profile;
    }

    void set_traffic_direction ( int stream_idx, traffic_direction_t dir )
    {
    	stream_table[stream_idx].direction = dir;
    }

    void set_traffic_src_ipaddress ( int stream_idx, char *src_ipaddr )
    {
    	memcpy(stream_table[stream_idx].src_ipaddr, src_ipaddr, sizeof(stream_table[stream_idx].src_ipaddr) - 1 );
    }

    void set_traffic_src_port ( int stream_idx, uint16_t port )
    {
       stream_table[stream_idx].src_port = port;
    }

    void set_traffic_dst_ipaddress (int stream_idx, char *dst_ipaddr )
    {
    	memcpy(stream_table[stream_idx].dest_ipaddr, dst_ipaddr, sizeof(stream_table[stream_idx].dest_ipaddr) - 1 );
    }

    void set_traffic_dst_port ( int stream_idx, uint16_t port )
    {
       stream_table[stream_idx].dest_port = port;
    }

    void set_traffic_class ( int stream_idx, qos_access_category_t traffic_class )
    {
    	stream_table[stream_idx].ac = traffic_class;
    }

    void set_traffic_payloadsize ( int stream_idx, int payload_size )
    {
        stream_table[stream_idx].payload_size = payload_size;
    }

    void set_traffic_duration ( int stream_idx, int duration )
    {
    	stream_table[stream_idx].duration = duration;
    }

    void set_traffic_framerate ( int stream_idx, int frame_rate )
    {
        stream_table[stream_idx].frame_rate = frame_rate;
    }

    void set_traffic_frame_sent ( int stream_idx, int frames_sent )
    {
    	stream_table[stream_idx].frames_sent = frames_sent;
    }

    void increment_traffic_frames_sent ( traffic_stream_t *ts )
    {
         ts->frames_sent++;
    }

    void increment_bytes_sent( traffic_stream_t *ts, int size )
    {
    	ts->bytes_sent += size;
    }

    void increment_traffic_frames_received ( traffic_stream_t *ts )
    {
    	ts->frames_received++;
    }

    void increment_bytes_received ( traffic_stream_t *ts, int size )
    {
    	ts->bytes_received += size;
    }
    char *get_ssid (void )
    {
    	return _ssid;
    }

    char *get_bssid( void )
    {
    	return _bssid;
    }

    char *get_channel(void )
    {
    	return _channel;
    }
    char *get_secruity_type (void )
    {
    	return _sectype;
    }
    char *get_keymgmt_type (void )
    {
    	return _keymgmttype;
    }
    char *get_encryption_type(void )
    {
    	return _encptype;
    }
    char *get_passphrase(void)
    {
    	return _passphrase;
    }
    uint8_t *get_wepkey_buffer (void )
    {
    	return _wepkey_buffer;
    }
    char *get_pmf(void)
    {
    	return _pmf;
    }
    char *get_dhcp(void)
    {
    	return _using_dhcp;
    }
    char *get_ipaddr(void)
    {
        return dut_ip_addr;
    }
    char *get_netmask (void )
    {
    	return dut_netmask;
    }
    char *get_dutgateway (void )
    {
    	return dut_gateway;
    }
    char *get_primarydns (void )
    {
    	return dut_primary_dns;
    }
    char *get_secondarydns (void )
    {
    	return dut_secondary_dns;
    }
    char *get_interface(void)
    {
    	return _interface;
    }

    traffic_stream_t *get_traffic_stream_instance (int stream_index )
    {
    	return &stream_table[stream_index];
    }
    traffic_profile_t get_traffic_profile(int stream_index )
    {
        return  stream_table[stream_index].profile;
    }

    traffic_direction_t get_traffic_direction (int stream_index )
    {
    	return stream_table[stream_index].direction;
    }

    char *get_traffic_src_ipaddress ( int stream_index )
    {
    	return stream_table[stream_index].src_ipaddr;
    }

    uint16_t get_traffic_src_port ( int stream_index )
    {
    	return stream_table[stream_index].src_port;
    }

    char *get_traffic_dst_ipaddress ( int stream_index )
    {
    	return stream_table[stream_index].dest_ipaddr;
    }

    uint16_t get_traffic_dst_port ( int stream_index)
    {
    	return stream_table[stream_index].dest_port;
    }

    qos_access_category_t get_traffic_class ( int stream_index )
    {
    	return stream_table[stream_index].ac;
    }

    int get_traffic_payloadsize ( int stream_index )
    {
    	return stream_table[stream_index].payload_size;
    }

    int get_traffic_framerate ( int stream_index )
    {
    	return stream_table[stream_index].frame_rate;
    }

    int get_traffic_duration ( int stream_index )
    {
    	return stream_table[stream_index].duration;
    }

    int get_traffic_stoptime ( int stream_index )
    {
    	return stream_table[stream_index].stop_time;
    }
    uint32_t get_traffic_streamid ( int stream_idx )
    {
       	 return stream_table[stream_idx].stream_id;
    }

    int get_traffic_frames_sent( int stream_idx )
    {
    	return stream_table[stream_idx].frames_sent;
    }
    int get_traffic_frames_received (int stream_idx )
    {
    	return stream_table[stream_idx].frames_received;
    }
    int get_traffic_bytes_sent (int stream_idx )
    {
        return stream_table[stream_idx].bytes_sent;
    }
    int get_traffic_bytes_received (int stream_idx )
    {
        return stream_table[stream_idx].bytes_received;
    }
    int get_traffic_out_of_seq (int stream_idx )
    {
    	return stream_table[stream_idx].out_of_sequence_frames;
    }
    wiced_wep_key_t * get_wepkey (void )
    {
    	return &wepkey;
    }

	private:
	    nsapi_wifi_ap_t _ap;
	    char _interface[16];    /*   = TEST_INTERFACE */
	    char _passphrase[64];   /*   = TEST_PASSPHRASE_DEFAULT;  */
	    uint8_t _wepkey_buffer[64];/*   = HEX values of WEP */
	    char _sectype[16];      /*   = TEST_SECTYPE_DEFAULT;  */
	    char _encptype[16];     /*   = TEST_ENCPTYPE_DEFAULT; */
	    char _keymgmttype[16];  /*   = TEST_KEYMGMTTYPE_DEFAULT; */
	    char _pmf[32];          /*   = TEST_PMF_DEFAULT; */
	    char _ssid[33];         /*   = TEST_SSID_DEFAULT; */
	    char _bssid[18];        /*   = TEST_BSSID_DEFAULT; */
	    char _channel[4];       /*   = TEST_CHANNEL_DEFAULT; */
	    char _using_dhcp[4];    /*   = TEST_USING_DHCP_DEFAULT; */
	    char dut_ip_addr[16];       /*   = DUT_IP_ADDR_DEFAULT; */
	    char dut_netmask[16];       /*   = DUT_NETMASK_DEFAULT; */
	    char dut_gateway[16];       /*   = DUT_GATEWAY_DEFAULT; */
	    char dut_primary_dns[16];   /*   = DUT_PRIMARY_DNS_DEFAULT; */
	    char dut_secondary_dns[16]; /*   = DUT_SECONDARY_DNS_DEFAULT; */
	    wiced_wep_key_t wepkey;  /* WEP encryption key */
	    traffic_stream_t stream_table[NUM_STREAM_TABLE_ENTRIES];
};

whd_security_t nsapi_to_whdsecurity(nsapi_security_t sec);
nsapi_security_t get_authtype( char* encptype, char* keymgmttype, char* sectype );
void sigmadut_init ( void );
