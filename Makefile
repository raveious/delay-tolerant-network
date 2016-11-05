COMPONENT=DTNAppC
TINYOS_ROOT_DIR?=tinyos-main
TINYOS_NO_DEPRECATION_WARNING?=1 # Telosb uses the older build system. Surpressing warning.
include $(TINYOS_ROOT_DIR)/Makefile.include
