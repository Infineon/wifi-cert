# WiFi Cert Middleware for ModusToolbox SDK

## Overview

The WiFi Cert middleware provides an easy way to test WFA cert such as 802.11n-PSK, WPA2 Enterprise security, PMF, WPA3 and 802.11AC using ModusToolbox SDK.

The WiFi Cert middleware consists of the following components:

- Sigma Control API specification (CAPI) commands and command handler functions to process commands to and from a test agent.

- WiFi Cert middleware Porting Layer to interface  with Wi-Fi and network layer functionality across SDKs such as FreeRTOS.

## Features

This library provides the library functions used by the WiFi Cert Tester tool to execute WFA Certification test suites for 802.11n PSK, Enterprise security, PMF, WPA3, and 802.11AC.

## Requirements

- [ModusToolbox® software](https://www.cypress.com/products/modustoolbox-software-environment) v2.4

- Programming Language: C

## Supported Platforms

This library and its features are supported on the following Cypress platforms:

- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; prototyping kit](https://www.cypress.com/CY8CPROTO-062-4343W) (CY8CPROTO-062-4343W)

- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; pioneer kit](https://www.cypress.com/CY8CKIT-062S2-43012)  (CY8CKIT-062S2-43012)

- [PSoC&trade; PSoC6 Development Kit](https://www.cypress.com/part/cy8ceval-062s2)(CY8CEVAL-062S2) + [Sterling LWB5Plus](https://www.mouser.com/new/laird-connectivity/laird-connectivity-sterling-lwb5plus) (CY8CEVAL-062S2-LAI-4373M2)

## Dependent Libraries

- [Wi-Fi Connection Manager](https://github.com/cypresssemiconductorco/wifi-connection-manager)

- [Command Console](https://github.com/cypresssemiconductorco/command-console)

- [Enterprise Security](https://github.com/cypresssemiconductorco/enterprise-security)


## Integration Notes

- The WiFi Cert Test Middleware library has been designed to work with ModusToolbox SDK.

* The library has been integrated into WiFi Cert Tester tool for ModusToolbox SDK.

* You only need to include this library in the required ecosystem to use these utilities. Depending on the ecosystem, the respective source files will be picked up and linked, using the `COMPONENT_ model_`.

## Additional Information

- [WiFi Cert Test Library API reference guide](https://cypresssemiconductorco.github.io/wifi-cert/api_reference_manual/html/index.html)

- [WiFi Cert Test Library version](./version.txt)

## Supported Software and Tools
ToolChain | OS
----------|---
GCC_ARM  IAR and ARMC6 | FreeRTOS
------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

-------------------------------------------------------------------------------

(c) 2021, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
This software, associated documentation and materials ("Software") is owned by Cypress Semiconductor Corporation or one of its affiliates ("Cypress") and is protected by and subject to worldwide patent protection (United States and foreign), United States copyright laws and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software ("EULA"). If no EULA applies, then any reproduction, modification, translation, compilation, or representation of this Software is prohibited without the express written permission of Cypress.
Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress reserves the right to make changes to the Software without notice. Cypress does not assume any liability arising out of the application or use of the Software or any product or circuit described in the Software. Cypress does not authorize its products for use in any products where a malfunction or failure of the Cypress product may reasonably be expected to result in significant property damage, injury or death ("High Risk Product"). By including Cypress's product in a High Risk Product, the manufacturer of such system or application assumes all risk of such use and in doing so agrees to indemnify Cypress against all liability.