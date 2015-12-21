# FreeRTOS build configuration
#
# Alexander Smirnov <asmirnov@ilbers.de>
# Copyright (c) 2014-2015 ilbers GmbH

# Choose the target platform:
# a20        - Allwinner A20 SoC
# exynos5250 - Samsung Exynos5250 SoC
PLATFORM := a20

# Choose demo appication:
# default    - Default LotOS application
APPLICATION := default

# Build options
DCONFIG += -DCONFIG_NET_LWIP
DCONFIG += -DCONFIG_MANGO_WATCHDOG
