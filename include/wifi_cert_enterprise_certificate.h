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

/** Application interface header file */
#include "cy_result.h"


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_WFA_ENT_ROOT_CERT_STRING \
        "-----BEGIN CERTIFICATE-----\r\n" \
        "MIID+TCCAuGgAwIBAgIJANqqHCazDkkOMA0GCSqGSIb3DQEBCwUAMIGSMQswCQYD\r\n" \
        "VQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTEUMBIGA1UEBwwLU2FudGEgQ2xh\r\n" \
        "cmExFzAVBgNVBAoMDldpLUZpIEFsbGlhbmNlMR0wGwYDVQQDDBRXRkEgUm9vdCBD\r\n" \
        "ZXJ0aWZpY2F0ZTEgMB4GCSqGSIb3DQEJARYRc3VwcG9ydEB3aS1maS5vcmcwHhcN\r\n" \
        "MTMwMzExMTkwMjI2WhcNMjMwMzA5MTkwMjI2WjCBkjELMAkGA1UEBhMCVVMxEzAR\r\n" \
        "BgNVBAgMCkNhbGlmb3JuaWExFDASBgNVBAcMC1NhbnRhIENsYXJhMRcwFQYDVQQK\r\n" \
        "DA5XaS1GaSBBbGxpYW5jZTEdMBsGA1UEAwwUV0ZBIFJvb3QgQ2VydGlmaWNhdGUx\r\n" \
        "IDAeBgkqhkiG9w0BCQEWEXN1cHBvcnRAd2ktZmkub3JnMIIBIjANBgkqhkiG9w0B\r\n" \
        "AQEFAAOCAQ8AMIIBCgKCAQEA6TOCu20m+9zLZITYAhGmtxwyJQ/1xytXSQJYX8LN\r\n" \
        "YUS/N3HG2QAQ4GKDh7DPDI13zhdc0yOUE1CIOXa1ETKbHIU9xABrL7KfX2HCQ1nC\r\n" \
        "PqRPiW9/wgQch8Aw7g/0rXmg1zewPJ36zKnq5/5Q1uyd8YfaXBzhxm1IYlwTKMlC\r\n" \
        "ixDFcAeVqHb74mAcdel1lxdagHvaL56fpUExm7GyMGXYd+Q2vYa/o1UwCMGfMOj6\r\n" \
        "FLHwKpy62KCoK3016HlWUlbpg8YGpLDt2BB4LzxmPfyH2x+Xj75mAcllOxx7GK0r\r\n" \
        "cGPpINRsr4vgoltm4Bh1eIW57h+gXoFfHCJLMG66uhU/2QIDAQABo1AwTjAdBgNV\r\n" \
        "HQ4EFgQUCwPCPlSiKL0+Sd5y8V+Oqw6XZ4IwHwYDVR0jBBgwFoAUCwPCPlSiKL0+\r\n" \
        "Sd5y8V+Oqw6XZ4IwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAsNxO\r\n" \
        "z9DXb7TkNFKtPOY/7lZig4Ztdu6Lgf6qEUOvJGW/Bw1WxlPMjpPk9oI+JdR8ZZ4B\r\n" \
        "9QhE+LZhg6SJbjK+VJqUcTvnXWdg0e8CgeUw718GNZithIElWYK3Kh1cSo3sJt0P\r\n" \
        "z9CiJfjwtBDwsdAqC9zV9tgp09QkEkav84X20VxaITa3H1QuK/LWSn/ORrzcX0Il\r\n" \
        "10YoF6Hz3ZWa65mUoMzd8DYtCyGtcbYrSt+NMCqRB186PDQn5XBCytgF8VuiCyyk\r\n" \
        "Z04hqHLzAFc21P9yhwKGi3BHD/Sep8fvr9y4VpMIqHQm2jaFPxY1VxhPSV+UHoE1\r\n" \
        "fCPitIJTp/iXi7uXTQ==\r\n" \
        "-----END CERTIFICATE-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_CLIENT_PRIVATE_KEY \
        "-----BEGIN RSA PRIVATE KEY-----\r\n" \
        "MIIEpQIBAAKCAQEA3KO5EOFtm/3wcNmYEgF1VgQpiVtMmsfCuvNpEYh5QdWieSJv\r\n" \
        "K0xJLWZTw0FYaDt1K/iI/WPLpA9x6gjGveU9Wty8vZYQyDBP1UakYGURmvxQv45I\r\n" \
        "ivbvUoCFz2aiZNbPyVRu2u3XgvAbyoqiBYV6B5dDeJyccFQPJGoOPHV2608azh9u\r\n" \
        "gvasFPOYkv3NaMxyTJqtOdlj0kGSCEqvPlZsZQm218UO5FNqGZMQ6lt4TCNzj0vN\r\n" \
        "LPKuLTM7orb8xTtCbwB4IeCBchO8oJyBO/pTPX9xMMxAsPZxAXS+wL352C4ZSBCP\r\n" \
        "EvMGU1KZ3fFwOULO0GuKyzbqiNu92SFiS4fb/wIDAQABAoIBAQDcnbCc2mt5AM98\r\n" \
        "Z3aQ+nhSy9Kkj2/njDqAKIc0ituEIpNUwEOcbaj2Bk1W/W3iuyEMGHURuMmUgAUN\r\n" \
        "WD0w/5j705+9ieG56eTJgts1r5mM+SHch+6tVQAz5GLn4N4cKlaWHyDBM/S77k47\r\n" \
        "lacwEijUkkFaxm3+O27woEMf3OxNl24KmRenMYBhqcsoT4BYBw3Bh8xe+XN95rXj\r\n" \
        "2BdIbr5+RWGc9Zsz4o5Wmd4mL/JvbKeohrsecien4TZRzWFku93XV5kie1c1aJy1\r\n" \
        "nJ85bGJk4focmP/2ToxQysTbPYCxHVTIHuADK/qf9SGHJ9F7EBHE7+0isuwBbqOD\r\n" \
        "OzS8rHdRAoGBAPCXlaHumEkLIRv3enhpHPBYxnDndNCtT1T6+Cuit/vfo6K6oA7p\r\n" \
        "iUaej/GPZsDKXhayeTiEaq7QMinUtGkiCgGlVtXghXuCZz6KrH19W6wzC6Pbokmq\r\n" \
        "BZak4LQcvGavt3VzjliAKLcdn6nQt/+bp/jKDJOKVbvb30sjS035Ah4zAoGBAOrF\r\n" \
        "BgE9UTEnfQHIh7pyiM1DAomBbdrlRos8maQl26cHqUHN3+wy1bGHLzOjYFFoAasx\r\n" \
        "eizw7Gudgbae28WIP1yLGrpt15cqVAvlCYmBtZ3C98FuT3FYgEEZpWNmE8Om+5UM\r\n" \
        "td+mtMjonWAPkCYC+alqUZzeIs+CZs5CHKYCDqcFAoGBAOfkQv38GV2102jARJPQ\r\n" \
        "RGtINaRXApmrod43s4Fjac/kAzVyiZk18PFXHUhnvlMt+jgIN5yIzMbHtsHo2SbH\r\n" \
        "/zsM4MBuklm0G80FHjIp5HT6EksSA77amF5VdptDYzfaP4p+IYIdrKCqddzYZrCA\r\n" \
        "mArMvAhs+iuCRhuG3is+SZNPAoGAHs6r8w2w0dp0tP8zkGvnN8hLVO//EnJzx2G0\r\n" \
        "Z63wHQMMWu5BLCWflSRANW6C/SvAzE450hvralPI6cX+4PT4G5TFdSFk4RlU3hq4\r\n" \
        "Has/wewLxv5Kvnz2l5Rd96U1gr8u1GhOlYKyxop/3FMuf050pJ6nBwa/WquqAfb6\r\n" \
        "+23ZrmECgYEA6l0GFHwMFBNnpPuxHgYgS5+4g3+8DhZZIDc7IflBCBWF/ZwbM+nH\r\n" \
        "+JSxiYYjvD7zIBhndqERcZ+fvbZTQ8oymr3j5AESM0ZfAHbft6IFQWjDUC3IDUF/\r\n" \
        "4F0cUidFC8smu6Wa2tjvSIz7DfvmDsn1l+7s9qQvDxdyPas0IkL/v8w=\r\n" \
        "-----END RSA PRIVATE KEY-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_CLIENT_CERT_STRING \
        "-----BEGIN CERTIFICATE-----\r\n" \
        "MIIEBTCCAu2gAwIBAgICEEYwDQYJKoZIhvcNAQELBQAwgZIxCzAJBgNVBAYTAlVT\r\n" \
        "MRMwEQYDVQQIDApDYWxpZm9ybmlhMRQwEgYDVQQHDAtTYW50YSBDbGFyYTEXMBUG\r\n" \
        "A1UECgwOV2ktRmkgQWxsaWFuY2UxHTAbBgNVBAMMFFdGQSBSb290IENlcnRpZmlj\r\n" \
        "YXRlMSAwHgYJKoZIhvcNAQkBFhFzdXBwb3J0QHdpLWZpLm9yZzAeFw0xMzA1MTAy\r\n" \
        "MzQ0NTFaFw0yMzA1MDgyMzQ0NTFaMH4xCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApD\r\n" \
        "YWxpZm9ybmlhMRcwFQYDVQQKDA5XaS1GaSBBbGxpYW5jZTEfMB0GA1UEAwwWQ2xp\r\n" \
        "ZW50IENlcnRpZmljYXRlIElETDEgMB4GCSqGSIb3DQEJARYRc3VwcG9ydEB3aS1m\r\n" \
        "aS5vcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDco7kQ4W2b/fBw\r\n" \
        "2ZgSAXVWBCmJW0yax8K682kRiHlB1aJ5Im8rTEktZlPDQVhoO3Ur+Ij9Y8ukD3Hq\r\n" \
        "CMa95T1a3Ly9lhDIME/VRqRgZRGa/FC/jkiK9u9SgIXPZqJk1s/JVG7a7deC8BvK\r\n" \
        "iqIFhXoHl0N4nJxwVA8kag48dXbrTxrOH26C9qwU85iS/c1ozHJMmq052WPSQZII\r\n" \
        "Sq8+VmxlCbbXxQ7kU2oZkxDqW3hMI3OPS80s8q4tMzuitvzFO0JvAHgh4IFyE7yg\r\n" \
        "nIE7+lM9f3EwzECw9nEBdL7AvfnYLhlIEI8S8wZTUpnd8XA5Qs7Qa4rLNuqI273Z\r\n" \
        "IWJLh9v/AgMBAAGjeDB2MA8GA1UdEwEB/wQFMAMCAQAwCwYDVR0PBAQDAgXgMBYG\r\n" \
        "A1UdJQEB/wQMMAoGCCsGAQUFBwMCMB0GA1UdDgQWBBQtC2mx8POhZRfB+sj4wX1Z\r\n" \
        "OzdrCzAfBgNVHSMEGDAWgBQLA8I+VKIovT5J3nLxX46rDpdngjANBgkqhkiG9w0B\r\n" \
        "AQsFAAOCAQEAsvHJ+J2YTCsEA69vjSmsGoJ2iEXDuHI+57jIo+8qRVK/m1is1eiU\r\n" \
        "AefExDtxxKTEPPtourlYO8A4i7oep9T43Be8nwVZdmxzfu14cdLKQrE+viCuHQTc\r\n" \
        "BLSoAv6/SZa3MeIRkkDSPtCPTJ/Pp+VYPK8gPlc9pwEs/KLgFxK/Sq6RDNjTPs6J\r\n" \
        "MChxi1iSdUES8mz3JDhQ2RQWVuibPorhgVqNTyXBjFUbYxTVl3hBCtK/Bd4IyFTX\r\n" \
        "Li90XXHseNbj2sHu3PFBU7PG5mhKQMUOYqvQzEDIXT6SDA+PrepqrwXn/BL861K6\r\n" \
        "GV4LcfKBg0HHdW9gJByZCN02igFTw56Kzg==\r\n" \
        "-----END CERTIFICATE-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_ROOT_CERT_STRING_MSFT \
        "-----BEGIN CERTIFICATE-----\r\n" \
        "MIIDoTCCAomgAwIBAgIQdf8o8nE9q5xOJKhfe8QoRzANBgkqhkiG9w0BAQsFADBj\r\n" \
        "MRMwEQYKCZImiZPyLGQBGRYDY29tMRYwFAYKCZImiZPyLGQBGRYGc2VydmVyMRMw\r\n" \
        "EQYKCZImiZPyLGQBGRYDaWFzMR8wHQYDVQQDExZpYXMtV0lOLU9OMUdCSDhMREcw\r\n" \
        "LUNBMB4XDTIxMDQyMDEyMjY1MVoXDTI2MDQyMDEyMzY1MFowYzETMBEGCgmSJomT\r\n" \
        "8ixkARkWA2NvbTEWMBQGCgmSJomT8ixkARkWBnNlcnZlcjETMBEGCgmSJomT8ixk\r\n" \
        "ARkWA2lhczEfMB0GA1UEAxMWaWFzLVdJTi1PTjFHQkg4TERHMC1DQTCCASIwDQYJ\r\n" \
        "KoZIhvcNAQEBBQADggEPADCCAQoCggEBAKD4qiELHV4ilebhSxpnqGvixyqWvH3w\r\n" \
        "XOuHbFQytxs7HF6ciZgTkqIkk+GtnzIBdtAvM+FU812HuS1uQTrK3keDVxyR6yah\r\n" \
        "RgXUYXdRqKi+S8dMGlK85y2mXq1lcbBxrhsaFNwKiXag85TisWR8GzznMBVUzjoh\r\n" \
        "+6RjH3nR+MctPGIf7xCrCrraHld0+HyKVsx/mGY4kSYkhyTS/2SjK8W8u3WfG1Sd\r\n" \
        "mGPssHs/FAWL0aLDd/kGO01fyrqKh3MOHA9z1/whYWiznj72uhmlUheQN3YnkcW7\r\n" \
        "zKFRE0duD9mOP7df8hMe2atQkBZbxN/5KIyGW2eUSFoyvI9yhT7GswkCAwEAAaNR\r\n" \
        "ME8wCwYDVR0PBAQDAgGGMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFNpFcD+f\r\n" \
        "hOjh2/l/5QaFcTPduUV5MBAGCSsGAQQBgjcVAQQDAgEAMA0GCSqGSIb3DQEBCwUA\r\n" \
        "A4IBAQBiqLiJL8dSJauOVVQNiyIkuS6wpNTAz+L4Q1I7KvU36KXJrHJp8mopR7u5\r\n" \
        "3qPv4oxkKBwCFCbLBtB/nM9SOhsKA5pGok37Wsr20ATl1R8Hcp1MWofEd4knhPjt\r\n" \
        "aV/4d2Ym960kIIPSIuQnRJhd7QM5HmGGbazRnbz78CGGYBfq5qVLsWravdozL0rm\r\n" \
        "mA7RZf4nK6KnOYqoGWNqCUvbWCcoFH4rnwcnEcIO/DTnl7oaPkp7Vyd+f0i+hp6A\r\n" \
        "A6VzKYMsq1Q/BDIGjV6QZjedkF1Hw8sONgMGPhYIQH2sshN+cfu32p0cWz7pFtYv\r\n" \
        "UiJzc2RCE0B4IS2ea8s0f4d9b7yB\r\n" \
        "-----END CERTIFICATE-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_CLIENT_PRIVATE_KEY_MSFT \
        "-----BEGIN PRIVATE KEY-----\r\n" \
        "MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAIt/lIWWJcDvbOnI\r\n" \
        "9tx0TVFthF7+KWA1eCux2UqE8HfXWER22HFwyuQ7Uv4Zp0/9yy/LvLQgg95fUUmD\r\n" \
        "UF3m/3/hcAKZS6srGAOqbw9Vr2q28mgzANzsPxZMzmdRaw8clBWSN71WcPjKXWwE\r\n" \
        "uQ1IAXu4GZ3TuOQn8asAoCWgB9BRAgMBAAECgYAca5muvbBjZmQiHTduIqO9+zoE\r\n" \
        "wHNdjoc1H77NV9hGEjGBgVUHh5tOdGesLT3RJjvHORLw/87d9z1wZLhy6SUUc6sW\r\n" \
        "VVbvY6seYQH4B+rfb5ifjyp/1FddvttD9x3ZSqMyufNkyDNpDZp7imFprcBA0m5m\r\n" \
        "NmD6usuvIjFpJGBiAQJBAMGveiLDxZcf07frLiKjwi0nvD62FZpOnbP0H53Qj4aH\r\n" \
        "ZtqnE+8XnKHOePFhb4zJ3Z/cksD+cA73Bl9AGk1VUTkCQQC4YRbL3YxpR8FHBhV3\r\n" \
        "tNrNIG4k4DHx74LBLHDDXg6IKKYuoMqekGa4+sHmJupJXkxJ80CqmGF3MCM2wxEx\r\n" \
        "sK/ZAkBYSdcQ1dDSDK4D3m6PV7qBvuUuaO38Au4cQ6HN3G4wUrRt409Q46CAmY/D\r\n" \
        "6DhjIvmlj3ROCS2nhK8g6CWVLtjxAkBBDPAQCSBK3KgSONFGJKa1jGWP2RGF/Cyz\r\n" \
        "nsxCAWM7IqxO8yfJfcTgKohSfRRXwk9EH/CGv7BTbsZdwcDFOUMxAkEAvlX2P8wu\r\n" \
        "+mL4CRP0CsF1r5B8T7VmMBGG4j+7v9mJaca3ccHKzb8FNF7l+/ohW85e4fGVW9xD\r\n" \
        "zB0dkKJ43NavXQ==\r\n" \
        "-----END PRIVATE KEY-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_CLIENT_CERT_STRING_MSFT \
        "-----BEGIN CERTIFICATE-----\r\n" \
        "MIIFmzCCBIOgAwIBAgITdAAAAAUZfaMjPGBFmwAAAAAABTANBgkqhkiG9w0BAQsF\r\n" \
        "ADBjMRMwEQYKCZImiZPyLGQBGRYDY29tMRYwFAYKCZImiZPyLGQBGRYGc2VydmVy\r\n" \
        "MRMwEQYKCZImiZPyLGQBGRYDaWFzMR8wHQYDVQQDExZpYXMtV0lOLU9OMUdCSDhM\r\n" \
        "REcwLUNBMB4XDTIxMDQyMTA5NTQ0M1oXDTIyMDQyMTA5NTQ0M1owZTETMBEGCgmS\r\n" \
        "JomT8ixkARkWA2NvbTEWMBQGCgmSJomT8ixkARkWBnNlcnZlcjETMBEGCgmSJomT\r\n" \
        "8ixkARkWA2lhczEOMAwGA1UEAxMFVXNlcnMxETAPBgNVBAMTCHdpZml1c2VyMIGf\r\n" \
        "MA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCLf5SFliXA72zpyPbcdE1RbYRe/ilg\r\n" \
        "NXgrsdlKhPB311hEdthxcMrkO1L+GadP/csvy7y0IIPeX1FJg1Bd5v9/4XACmUur\r\n" \
        "KxgDqm8PVa9qtvJoMwDc7D8WTM5nUWsPHJQVkje9VnD4yl1sBLkNSAF7uBmd07jk\r\n" \
        "J/GrAKAloAfQUQIDAQABo4ICyDCCAsQwFwYJKwYBBAGCNxQCBAoeCABVAHMAZQBy\r\n" \
        "MB0GA1UdDgQWBBThpmD/SsIg2BB/RHXlg4/0vLv/fjAOBgNVHQ8BAf8EBAMCBaAw\r\n" \
        "HwYDVR0jBBgwFoAU2kVwP5+E6OHb+X/lBoVxM925RXkwgeIGA1UdHwSB2jCB1zCB\r\n" \
        "1KCB0aCBzoaBy2xkYXA6Ly8vQ049aWFzLVdJTi1PTjFHQkg4TERHMC1DQSxDTj1X\r\n" \
        "SU4tT04xR0JIOExERzAsQ049Q0RQLENOPVB1YmxpYyUyMEtleSUyMFNlcnZpY2Vz\r\n" \
        "LENOPVNlcnZpY2VzLENOPUNvbmZpZ3VyYXRpb24sREM9aWFzLERDPXNlcnZlcixE\r\n" \
        "Qz1jb20/Y2VydGlmaWNhdGVSZXZvY2F0aW9uTGlzdD9iYXNlP29iamVjdENsYXNz\r\n" \
        "PWNSTERpc3RyaWJ1dGlvblBvaW50MIHOBggrBgEFBQcBAQSBwTCBvjCBuwYIKwYB\r\n" \
        "BQUHMAKGga5sZGFwOi8vL0NOPWlhcy1XSU4tT04xR0JIOExERzAtQ0EsQ049QUlB\r\n" \
        "LENOPVB1YmxpYyUyMEtleSUyMFNlcnZpY2VzLENOPVNlcnZpY2VzLENOPUNvbmZp\r\n" \
        "Z3VyYXRpb24sREM9aWFzLERDPXNlcnZlcixEQz1jb20/Y0FDZXJ0aWZpY2F0ZT9i\r\n" \
        "YXNlP29iamVjdENsYXNzPWNlcnRpZmljYXRpb25BdXRob3JpdHkwKQYDVR0lBCIw\r\n" \
        "IAYKKwYBBAGCNwoDBAYIKwYBBQUHAwQGCCsGAQUFBwMCMDIGA1UdEQQrMCmgJwYK\r\n" \
        "KwYBBAGCNxQCA6AZDBd3aWZpdXNlckBpYXMuc2VydmVyLmNvbTBEBgkqhkiG9w0B\r\n" \
        "CQ8ENzA1MA4GCCqGSIb3DQMCAgIAgDAOBggqhkiG9w0DBAICAIAwBwYFKw4DAgcw\r\n" \
        "CgYIKoZIhvcNAwcwDQYJKoZIhvcNAQELBQADggEBAJxCRQhdWLg5ULefFPZNOSsh\r\n" \
        "krA+4qMZhhE5wqpVfJ7JoI9qHwwxg73elAUIgItD5SLmRfJ3dCSVnDn68af3ocId\r\n" \
        "k0lGGdyCsCn3vW57FhiDPGTzQNmrUbs3fSksJiwoIa/tt/hoVKqinSkseSi1LtI6\r\n" \
        "8bNV/f8Ifar0u4U4x4QpS4jravsIyCSgpllAMgsQ9+0jmCl1o3GGtPX2fmpbPVsh\r\n" \
        "ilOCCQ8huwewe7s1KyUSl6moLHRMmzMQQDFNk01TKxLpwOqtKH7IrBtBYUAjUXKr\r\n" \
        "JsklL1cM7QTEJpHP09I4aFWoFJebE94FPA2BmULcN8aDbuZAlJ3WmagYbZCh4T4=\r\n" \
        "-----END CERTIFICATE-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_ROOT_CERT_STRING_PMF \
        "-----BEGIN CERTIFICATE-----\r\n" \
        "MIIEGjCCAwKgAwIBAgIUNJqZbmEwU+HDER28jLqWcB09dKAwDQYJKoZIhvcNAQEL\r\n" \
        "BQAwcDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFDASBgNVBAcM\r\n" \
        "C1NhbnRhIENsYXJhMRcwFQYDVQQKDA5XaS1GaSBBbGxpYW5jZTEdMBsGA1UEAwwU\r\n" \
        "V0ZBIFJvb3QgQ2VydGlmaWNhdGUwHhcNMjAwNzMwMTgwMTQ5WhcNMzAwNzI4MTgw\r\n" \
        "MTQ5WjBwMQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTEUMBIGA1UE\r\n" \
        "BwwLU2FudGEgQ2xhcmExFzAVBgNVBAoMDldpLUZpIEFsbGlhbmNlMR0wGwYDVQQD\r\n" \
        "DBRXRkEgUm9vdCBDZXJ0aWZpY2F0ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\r\n" \
        "AQoCggEBANp9wZirAd4/9rDd9L8fO8ysVA3cIBTBA3pfGlQvhSWjT/OhoKkP5Zr5\r\n" \
        "yy4K0i+ujeP0xqUDNFnhzLDNb/5LjxWVaGaGu3TpPzjKqBJT4srsIGskNwkwzt2z\r\n" \
        "IclGN4vy3/9kvIrozgZ7HAT8OKwXmXUTAD/4yw0AQv6ceFVcnNNtXsV9gJR4LU2j\r\n" \
        "PtxK5bbzV7ABj/StJCfj55UF4Tu8iL08d3rJs2r7RnR13lRWziNUtGjOT6VVKTAT\r\n" \
        "F60NZhQhp/fJ2FKizfpWTTP2TIXoBNy8JJUo+xUf9RO3mWmydKYNIqmfWY8r8O/A\r\n" \
        "Ir4xfgVtauRtfeIR9hGYyE+wmx8sDuUCAwEAAaOBqzCBqDAdBgNVHQ4EFgQUrhjL\r\n" \
        "sQJonLFV5x1AXxdPAtxLGq4wHwYDVR0jBBgwFoAUrhjLsQJonLFV5x1AXxdPAtxL\r\n" \
        "Gq4wEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAQYwQgYIKwYBBQUH\r\n" \
        "AQEENjA0MDIGCCsGAQUFBzABhiZodHRwOi8vb2NzcC5yMi10ZXN0YmVkLndpLWZp\r\n" \
        "Lm9yZzo4ODg4LzANBgkqhkiG9w0BAQsFAAOCAQEAVXYtlHbSsiDsIDvURStDqW/M\r\n" \
        "izqNXYNgP/QVOxkEacvlOym1kZmyMJaKZsJXf+XxQ9wLmn5FH4wxcOu03Ln+7eV8\r\n" \
        "DMp6QffRIaAkwbPj5hvDO6EvUXOSDNPT211axoutK91C4WgY9FM6rFZ9Rjm5fTx+\r\n" \
        "LOI9yFDt+EVtKCJVZipNss9y2YqOcUfZ+albcWELPq7BvaseEFhK+RY2xmrhTkef\r\n" \
        "OB83wBbnRo++COJN40xU7N09XsNsINEVPONDAjy9ilYIGiGtEo1habmRlfrz5QtL\r\n" \
        "AWUmTuYWA5JJHBG9w9Lrruu/fsl4XSLvvAE7Mr+S3ata6DcajMmJcs+N9iHmbA==\r\n" \
        "-----END CERTIFICATE-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_CLIENT_PRIVATE_KEY_PMF \
        "-----BEGIN PRIVATE KEY-----\r\n" \
        "MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDQ3X08kE4S7AjR\r\n" \
        "Hj8/MTpvVLTVUiWY7XWaEm+b03+5AG0MunKjC74olRWiXW2y3WrwYIOmO0UOjHky\r\n" \
        "Fu2AWCgkcSD0JZxEOtmn66QAGW6tZhvf3ChAbQAdw9qLEHVTVJohAQjil3oqSmpu\r\n" \
        "e4y2+0doZGSwDbKz9wdXmIN3acTXRol4e0N8QaziLxv/8P081U0TJ0AnssIszCX8\r\n" \
        "APDANiiAI1zqprXRxPk3HhjT6s3bbEDfrZCjx2AWBpR+ogXgm8uIoXQ2IySjdkgS\r\n" \
        "Mh19hl+00ZKdJwC4PNdsxUdJW9wn8WPqMjTHSJyJMZOH/8C0JIEy3VcwQKJ1AgkN\r\n" \
        "8QIMAtPZAgMBAAECggEAbPM8gr0bnM83Ai5u+32oZ66d7bC+H/muIMKBKZ23wD8y\r\n" \
        "ozMW3WogtXafJGTr9iN9xwnxYmMEcMRh6/j9xrc1qVzBvdlonEa+htNqZwrdKuJa\r\n" \
        "o/lpncG+D8i8VVIugVteUt0BT4GInkrXHe6gxkppQ1ErHJLopuuzDsRlv4KSxV/5\r\n" \
        "8gMBlsVRBqfbsnb/uTi+7bfnL8CelZ52YORHedz8L6iEgw6I/ASNnoaF/Fz1aP39\r\n" \
        "/2QQkUdYTKkC9bbHFx+JHXZRNVIffXQVhfcCDXHxAnvoajyfIQvEznK8J5NuEm0u\r\n" \
        "RPyPwOipQryBEAISZNE94j689IAl+SRyjrEH95jU6QKBgQD74Vs/YJv/h29Qmnd4\r\n" \
        "jXIFogUTbYn1FDpHJts5BS04mm8dVmhk0wHkIdlY8DlKvSF+u/+QHz+jtGH/3uRL\r\n" \
        "fWmo6LxcFU5eH7XYjio0YWP8JbWYQTCmq9a4JECnQa3yqFbcWGC//buUluQagzp2\r\n" \
        "pQ3LhXKxLLSj12G96DXjPo5p8wKBgQDUSAZkwWtrKCaQNI4Qr/u+IVw/khj/fc5F\r\n" \
        "suASiNBed4OKz6n0epCCE6o6qdDM9GvOEMsaORPJtffLhhpHfErcPZ7sKKdzBYts\r\n" \
        "0c1KeKWJwwrSnAaTk8DacqbIa5u3wuKrBMZK+c2nscyF8Jpv+Uj7Wo9NjRJGGxHG\r\n" \
        "lBGbYCmSAwKBgBUdgT7A5JL5UZWaPWW3xdU/DdnhFu2asoAAe+Q90Nqr56C3+6xo\r\n" \
        "HAJ56/SOvt7LGCG9tODqHbWli4mdr9m8O71TNWcqyhUN53quAE+/0lfy+VQ4MtHm\r\n" \
        "Gev57gmXcfOjtRRnvEScCrucIqcuVRpZH9zt5P2BV+tcRO9cphfUyK+NAoGAC82l\r\n" \
        "bVzD2EV3P3VsoMjoOn4oXvCEq1AXdBCqHN22mEoU6lb5oDpgZpspT/G5Rmwgwmh0\r\n" \
        "opSh4QlZKdvfAjKLBwv2FjN8fAzG39oajBWBPtO1elhqlTIV+w987o/tLJ6Nc+zk\r\n" \
        "Ca8yHCzqrlfefnNh93fll+5MIdHHfz8ka3tBVHkCgYBb86Vrpf3OKRaZCEnXdcLi\r\n" \
        "IgweD95iiKzlEdOrhU4/u1+31JLiCC57fXDmP6WPkLXYW1USOXQSfUbEy5c718dP\r\n" \
        "kaZX6AkHJdMkGYvGda9wwCxxLSQ8r/wpckbr84Mr3OKJQAXdzya7RFf1FsBRbbw3\r\n" \
        "ATz8Z/ROLNtdkhqgrRgqFw==\r\n" \
        "-----END PRIVATE KEY-----\r\n" \
        "\0"\
        "\0"

#define WIFI_WFA_ENT_CLIENT_CERT_STRING_PMF \
        "-----BEGIN CERTIFICATE-----\r\n" \
        "MIIDzDCCArSgAwIBAgIUHOt6O0IIpIzDo3IMdEKw7e46rrgwDQYJKoZIhvcNAQEL\r\n" \
        "BQAwcDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFDASBgNVBAcM\r\n" \
        "C1NhbnRhIENsYXJhMRcwFQYDVQQKDA5XaS1GaSBBbGxpYW5jZTEdMBsGA1UEAwwU\r\n" \
        "V0ZBIFJvb3QgQ2VydGlmaWNhdGUwHhcNMjAwNzMwMTgwMTQ5WhcNMzAwNzI4MTgw\r\n" \
        "MTQ5WjBcMQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTEXMBUGA1UE\r\n" \
        "CgwOV2ktRmkgQWxsaWFuY2UxHzAdBgNVBAMMFkNsaWVudCBDZXJ0aWZpY2F0ZSBJ\r\n" \
        "REwwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDQ3X08kE4S7AjRHj8/\r\n" \
        "MTpvVLTVUiWY7XWaEm+b03+5AG0MunKjC74olRWiXW2y3WrwYIOmO0UOjHkyFu2A\r\n" \
        "WCgkcSD0JZxEOtmn66QAGW6tZhvf3ChAbQAdw9qLEHVTVJohAQjil3oqSmpue4y2\r\n" \
        "+0doZGSwDbKz9wdXmIN3acTXRol4e0N8QaziLxv/8P081U0TJ0AnssIszCX8APDA\r\n" \
        "NiiAI1zqprXRxPk3HhjT6s3bbEDfrZCjx2AWBpR+ogXgm8uIoXQ2IySjdkgSMh19\r\n" \
        "hl+00ZKdJwC4PNdsxUdJW9wn8WPqMjTHSJyJMZOH/8C0JIEy3VcwQKJ1AgkN8QIM\r\n" \
        "AtPZAgMBAAGjcjBwMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFJgjVzYiNkdDhvpU\r\n" \
        "tK7D0vqa3NzgMB8GA1UdIwQYMBaAFK4Yy7ECaJyxVecdQF8XTwLcSxquMBMGA1Ud\r\n" \
        "JQQMMAoGCCsGAQUFBwMCMAsGA1UdDwQEAwIF4DANBgkqhkiG9w0BAQsFAAOCAQEA\r\n" \
        "iGC0RDnySgq/Mbbhc2RZEHZzWkBPPl2/qhWEO7XRGP4Zk45RbFNglbCQhzEFRsFq\r\n" \
        "sEx3xLfLpK+yGhi4tvlWipxPiGREVHn7qV6xnrMK/oN17BeBjdzZR1t3GDxTWfcQ\r\n" \
        "PrNWFNvptIAqd0mfBg6sSlFK6L8oRCzD1pg6x8Jvdx2o11bvz/VVUM3xhTODFDro\r\n" \
        "uwuC62pLtMUoJnhBdB/GN7et4Xf/nuwfw0lp8aAn0wT1xMQwk+LTPX2DVzRiOLNR\r\n" \
        "8gnjgjfFNmQzcZ4VdKviujytVDzqgwMyjbQBhwbL4HFPPrk2iArOlqTctAmMId7R\r\n" \
        "Kiwu3nJJjz2Re71qTJyLfQ==\r\n" \
        "-----END CERTIFICATE-----\r\n" \
        "\0"\
        "\0"

#ifdef __cplusplus
} /*extern "C" */
#endif
