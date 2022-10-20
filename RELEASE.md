# WiFi Cert Middleware for ModusToolbox SDK

## What's Included?
See the [README.md](./README.md) for a complete description of the WiFi Cert Middleware using ModusToolbox SDK.

## Changelog

### v4.0.0
* Adapt the WiFi-Cert library  API(s) to wifi-connection manager(3.x) and wifi-core-freertos-lwip-mbedtls (1.x)
* Fixes issues for broadcast traffic receiption and ping thread fix for PMF

### v3.4.0
* Add support for setting Preferred network for WPA3 H2E (password-based element)PT derivation.
* Add command to dump H2E AP(s)

### v3.3.0
Remove supported platforms section from README.md

### v3.2.0
* Added support for CY8CEVAL-062S2-LAI-4373M2 kit
* Update Copyright sections and add Infineon logo
* Bug fix to check for stack memory allocation after max retries

### v3.1.0
* Added support for WPA2 Enterprise security with following protocols
	* EAP-TLS
	* EAP-TTLS
	* PEAP0
* Fix Coverity issues 
* Fixed command console library github link in README.md

### v3.0.0
* Initial release
* 11n PSK Certification Passed

### Supported Software and Tools
This version of the library was validated for compatibility with the following Software and Tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox Software Environment                       | 3.0.0   |
| - ModusToolbox Device Configurator                      | 3.10.0  |
| PSoC 6 Peripheral Driver Library (PDL)                  | 2.4.0   |
| GCC Compiler                                            | 10.3.1  |
| IAR Compiler                                            | 9.3     |
| ARMC6 Compiler                                          | 6.16    |