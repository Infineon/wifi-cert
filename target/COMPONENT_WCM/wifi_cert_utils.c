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
#include "cy_result.h"
#include "wifi_cert_sigma.h"
#include "wifi_cert_sigma_api.h"
#include "wifi_intf_api.h"
#include "whd_wifi_api.h"
#include "cy_nw_helper.h"
#include "wifi_cert_utils.h"

/* 80MHz channels in 5GHz band */
static const uint8_t wifi_5g_80m_chans[] =
{42, 58, 106, 122, 138, 155};
#define WF_NUM_5G_80M_CHANS \
    (sizeof(wifi_5g_80m_chans)/sizeof(uint8_t))

cy_rslt_t cywifi_dump_bssinfo(uint8_t * buffer )
{
    uint16_t capability;
    int i, mcs_idx = 0, start_idx = 0;
    bool start_idx_valid = false;
    wl_bss_info_t * bss_info = (wl_bss_info_t *)buffer;
    wl_bss_info_109_t *new_bssinfo = (wl_bss_info_109_t *)buffer;

    printf("\nSSID: %s", new_bssinfo->SSID);
    if ( bss_info->version > LEGACY2_WL_BSS_INFO_VERSION )
    {
        capability = new_bssinfo->capability;
        capability &= (DOT11_CAP_ESS | DOT11_CAP_IBSS);

        if (capability == DOT11_CAP_ESS)
        {
            printf("\nMode: %s", "Managed ");
        }
        else if (capability == DOT11_CAP_IBSS)
        {
            printf("\nMode: %s", "Ad Hoc ");
        }
        else
        {
           printf("\nMode: %s", "<unknown> ");
        }

        printf("RSSI: :%d dBm SNR: %d dB noise: %d dBm Flags: %d",
                new_bssinfo->RSSI, new_bssinfo->SNR, new_bssinfo->phy_noise, new_bssinfo->flags);

        printf("\nBSSID:%02X:%02X:%02X:%02X:%02X:%02X", new_bssinfo->BSSID.octet[0], new_bssinfo->BSSID.octet[1],
                    new_bssinfo->BSSID.octet[2], new_bssinfo->BSSID.octet[3], new_bssinfo->BSSID.octet[4], new_bssinfo->BSSID.octet[5]);
	
        printf(" Capability: ");
        if (new_bssinfo->capability & DOT11_CAP_ESS)
              printf("%s", "ESS ");
        if (new_bssinfo->capability & DOT11_CAP_IBSS)
              printf("%s", "IBSS ");

        printf("\nSupported Rates: ");
        cywifi_dump_rateset(new_bssinfo->rateset.rates, dtoh32(new_bssinfo->rateset.count));
        printf("\n");

        printf("Extended Capabilities: ");
        if (dtoh32(new_bssinfo->ie_length))
        {
            cywifi_dump_ext_cap((uint8_t *)(((uint8_t *)new_bssinfo) + dtoh16(new_bssinfo->ie_offset)),
                                dtoh32(new_bssinfo->ie_length));
        }

        printf("\n");
        if (new_bssinfo->vht_cap)
        {
            printf("VHT Capable:\n");
        }
        else
        {
            printf("HT Capable:\n");
        }
        if (CHSPEC_IS8080(new_bssinfo->chanspec))
        {
            printf("Chanspec: 5GHz channel %d-%d 80+80MHz (0x%x)\n",
                   cywifi_chspec_primary80_channel(new_bssinfo->chanspec),
                   cywifi_chspec_secondary80_channel(new_bssinfo->chanspec),
                   new_bssinfo->chanspec);
        }
        else
        {
            printf("Chanspec: %sGHz channel %d %dMHz (0x%x)\n",
                   CHSPEC_IS2G(new_bssinfo->chanspec)?"2.4":"5", CHSPEC_CHANNEL(new_bssinfo->chanspec),
                   (CHSPEC_IS160(new_bssinfo->chanspec) ?
                   160:(CHSPEC_IS80(new_bssinfo->chanspec) ?
                   80 :(CHSPEC_IS40(new_bssinfo->chanspec) ?
                   40 :(CHSPEC_IS20(new_bssinfo->chanspec) ? 20 : 10)))),
                   new_bssinfo->chanspec);
        }
        printf("Primary channel: %d\n", new_bssinfo->ctl_ch);
        printf("HT Capabilities: ");
        if (dtoh32(new_bssinfo->nbss_cap) & HT_CAP_40MHZ)
        {
            printf("40Mhz ");
        }
        if (dtoh32(new_bssinfo->nbss_cap) & HT_CAP_SHORT_GI_20)
        {
            printf("SGI20 ");
        }
        if (dtoh32(new_bssinfo->nbss_cap) & HT_CAP_SHORT_GI_40)
        {
            printf("SGI40 ");
        }
        printf("\n");
        printf("Supported HT MCS:  ");
        for (mcs_idx = 0; mcs_idx < (MCSSET_LEN * 8); mcs_idx++)
        {
            if (isset(new_bssinfo->basic_mcs, mcs_idx) && !start_idx_valid)
            {
                printf(" %d", mcs_idx);
                start_idx = mcs_idx;
                start_idx_valid = true;
            }

            if (!isset(new_bssinfo->basic_mcs, mcs_idx) && start_idx_valid)
            {
                if ((mcs_idx - start_idx) > 1)
                {
                    printf("-%d", (mcs_idx - 1));
                }
                start_idx_valid = false;
            }
        }
        printf("\n");
        printf("Negotiated VHT MCS: \n");
        if (new_bssinfo->vht_cap)
        {
            uint mcs, rx_mcs, prop_mcs = VHT_PROP_MCS_MAP_NONE;
            char *mcs_str, *rx_mcs_str;

            if (new_bssinfo->vht_mcsmap)
            {
                for (i = 1; i <= VHT_CAP_MCS_MAP_NSS_MAX; i++)
                {
                    mcs = VHT_MCS_MAP_GET_MCS_PER_SS(i, dtoh16(new_bssinfo->vht_mcsmap));

                    /* roundup to be in sync with driver
                     * wlc_bss2wl_bss().
                     */
                    if (dtoh16(new_bssinfo->length) >= (OFFSETOF(wl_bss_info_109_t,
                                vht_mcsmap_prop) +
                                ROUNDUP(dtoh32(new_bssinfo->ie_length), 4) +
                                sizeof(uint16_t)))
                    {
                        prop_mcs = VHT_MCS_MAP_GET_MCS_PER_SS(i,
                        dtoh16(new_bssinfo->vht_mcsmap_prop));
                    }
                    mcs_str =
                             (mcs == VHT_CAP_MCS_MAP_0_9 ? "0-9 " :
                             (mcs == VHT_CAP_MCS_MAP_0_8 ? "0-8 " :
                             (mcs == VHT_CAP_MCS_MAP_0_7 ? "0-7 " :
                              " -- ")));
                    if (prop_mcs != VHT_PROP_MCS_MAP_NONE)
                    {
                        mcs_str =
                                 (mcs == VHT_CAP_MCS_MAP_0_9 ? "0-11      " :
                                 (mcs == VHT_CAP_MCS_MAP_0_8 ? "0-8, 10-11" :
                                 (mcs == VHT_CAP_MCS_MAP_0_7 ? "0-7, 10-11" :
                                  "    --    ")));
                    }

                    if (mcs != VHT_CAP_MCS_MAP_NONE)
                    {
                        printf("NSS%d : %s \n", i,
                               mcs_str);
                    }
                }
            }
            else
            {
                printf("\tSupported VHT MCS:\n");
                for (i = 1; i <= VHT_CAP_MCS_MAP_NSS_MAX; i++)
                {
                    mcs = VHT_MCS_MAP_GET_MCS_PER_SS(i,
                                        dtoh16(new_bssinfo->vht_txmcsmap));

                    rx_mcs = VHT_MCS_MAP_GET_MCS_PER_SS(i,
                                        dtoh16(new_bssinfo->vht_rxmcsmap));

                    /* roundup to be in sync with driver
                     * wlc_bss2wl_bss().
                     */
                    if (dtoh16(new_bssinfo->length) >= (OFFSETOF(wl_bss_info_109_t, vht_txmcsmap_prop) +
                                ROUNDUP(dtoh32(new_bssinfo->ie_length), 4) +
                                sizeof(uint16_t)))
                    {
                         prop_mcs = VHT_MCS_MAP_GET_MCS_PER_SS(i,
                         dtoh16(new_bssinfo->vht_txmcsmap_prop));
                    }

                    mcs_str =
                             (mcs == VHT_CAP_MCS_MAP_0_9 ? "0-9 " :
                             (mcs == VHT_CAP_MCS_MAP_0_8 ? "0-8 " :
                             (mcs == VHT_CAP_MCS_MAP_0_7 ? "0-7 " : " -- ")));
                    if (prop_mcs != VHT_PROP_MCS_MAP_NONE)
                    {
                         mcs_str =
                                (mcs == VHT_CAP_MCS_MAP_0_9 ? "0-11      " :
                                (mcs == VHT_CAP_MCS_MAP_0_8 ? "0-8, 10-11" :
                                (mcs == VHT_CAP_MCS_MAP_0_7 ? "0-7, 10-11" :
                                 "    --    ")));
                    }

                    rx_mcs_str =
                               (rx_mcs == VHT_CAP_MCS_MAP_0_9 ? "0-9 " :
                               (rx_mcs == VHT_CAP_MCS_MAP_0_8 ? "0-8 " :
                               (rx_mcs == VHT_CAP_MCS_MAP_0_7 ? "0-7 " : " -- ")));

                    if (prop_mcs != VHT_PROP_MCS_MAP_NONE)
                    {
                         rx_mcs_str =
                                (rx_mcs == VHT_CAP_MCS_MAP_0_9 ? "0-11      " :
                                (rx_mcs == VHT_CAP_MCS_MAP_0_8 ? "0-8, 10-11" :
                                (rx_mcs == VHT_CAP_MCS_MAP_0_7 ? "0-7, 10-11" :
                                 "    --    ")));
                    }

                    if ((mcs != VHT_CAP_MCS_MAP_NONE) ||
                                (rx_mcs != VHT_CAP_MCS_MAP_NONE))
                    {
                        printf("\t\tNSS%d Tx: %s  Rx: %s\n", i,
                                mcs_str, rx_mcs_str);
                    }
               }
            }
        }

        if (cywifi_find_vs_ie((uint8_t *)(((uint8_t *)new_bssinfo) + dtoh16(new_bssinfo->ie_offset)),
                dtoh32(new_bssinfo->ie_length),
                (uint8_t *)WFA_OUI, WFA_OUI_LEN, WFA_OUI_TYPE_OSEN) != NULL)
        {
                printf("OSEN supported\n");
        }

        cywifi_print_vs_ie((uint8_t *)(((uint8_t *)new_bssinfo) + dtoh16(new_bssinfo->ie_offset)),
                dtoh32(new_bssinfo->ie_length));
    }
    return CY_RSLT_SUCCESS;
}

cywifi_tlv_t *cywifi_find_vs_ie(uint8_t *parse, int len,
    uint8_t *oui, uint8_t oui_len, uint8_t oui_type)
{
    cywifi_tlv_t *ie;

    while ((ie = (cywifi_tlv_t *)cywifi_parse_tlvs(parse, (int)len, DOT11_MNG_VS_ID))) {
        if (cywifi_vs_ie_match((uint8_t *)ie, oui, oui_len, oui_type))
            return ie;
        if ((ie = cywifi_next_tlv(ie, &len)) == NULL)
            break;
    }
    return NULL;
}

/* vendor specific TLV match */
bool cywifi_vs_ie_match(uint8_t *ie, uint8_t *oui, int oui_len, uint8_t type)
{
    /* If the contents match the OUI and the type */
    if (ie[TLV_LEN_OFF] >= oui_len + 1 &&
        !memcmp(&ie[TLV_BODY_OFF], oui, oui_len) &&
        type == ie[TLV_BODY_OFF + oui_len]) {
        return true;
    }

    return false;
}

cy_rslt_t cywifi_dump_rateset(uint8_t *rates, uint count)
{
    uint i;
    uint r;
    bool b;

    printf("[ ");
    for (i = 0; i < count; i++) {
        r = rates[i] & 0x7f;
        b = rates[i] & 0x80;
        if (r == 0)
            break;
        printf("%d%s%s ", (r / 2), (r % 2)?".5":"", b?"(b)":"");
    }
    printf("]");

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_dump_ext_cap(uint8_t* cp, uint len)
{
    uint8_t *parse = cp;
    uint parse_len = len;
    uint8_t *ext_cap_ie;

    if ((ext_cap_ie = cywifi_parse_tlvs(parse, parse_len, DOT11_MNG_EXT_CAP_ID))) {
        cywifi_ext_cap_ie_dump((cywifi_tlv_t*)ext_cap_ie);
    } else
        printf("Extended Capabilities: Not_Available\n");

    return CY_RSLT_SUCCESS;
}

cy_rslt_t cywifi_ext_cap_ie_dump(cywifi_tlv_t* ext_cap_ie)
{
   if (ext_cap_ie->len >= CEIL(DOT11_EXT_CAP_IW, NBBY)) {
        /* check IW bit */
        if (isset(ext_cap_ie->data, DOT11_EXT_CAP_IW))
            printf("IW ");
    }

    if (ext_cap_ie->len >= CEIL(DOT11_EXT_CAP_CIVIC_LOC, NBBY)) {
        /* check Civic Location bit */
        if (isset(ext_cap_ie->data, DOT11_EXT_CAP_CIVIC_LOC))
            printf("Civic_Location ");
    }

    if (ext_cap_ie->len >= CEIL(DOT11_EXT_CAP_LCI, NBBY)) {
        /* check Geospatial Location bit */
        if (isset(ext_cap_ie->data, DOT11_EXT_CAP_LCI))
            printf("Geospatial_Location ");
    }

    if (ext_cap_ie->len > 0) {
        /* check 20/40 BSS Coexistence Management support bit */
        if (isset(ext_cap_ie->data, DOT11_EXT_CAP_OBSS_COEX_MGMT))
            printf("20/40_Bss_Coexist ");
    }

    if (ext_cap_ie->len >= CEIL(DOT11_EXT_CAP_BSSTRANS_MGMT, NBBY)) {
        /* check BSS Transition Management support bit */
        if (isset(ext_cap_ie->data, DOT11_EXT_CAP_BSSTRANS_MGMT))
            printf("BSS_Transition");
    }
    return CY_RSLT_SUCCESS;
}

uint8_t* cywifi_parse_tlvs(uint8_t *tlv_buf, int buflen, uint key)
{
    uint8_t *cp;
    int totlen;
    cp = tlv_buf;
    totlen = buflen;

    /* find tagged parameter */
    while (totlen >= 2) {
        uint tag;
        int len;

        tag = *cp;
        len = *(cp +1);

        /* validate remaining totlen */
        if ((tag == key) && (totlen >= (len + 2)))
            return (cp);

        cp += (len + 2);
        totlen -= (len + 2);
    }

    return NULL;
}

/*
 * Returns the primary 80 Mhz channel for the provided chanspec
 *
 *    chanspec - Input chanspec for which the 80MHz primary channel has to be retrieved
 *
 *  returns -1 in case the provided channel is 20/40 Mhz chanspec
 */

uint8_t cywifi_chspec_primary80_channel(chanspec_t chanspec)
{
    uint8_t primary80_chan;

    if (CHSPEC_IS80(chanspec))
    {
        primary80_chan = CHSPEC_CHANNEL(chanspec);
    }
    else if (CHSPEC_IS8080(chanspec))
    {
        /* Channel ID 1 corresponds to frequency segment 0, the primary 80 MHz segment */
        primary80_chan = cywifi_chspec_get80Mhz_ch(CHSPEC_CHAN1(chanspec));
    }
    else if (CHSPEC_IS160(chanspec))
    {
        uint8_t center_chan = CHSPEC_CHANNEL(chanspec);
        uint sb = CHSPEC_CTL_SB(chanspec) >> WL_CHANSPEC_CTL_SB_SHIFT;

        /* based on the sb value primary 80 channel can be retrieved
         * if sb is in range 0 to 3 the lower band is the 80Mhz primary band
         */
        if (sb < 4)
        {
            primary80_chan = center_chan - CH_40MHZ_APART;
        }
        /* if sb is in range 4 to 7 the upper band is the 80Mhz primary band */
        else
        {
            primary80_chan = center_chan + CH_40MHZ_APART;
        }
    }
    else
    {
        /* for 20 and 40 Mhz */
        primary80_chan = -1;
    }
    return primary80_chan;
}

uint8_t cywifi_chspec_secondary80_channel(chanspec_t chanspec)
{
    uint8_t secondary80_chan;

    if (CHSPEC_IS8080(chanspec))
    {
        secondary80_chan = cywifi_chspec_get80Mhz_ch(CHSPEC_CHAN2(chanspec));
    }
    else if (CHSPEC_IS160(chanspec))
    {
        uint8_t center_chan = CHSPEC_CHANNEL(chanspec);
        uint sb = CHSPEC_CTL_SB(chanspec) >> WL_CHANSPEC_CTL_SB_SHIFT;

        /* based on the sb value  secondary 80 channel can be retrieved
         * if sb is in range 0 to 3 upper band is the secondary 80Mhz band
         */
        if (sb < 4)
        {
            secondary80_chan = center_chan + CH_40MHZ_APART;
        }
        /* if sb is in range 4 to 7 the lower band is the secondary 80Mhz band */
        else
        {
            secondary80_chan = center_chan - CH_40MHZ_APART;
        }
    }
    else
    {
        /* for 20, 40, and 80 Mhz */
        secondary80_chan = -1;
    }
    return secondary80_chan;
}

uint8_t cywifi_chspec_get80Mhz_ch(uint8_t chan_80Mhz_id)
{
    if (chan_80Mhz_id < WF_NUM_5G_80M_CHANS)
    {
        return wifi_5g_80m_chans[chan_80Mhz_id];
    }
    return 0;
}

void cywifi_print_vs_ie(uint8_t *parse, int len)
{
    cywifi_tlv_t *ie;
    while ((ie = (cywifi_tlv_t *)cywifi_parse_tlvs(parse, (int)len, DOT11_MNG_VS_ID)))
    {
        int len_tmp = 0;
        printf("VS_IE:");
        printf("%02x%02x", ie->id, ie->len);
        while (len_tmp < ie->len)
        {
            printf("%02x", ie->data[len_tmp]);
            len_tmp++;
        }
        printf("\n");

        if ((parse = (uint8_t *)cywifi_next_tlv(ie, &len)) == NULL)
            break;
    }
}

/*
 * Advance from the current 1-byte tag/1-byte length/variable-length value
 * triple, to the next, returning a pointer to the next.
 * If the current or next TLV is invalid (does not fit in given buffer length),
 * NULL is returned.
 * *buflen is not modified if the TLV elt parameter is invalid, or is decremented
 * by the TLV parameter's length if it is valid.
 */
cywifi_tlv_t * cywifi_next_tlv(cywifi_tlv_t *elt, int *buflen)
{
    int len;

    /* validate current elt */
    if (!cywifi_valid_tlv(elt, *buflen)) {
        return NULL;
    }

    /* advance to next elt */
    len = elt->len;
    elt = (cywifi_tlv_t*)(elt->data + len);
    *buflen -= (TLV_HDR_LEN + len);

    /* validate next elt */
    if (!cywifi_valid_tlv(elt, *buflen)) {
        return NULL;
    }
    return elt;
}

void cywifi_ether_aton(const char *mac, whd_mac_t *output)
{
    char *c = (char *) mac;
    int i = 0;

    memset(output->octet, 0, sizeof(whd_mac_t));
    for (;;) {
        output->octet[i++] = (unsigned char) strtoul(c, &c, 16);
        if (!*c++ || i == sizeof(whd_mac_t))
            break;
    }
}

#endif /* WIFICERT_NO_HARDWARE */
