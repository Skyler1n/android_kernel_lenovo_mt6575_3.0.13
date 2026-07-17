ccflags-y += -I$(src)/linux/include -I$(src)/linux/pri/include -I$(src)/core/include -I$(src)/include -I$(src)/

COMMON_SRC_PATH := $(src)
COMMON_OBJ_PATH := $(src)

# WMT DRIVER
obj-$(CONFIG_MTK_COMBO) += mtk_stp_wmt.o
# WMT DRIVER-core part
mtk_stp_wmt-objs	:= core/wmt_core.module core/wmt_ctrl.module  core/wmt_func.module core/wmt_ic_6620.module core/wmt_lib.module core/wmt_conf.module
$(shell cp $(COMMON_SRC_PATH)/core/wmt_core.o.new 				        $(COMMON_OBJ_PATH)/core/wmt_core.module)
$(shell cp $(COMMON_SRC_PATH)/core/wmt_ctrl.o.new 				        $(COMMON_OBJ_PATH)/core/wmt_ctrl.module)
$(shell cp $(COMMON_SRC_PATH)/core/wmt_func.o.new 				        $(COMMON_OBJ_PATH)/core/wmt_func.module)
$(shell cp $(COMMON_SRC_PATH)/core/wmt_ic_6620.o.new 				      $(COMMON_OBJ_PATH)/core/wmt_ic_6620.module)
$(shell cp $(COMMON_SRC_PATH)/core/wmt_lib.o.new 				          $(COMMON_OBJ_PATH)/core/wmt_lib.module)
$(shell cp $(COMMON_SRC_PATH)/core/wmt_conf.o.new 				        $(COMMON_OBJ_PATH)/core/wmt_conf.module)



# WMT DRIVER-linux private part
mtk_stp_wmt-objs	+= linux/pri/wmt_dev.module linux/pri/wmt_exp.module
$(shell cp $(COMMON_SRC_PATH)/linux/pri/wmt_dev.o.new 				        $(COMMON_OBJ_PATH)/linux/pri/wmt_dev.module)
$(shell cp $(COMMON_SRC_PATH)/linux/pri/wmt_exp.o.new 				        $(COMMON_OBJ_PATH)/linux/pri/wmt_exp.module)

# WMT DRIVER-OSAL
mtk_stp_wmt-objs	+= linux/osal.o
# WMT DRIVER-platform implementation
ccflags-y += -D WMT_PLAT_ALPS
mtk_stp_wmt-objs	+= platform/alps/wmt_plat_alps.o
# FIXME: select desired hw according to project configuration
mtk_stp_wmt-objs	+= platform/alps/mtk_wcn_cmb_hw_6620.o 
#mtk_stp_wmt-objs	+= platform/alps/mtk_wcn_cmb_hw_6628.o 
mtk_stp_wmt-objs	+= linux/pri/stp_exp.module core/stp_core.module core/psm_core.module core/btm_core.module linux/pri/stp_dbg.module
$(shell cp $(COMMON_SRC_PATH)/linux/pri/stp_exp.o.new 				    $(COMMON_OBJ_PATH)/linux/pri/stp_exp.module)
$(shell cp $(COMMON_SRC_PATH)/core/stp_core.o.new 				        $(COMMON_OBJ_PATH)/core/stp_core.module)
$(shell cp $(COMMON_SRC_PATH)/core/psm_core.o.new 				        $(COMMON_OBJ_PATH)/core/psm_core.module)
$(shell cp $(COMMON_SRC_PATH)/core/btm_core.o.new 				        $(COMMON_OBJ_PATH)/core/btm_core.module)
$(shell cp $(COMMON_SRC_PATH)/linux/pri/stp_dbg.o.new 				    $(COMMON_OBJ_PATH)/linux/pri/stp_dbg.module)


# WMT stub part (built-in kernel image)
obj-y			+= platform/alps/mtk_wcn_cmb_stub_alps.o


obj-$(CONFIG_MTK_COMBO) += mtk_stp_uart.o
mtk_stp_uart-objs	:= linux/pri/stp_uart.module
$(shell cp $(COMMON_SRC_PATH)/linux/pri/stp_uart.o.new 				        $(COMMON_OBJ_PATH)/linux/pri/stp_uart.module)

obj-$(CONFIG_MTK_COMBO) += mtk_hif_sdio.o
mtk_hif_sdio-objs	:= linux/hif_sdio.o

obj-$(CONFIG_MTK_COMBO) += mtk_stp_sdio.o
mtk_stp_sdio-objs	:= linux/pri/stp_sdio.module
$(shell cp $(COMMON_SRC_PATH)/linux/pri/stp_sdio.o.new 				        $(COMMON_OBJ_PATH)/linux/pri/stp_sdio.module)

obj-$(CONFIG_MTK_COMBO_BT) += mtk_stp_bt.o
mtk_stp_bt-objs		:= linux/stp_chrdev_bt.o

obj-$(CONFIG_MTK_COMBO_GPS) += mtk_stp_gps.o
mtk_stp_gps-objs	:= linux/stp_chrdev_gps.o


obj-$(CONFIG_MTK_COMBO_WIFI) += mtk_wmt_wifi.o
mtk_wmt_wifi-objs       := linux/wmt_chrdev_wifi.o 
