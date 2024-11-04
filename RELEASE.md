# WiFi Cert Middleware for ModusToolbox SDK

## What's Included?
See the [README.md](./README.md) for a complete description of the WiFi Cert Middleware using ModusToolbox SDK.

## Changelog

### v5.1.0
* For CYW955913 ES100.1 Tri band release
* Supports 6GHz cert tests, MBO, OCE, OWE test cases

### v5.0.0
* For CYW955913 ES100 Dual band release

### v4.4.1
* Sigma changes to support H1CP cert
* Changes to support netxduo calls
* Sigma code chanegs fpr PMF, implementation of sta_set_11n, correct device_get_info
* correct MCS in VHT capabilites in assoc request
* Correct WPA2 configuration when AP is in PMF required or optional mode.

### v4.4.0
* Fixes DUT send assoc request with an AP that only supports WPA2-Personal, when SAE Transition Disable
* Provide the correct firmware version in device_get_info
* Fixes sta_reassoc capi failed to return status completed
* Add Clear PMKID API in sta_reset_default

### v4.3.0
* Fixes capi misses sending "status,COMPLETE" to UCC 
* Enlarge CAPI command length up to 1024 for supporting WPA3 192bit

### v4.2.0
* Added support for CYW943907AEVAL1F kit

### v4.1.0
* Add support for 11AX STA's Sigma Command

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
