








#include "osal_typedef.h"

#include "mtk_wcn_cmb_hw.h"
#include "wmt_plat.h"
#include "wmt_lib.h"










PWR_SEQ_TIME gPwrSeqTime;








INT32 wmt_cmb_hw_pwr_off(VOID)
{
    INT32 iRet = 0;
    WMT_INFO_FUNC("WMT-CMB, hw_pwr_off start\n");

/*1. disable irq --> should be done when do wmt-ic swDeinit period*/

/*2. set bgf eint/all eint to deinit state, namely input low state*/
//    wmt_plat_gpio_ctrl(PIN_ALL_EINT,  PIN_STA_DEINIT);
    iRet += wmt_plat_gpio_ctrl(PIN_BGF_EINT,  PIN_STA_DEINIT);

/*3. set audio interface to CMB_STUB_AIF_0, BT PCM OFF, I2S OFF*/
    iRet += wmt_plat_audio_ctrl(CMB_STUB_AIF_0, CMB_STUB_AIF_CTRL_DIS);

/*4. set control gpio into deinit state, namely input low state*/
    iRet += wmt_plat_gpio_ctrl(PIN_LDO,       PIN_STA_DEINIT);
    iRet += wmt_plat_gpio_ctrl(PIN_PMU,       PIN_STA_DEINIT);
    iRet += wmt_plat_gpio_ctrl(PIN_RST,       PIN_STA_DEINIT);

/*5. set uart tx/rx into deinit state, namely input low state*/
    iRet += wmt_plat_gpio_ctrl(PIN_UART_GRP,  PIN_STA_DEINIT);

    iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA,  PIN_STA_DEINIT);
    iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC,  PIN_STA_DEINIT);
    WMT_INFO_FUNC("WMT-CMB, hw_pwr_off finish\n");
    return iRet;
}



INT32 wmt_cmb_hw_pwr_on(VOID)
{
    static UINT32 _pwr_first_time = 0;
    INT32 iRet = 0;
    WMT_INFO_FUNC("WMT-CMB, hw_pwr_on start\n");
/*set all control and eint gpio to init state, namely input low mode*/
    iRet += wmt_plat_gpio_ctrl(PIN_LDO,       PIN_STA_INIT);
    iRet += wmt_plat_gpio_ctrl(PIN_PMU,       PIN_STA_INIT);
    iRet += wmt_plat_gpio_ctrl(PIN_RST,       PIN_STA_INIT);
    iRet += wmt_plat_gpio_ctrl(PIN_BGF_EINT,  PIN_STA_INIT);
    iRet += wmt_plat_gpio_ctrl(PIN_GPS_LNA,  PIN_STA_INIT);
    iRet += wmt_plat_gpio_ctrl(PIN_GPS_SYNC,  PIN_STA_INIT);
//    wmt_plat_gpio_ctrl(PIN_WIFI_EINT, PIN_STA_INIT);
//    wmt_plat_gpio_ctrl(PIN_ALL_EINT,  PIN_STA_INIT);

/*1. pull high LDO to supply power to chip*/
    iRet += wmt_plat_gpio_ctrl(PIN_LDO,       PIN_STA_OUT_H);
    osal_msleep(gPwrSeqTime.ldoStableTime);

/*2. export RTC clock to chip*/
    if (0 == _pwr_first_time)
    {
    /*rtc clock should be output all the time, so no need to enable output again*/
        iRet += wmt_plat_gpio_ctrl(PIN_RTC,       PIN_STA_INIT);
        osal_msleep(gPwrSeqTime.rtcStableTime);
        _pwr_first_time = 1;
        WMT_INFO_FUNC("WMT-CMB, rtc clock exported\n");
    }

/*3. set UART Tx/Rx to UART mode*/
    iRet += wmt_plat_gpio_ctrl(PIN_UART_GRP,  PIN_STA_INIT);

/*4. PMU->output low, RST->output low, sleep off stable time*/
    iRet += wmt_plat_gpio_ctrl(PIN_PMU,       PIN_STA_OUT_L);
    iRet += wmt_plat_gpio_ctrl(PIN_RST,       PIN_STA_OUT_L);

/*5. PMU->output high, sleep rst stable time*/
    iRet += wmt_plat_gpio_ctrl(PIN_PMU,       PIN_STA_OUT_H);
    osal_msleep(gPwrSeqTime.rstStableTime);

/*6. RST->output high, sleep on stable time*/
    iRet += wmt_plat_gpio_ctrl(PIN_RST,       PIN_STA_OUT_H);
    osal_msleep(gPwrSeqTime.onStableTime);

/*7. set audio interface to CMB_STUB_AIF_1, BT PCM ON, I2S OFF*/
    iRet += wmt_plat_audio_ctrl(CMB_STUB_AIF_1, CMB_STUB_AIF_CTRL_DIS);

/*8. set EINT< -ommited-> move this to WMT-IC module, where common sdio interface will be identified and do proper operation*/

    WMT_INFO_FUNC("WMT-CMB, hw_pwr_on finish\n");
    return iRet;

}

INT32 wmt_cmb_hw_rst(VOID)
{
    WMT_INFO_FUNC("WMT-CMB, hw_rst start, eirq should be disabled before this step\n");
    wmt_plat_gpio_ctrl(PIN_RST,       PIN_STA_OUT_L);
    osal_msleep(gPwrSeqTime.rstStableTime);
    wmt_plat_gpio_ctrl(PIN_RST,       PIN_STA_OUT_H);
    WMT_INFO_FUNC("WMT-CMB, hw_rst finish, eirq should be enabled after this step\n");
    return 0;
}

static VOID wmt_cmb_hw_dmp_seq (VOID)
{
    PUINT32 pTimeSlot = (PUINT32)&gPwrSeqTime;
    WMT_INFO_FUNC("combo chip power on sequence time, RTC (%d), LDO (%d),RST(%d),  OFF(%d), ON(%d)\n", *pTimeSlot++, *pTimeSlot++, *pTimeSlot++, *pTimeSlot++, *pTimeSlot++);
    return;
}

INT32 wmt_cmb_hw_init(P_PWR_SEQ_TIME pPwrSeqTime)
{
    if (NULL == pPwrSeqTime)
    {
        WMT_WARN_FUNC("pPwrSeqTime = NULL, use default hw init sequence parameters\n");
        gPwrSeqTime.ldoStableTime = DFT_LDO_STABLE_TIME;
        gPwrSeqTime.offStableTime = DFT_OFF_STABLE_TIME;
        gPwrSeqTime.onStableTime = DFT_ON_STABLE_TIME;
        gPwrSeqTime.rstStableTime = DFT_RST_STABLE_TIME;
        gPwrSeqTime.rtcStableTime = DFT_RTC_STABLE_TIME;
    }
    else
    {
    #if 0
        gPwrSeqTime.ldoStableTime = pPwrSeqTime->ldoStableTime;
        gPwrSeqTime.offStableTime = pPwrSeqTime->offStableTime;
        gPwrSeqTime.onStableTime = pPwrSeqTime->onStableTime;
        gPwrSeqTime.rstStableTime = pPwrSeqTime->rstStableTime;
        gPwrSeqTime.rtcStableTime = pPwrSeqTime->rtcStableTime;
    #endif
    /*memcpy may be more performance*/
        WMT_DBG_FUNC("setting hw init sequence parameters\n");
        osal_memcpy(&gPwrSeqTime, pPwrSeqTime, osal_sizeof(gPwrSeqTime));
    }
    wmt_cmb_hw_dmp_seq();
    return 0;
}

INT32 wmt_cmb_hw_deinit(VOID)
{

    WMT_WARN_FUNC("wmt_cmb_hw_deinit start, set to default hw init sequence parameters\n");
    gPwrSeqTime.ldoStableTime = DFT_LDO_STABLE_TIME;
    gPwrSeqTime.offStableTime = DFT_OFF_STABLE_TIME;
    gPwrSeqTime.onStableTime = DFT_ON_STABLE_TIME;
    gPwrSeqTime.rstStableTime = DFT_RST_STABLE_TIME;
    gPwrSeqTime.rtcStableTime = DFT_RTC_STABLE_TIME;
    WMT_WARN_FUNC("wmt_cmb_hw_deinit finish\n");
    return 0;
}







