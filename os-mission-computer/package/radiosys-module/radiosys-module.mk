RADIOSYS_MODULE_VERSION = 1.0
RADIOSYS_MODULE_SITE = $(BR2_EXTERNAL_RADIOSYS_PATH)/package/radiosys-module/src
RADIOSYS_MODULE_SITE_METHOD = local
RADIOSYS_MODULE_MODULE_SUBDIRS = .
$(eval $(kernel-module))
$(eval $(generic-package))

