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

#ifndef OFFSETOF
#ifdef __ARMCC_VERSION
/*
 * The ARM RVCT compiler complains when using OFFSETOF where a constant
 * expression is expected, such as an initializer for a static object.
 * offsetof from the runtime library doesn't have that problem.
 */
#include <stddef.h>
#define OFFSETOF(type, member)  offsetof(type, member)
#else
#  if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 8))
/* GCC 4.8+ complains when using our OFFSETOF macro in array length declarations. */
#    define OFFSETOF(type, member)  __builtin_offsetof(type, member)
#  else
#include <stdint.h>
#    define OFFSETOF(type, member)  ((uint)(uintptr_t)&((type *)0)->member)
#  endif /* GCC 4.8 or newer */
#endif /* __ARMCC_VERSION */
#endif /* OFFSETOF */

#define WLAN_IOCTL_BUF_LEN  (256)

#ifndef NBBY
#define NBBY    8   /* 8 bits per byte */
#endif

#ifndef isset
#define isset(a, i) (((const uint8 *)a)[(i) / NBBY] & (1 << ((i) % NBBY)))
#endif
#ifndef CEIL
#define CEIL(x, y)      (((x) + ((y) - 1)) / (y))
#endif
#ifndef ROUNDUP
#define ROUNDUP(x, y)       ((((x) + ((y) - 1)) / (y)) * (y))
#endif

/* 802.11h defines */
#define DOT11_MNG_EXT_CAP_ID            127 /* d11 mgmt ext capability */

/* Extended capabilities IE bitfields */
/* 20/40 BSS Coexistence Management support bit position */
#define DOT11_EXT_CAP_OBSS_COEX_MGMT        0
/* Civic Location */
#define DOT11_EXT_CAP_CIVIC_LOC         14
/* Geospatial Location */
#define DOT11_EXT_CAP_LCI           15
/* BSS Transition Management support bit position */
#define DOT11_EXT_CAP_BSSTRANS_MGMT     19
/* inter working support bit position */
#define DOT11_EXT_CAP_IW            31


/* Management Frame Information Element IDs */
#define DOT11_MNG_VS_ID             221 /* d11 management Vendor Specific IE */

#define HT_CAP_LDPC_CODING    0x0001    /* Support for rx of LDPC coded pkts */
#define HT_CAP_40MHZ          0x0002    /* FALSE:20Mhz, TRUE:20/40MHZ supported */
#define HT_CAP_SHORT_GI_20    0x0020    /* 20MHZ short guard interval support */
#define HT_CAP_SHORT_GI_40    0x0040    /* 40Mhz short guard interval support */

#define WL_CHANSPEC_BW_MASK     0x3800
#define WL_CHANSPEC_BW_SHIFT        11
#define WL_CHANSPEC_BW_5        0x0000
#define WL_CHANSPEC_BW_10       0x0800
#define WL_CHANSPEC_BW_20       0x1000
#define WL_CHANSPEC_BW_40       0x1800
#define WL_CHANSPEC_BW_80       0x2000
#define WL_CHANSPEC_BW_160      0x2800
#define WL_CHANSPEC_BW_8080     0x3000
#define WL_CHANSPEC_BW_2P5      0x3800

#define WL_CHANSPEC_BAND_MASK       0xc000
#define WL_CHANSPEC_BAND_SHIFT      14
#define WL_CHANSPEC_BAND_2G     0x0000
#define WL_CHANSPEC_BAND_3G     0x4000
#define WL_CHANSPEC_BAND_4G     0x8000
#define WL_CHANSPEC_BAND_5G     0xc000
#define INVCHANSPEC         255
#define MAX_CHANSPEC                0xFFFF
#define CHSPEC_CHANNEL(chspec)  ((uint8)((chspec) & 0x00ff))

/* channel defines */
#define CH_UPPER_SB         0x01
#define CH_LOWER_SB         0x02
#define CH_EWA_VALID            0x04
#define CH_80MHZ_APART          16
#define CH_40MHZ_APART          8
#define CH_20MHZ_APART          4
#define CH_10MHZ_APART          2
#define CH_5MHZ_APART           1   /* 2G band channels are 5 Mhz apart */
//#define CH_MAX_2G_CHANNEL       14  /* Max channel in 2G band */

#define WL_CHANSPEC_CHAN_MASK       0x00ff
#define WL_CHANSPEC_CHAN_SHIFT      0
#define WL_CHANSPEC_CHAN1_MASK      0x000f
#define WL_CHANSPEC_CHAN1_SHIFT     0
#define WL_CHANSPEC_CHAN2_MASK      0x00f0
#define WL_CHANSPEC_CHAN2_SHIFT     4
#define WL_CHANSPEC_CTL_SB_MASK     0x0700
#define WL_CHANSPEC_CTL_SB_SHIFT    8

#define CHSPEC_CHAN1(chspec)    ((chspec) & WL_CHANSPEC_CHAN1_MASK) >> WL_CHANSPEC_CHAN1_SHIFT
#define CHSPEC_CHAN2(chspec)    ((chspec) & WL_CHANSPEC_CHAN2_MASK) >> WL_CHANSPEC_CHAN2_SHIFT

#define CHSPEC_CTL_SB(chspec)   ((chspec) & WL_CHANSPEC_CTL_SB_MASK)
#define CHSPEC_IS20(chspec) (((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_20)
#define CHSPEC_IS20_5G(chspec)  ((((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_20) && \
                                CHSPEC_IS5G(chspec))
#ifndef CHSPEC_IS40
#define CHSPEC_IS40(chspec) (((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_40)
#endif
#ifndef CHSPEC_IS80
#define CHSPEC_IS80(chspec) (((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_80)
#endif
#ifndef CHSPEC_IS160
#define CHSPEC_IS160(chspec)    (((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_160)
#endif
#ifndef CHSPEC_IS8080
#define CHSPEC_IS8080(chspec)   (((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_8080)
#endif
#define CHSPEC_IS5G(chspec) (((chspec) & WL_CHANSPEC_BAND_MASK) == WL_CHANSPEC_BAND_5G)
#define CHSPEC_IS2G(chspec) (((chspec) & WL_CHANSPEC_BAND_MASK) == WL_CHANSPEC_BAND_2G)

#define WFA_OUI         "\x50\x6F\x9A"  /* WFA OUI */
#define WFA_OUI_LEN     3       /* WFA OUI length */
#define WFA_OUI_TYPE_OSEN   0x12

/*
 * VHT Capabilities IE (sec 8.4.2.160) defines
 */
#define VHT_CAP_MCS_MAP_0_7                     0
#define VHT_CAP_MCS_MAP_0_8                     1
#define VHT_CAP_MCS_MAP_0_9                     2
#define VHT_CAP_MCS_MAP_NONE                    3
#define VHT_CAP_MCS_MAP_S                       2 /* num bits for 1-stream */
#define VHT_CAP_MCS_MAP_M                       0x3 /* mask for 1-stream */

/* VHT rates bitmap */
#define VHT_CAP_MCS_0_7_RATEMAP     0x00ff
#define VHT_CAP_MCS_0_8_RATEMAP     0x01ff
#define VHT_CAP_MCS_0_9_RATEMAP     0x03ff
#define VHT_CAP_MCS_FULL_RATEMAP    VHT_CAP_MCS_0_9_RATEMAP

#define VHT_PROP_MCS_MAP_10_11                   0
#define VHT_PROP_MCS_MAP_UNUSED1                 1
#define VHT_PROP_MCS_MAP_UNUSED2                 2
#define VHT_PROP_MCS_MAP_NONE                    3
#define VHT_PROP_MCS_MAP_NONE_ALL                0xffff

#define VHT_CAP_MCS_MAP_NSS_MAX                 8

/* Def for rx & tx basic mcs maps - ea ss num has 2 bits of info */
#define VHT_MCS_MAP_GET_SS_IDX(nss) (((nss)-1) * VHT_CAP_MCS_MAP_S)
#define VHT_MCS_MAP_GET_MCS_PER_SS(nss, mcsMap) \
    (((mcsMap) >> VHT_MCS_MAP_GET_SS_IDX(nss)) & VHT_CAP_MCS_MAP_M)


/* TLV defines */
#define TLV_TAG_OFF     0   /* tag offset */
#define TLV_LEN_OFF     1   /* length offset */
#define TLV_HDR_LEN     2   /* header length */
#define TLV_BODY_OFF        2   /* body offset */
#define TLV_BODY_LEN_MAX    255 /* max body length */

/* tlv header - two bytes */
#define BCM_TLV_HDR_SIZE (OFFSETOF(cywifi_tlv_t, data))

/* Check that bcm_tlv_t fits into the given buffer len */
#define cywifi_valid_tlv(elt, buflen) (\
     ((int)(buflen) >= (int)BCM_TLV_HDR_SIZE) && \
     ((int)(buflen) >= (int)(BCM_TLV_HDR_SIZE + (elt)->len)))

/* Legacy structure to help keep backward compatible wl tool and tray app */
#define LEGACY_WL_BSS_INFO_VERSION  107

/*
 * Per-BSS information structure.
 */

#define LEGACY2_WL_BSS_INFO_VERSION 108     /**< old version of wl_bss_info struct */

/**
 * Structure type(aka id)/length/value buffer triple
 */
typedef struct cywifi_tlv {
    uint8_t   id;          /**< the id     */
    uint8_t   len;         /**< the length */
    uint8_t   data[1];     /**< the data   */
} cywifi_tlv_t;

/**
 * BSS info structure
 * Applications MUST CHECK ie_offset field and length field to access IEs and
 * next bss_info structure in a vector (in wl_scan_results_t)
 */
typedef struct wl_bss_info_109 {
    uint32_t      version;        /**< version field */
    uint32_t      length;         /**< byte length of data in this record,
                                  * starting at version and including IEs
                                  */
    whd_mac_t BSSID;                  /**< The BSSID of the AP */
    uint16_t      beacon_period;      /**< units are Kusec */
    uint16_t      capability;     /**< Capability information */
    uint8_t       SSID_len;       /**< The length of the SSID */
    uint8_t       SSID[32];       /**< The SSID of the AP */
    uint8_t       bcnflags;       /**< additional flags w.r.t. beacon */
    struct {
        uint32_t  count;          /**< # rates in this set */
        uint8_t   rates[16];      /**< rates in 500kbps units w/hi bit set if basic */
    } rateset;                    /**< supported rates */
    chanspec_t  chanspec;         /**< chanspec for bss */
    uint16_t      atim_window;    /**< units are Kusec */
    uint8_t       dtim_period;    /**< DTIM period */
    uint8_t       accessnet;      /**< from beacon interwork IE (if bcnflags) */
    int16_t       RSSI;           /**< receive signal strength (in dBm) */
    int8_t        phy_noise;      /**< noise (in dBm) */
    uint8_t       n_cap;          /**< BSS is 802.11N Capable */
    uint16_t      freespace1;     /**< make implicit padding explicit */
    uint32_t      nbss_cap;       /**< 802.11N+AC BSS Capabilities */
    uint8_t       ctl_ch;         /**< 802.11N BSS control channel number */
    uint8_t       padding1[3];    /**< explicit struct alignment padding */
    uint16_t      vht_rxmcsmap;   /**< VHT rx mcs map (802.11ac IE, VHT_CAP_MCS_MAP_*) */
    uint16_t      vht_txmcsmap;   /**< VHT tx mcs map (802.11ac IE, VHT_CAP_MCS_MAP_*) */
    uint8_t       flags;          /**< flags */
    uint8_t       vht_cap;        /**< BSS is vht capable */
    uint8_t       reserved[2];    /**< Reserved for expansion of BSS properties */
    uint8_t       basic_mcs[MCSSET_LEN];  /**< 802.11N BSS required MCS set */

    uint16_t      ie_offset;      /**< offset at which IEs start, from beginning */
    uint16_t      freespace2;     /**< making implicit padding explicit */
    uint32_t      ie_length;      /**< byte length of Information Elements */
    int16_t       SNR;            /**< average SNR of during frame reception */
    uint16_t      vht_mcsmap;     /**< STA's Associated vhtmcsmap */
    uint16_t      vht_mcsmap_prop;    /**< STA's Associated prop vhtmcsmap */
    uint16_t      vht_txmcsmap_prop;  /**< prop VHT tx mcs prop */
    /* Add new fields here */
    /* variable length Information Elements */
} wl_bss_info_109_t;


/** This function gets wl_bss_info_t structure
 *
 * @param   buffer     : buffer to get from WiFi
 * @return  cy_rslt_t  : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 */
cy_rslt_t cywifi_get_bss_info( uint8_t *buffer );



/** This function prints bss info
 * @param   buffer : The pointer wl_bss_info structure
 * @return  cy_rslt_t : CY_RSLT_SUCCESS
 *                    : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_dump_bssinfo(uint8_t  *buffer );

/** This function dumps supported rates
 * @param   rates : The pointer supported rates
 * @param   count : The number of supported rate sets
 * @return  cy_rslt_t : CY_RSLT_SUCCESS
 *                    : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_dump_rateset(uint8_t *rates, uint count);

/** This function dumps extended capabilities
 * @param   cp    : The pointer extended capabilities offset
 * @param   len   : The length of the IE
 * @return  cy_rslt_t : CY_RSLT_SUCCESS
 *                    : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_dump_ext_cap(uint8_t *cp, uint len);

/** This function dumps extended IE's
 * @param   ext_cap_ie : The pointer extended capabilities IE's
 * @return  cy_rslt_t  : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 *
 *******************************************************************************/
cy_rslt_t cywifi_ext_cap_ie_dump(cywifi_tlv_t *ext_cap_ie);

/** This function traverse a string of 1-byte tag/1-byte
 * length/variable-length value triples, returning a pointer
 * to the substring whose first element matches tag
 * @param   tlv_buf    : The pointer tlv buffer
 * @param   buflen     : The length of the buffer
 * @param   key        : The key to search for
 * @return  cy_rslt_t  : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 */
uint8_t* cywifi_parse_tlvs(uint8_t *tlv_buf, int buflen, uint key);


/** This function returns the primary 80 Mhz channel for the
 *  provided chanspec
 * @param   chanspec   : The chanspec of channel Input chanespec for which the 80Mhz primary channel has to be retrieved
 * @return             : -1 in case the provided channel is 20/40 Mhz chanspec
 *
 */
uint8_t cywifi_chspec_primary80_channel(chanspec_t chanspec);

/** This function returns returns the secondary 80 Mhz channel
 *  for the provided chanspec
 * @param   chanspec  : The chanspec of channel Input chanspec for which the 80MHz secondary channel has to be retrieved
 * @return            : -1 in case the provided channel is 20/40/80 Mhz chanspec
 */
uint8_t cywifi_chspec_secondary80_channel(chanspec_t chanspec);

/** This function returns returns channel list to 80Mhz given an channel
 * @param   chan_80Mhz_id   : The 80Mhz channel id
 * @return                  : returns an 80Mhz channel
 */
uint8_t cywifi_chspec_get80Mhz_ch(uint8_t chan_80Mhz_id);

/** This function prints vendor specific IE
 *
 * @param parse     : The pointer to IE to be parsed
 * @param len       : The length of the VS IE
 *
 *******************************************************************************/
void cywifi_print_vs_ie(uint8_t *parse, int len);

/** This function advances from the current 1-byte tag/1-byte length/variable-length value
 * triple, to the next, returning a pointer to the next.
 * If the current or next TLV is invalid (does not fit in given buffer length),
 * NULL is returned.
 * *buflen is not modified if the TLV elt parameter is invalid, or is decremented
 * by the TLV parameter's length if it is valid.
 *
 *  @param elt     : The pointer TLV
 *  @param buflen  : The pointer to the buffer length
 *  @return cywifi_tlv_t : The pointer to the cywifi_tlv_t structure
 */
cywifi_tlv_t * cywifi_next_tlv(cywifi_tlv_t *elt, int *buflen);

/** This function checks if vendor ie matches with type
 *  @param ie      : The pointer to IE
 *  @param oui     : The pointer to the oui
 *  @param oui_len : The oui length
 *  @param type    : The type to match
 *  @return        : true if IE match is found else false.
 */
bool cywifi_vs_ie_match(uint8_t *ie, uint8_t *oui, int oui_len, uint8_t type);

/** This function finds the vendor given an IE type
 * @param parse   : The pointer to IE to parsed
 * @param len     : The length of the IE
 * @param oui     : The pointer to the oui
 * @param oui_len : The length of the oui
 * @param oui_type: The type of oui
 * @return        : The pointer to the cywifi_tlv_t structure
 */
cywifi_tlv_t *cywifi_find_vs_ie(uint8_t *parse, int len,
                                uint8_t *oui, uint8_t oui_len, uint8_t oui_type);

/** This function scans the AP for H2E
 * @param wifi_ap      : The pointer to AP instance
 * @param count        : The count of the number of AP(s)
 * @return  cy_rslt_t  : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 */
cy_rslt_t cywifi_start_pt_scan( void *wifi_ap, uint32_t count);

/** This function sets the SSID of the H2E AP
 * @param ssid         : The pointer to ssid
 * @return  cy_rslt_t  : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 */
int cy_wpa3_set_ssid( char *ssid );

/** This function sets the passphrase of the H2E AP
 * @param passphrase   : The pointer to AP passphrase
 * @return  cy_rslt_t  : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 */
int cy_wpa3_set_ssid_passphrase (char *passphrase);

/** This function commits the Preferred
 *  network list and derives PT
 */
int cy_wpa3_commit_wp3_pfn_network ( void );

/** This function retrieves  the Preferred network list PT
 * @param ssid         : The pointer to SSID
 * @param passphrase   : The pointer to passphrase
 * @param pt           : The pointer to PT
 * @return cy_rslt_t   : CY_RSLT_SUCCESS
 *                     : CY_RSLT_TYPE_ERROR
 */
extern cy_rslt_t cy_wpa3_get_pfn_network( uint8_t * ssid, uint8_t *passphrase, uint8_t *pt );

/** This function  calucates the PT
 * @param ssid             : The pointer to SSID
 * @param ssid_len         : The length of SSID
 * @param passphrase       : The pointer to passphrase
 * @param passphrase_len   : The length of passphrase
 * @param output           : The pointer to output buffer
 * @param outlen           : The length of the output buffer
 * @return cy_rslt_t       : CY_RSLT_SUCCESS
 *                         : CY_RSLT_TYPE_ERROR
 */
extern cy_rslt_t wpa3_supplicant_h2e_pfn_list_derive_pt (uint8_t *ssid, uint8_t ssid_len, uint8_t *passphrase, uint8_t passphrase_len, uint8_t *output, uint8_t outlen );
