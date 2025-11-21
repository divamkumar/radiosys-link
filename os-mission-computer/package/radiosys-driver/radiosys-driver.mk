RADIOSYS_DRIVER_VERSION = 1.0
RADIOSYS_DRIVER_SITE = $(BR2_EXTERNAL_RADIOSYS_PATH)/package/radiosys-driver/src
RADIOSYS_DRIVER_SITE_METHOD = local

define RADIOSYS_DRIVER_BUILD_CMDS
    $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef

define RADIOSYS_DRIVER_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/radiosys_daemon $(TARGET_DIR)/usr/bin/radiosys_daemon
endef

$(eval $(generic-package))

