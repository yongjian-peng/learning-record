#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include "iniparser/iniparser.h"
#include "ctrlboard.h"
#include "network_config.h"

/* Eason Refined in Mar. 2020 */


/* WIFI Static Varibles */
static struct timeval       tvStart     = {0, 0}, tvEnd     = {0, 0},
                            tvStartWifi = {0, 0}, tvEndWifi = {0, 0};
static bool                 networkWifiIsReady, process_set  = false;
static int                  wifimgr_init = WIFIMGR_ECODE_NOT_INIT;
static int                  gWebServerInit = 0;
/* WIFI Globel Varibles */
WIFI_MGR_SETTING            gWifiSetting;


/* ======================================================================================= */

/**
  *
  *Static Network Functions
  *
  */

/* ======================================================================================= */

static void ResetWifi(void)
{
    char buf[16], *saveptr;

    memset(&gWifiSetting.setting, 0, sizeof (ITPEthernetSetting));

    gWifiSetting.setting.index = 0;

    // dhcp
    if (Ethernet_Wifi_DualMAC == 0)
        gWifiSetting.setting.dhcp     = theConfig.dhcp;
    else
        gWifiSetting.setting.dhcp     = 1; //if eth+wifi daul work, use DHCP IP.


    // autoip
    gWifiSetting.setting.autoip = 0;

    // ipaddr
    strcpy(buf, theConfig.ipaddr);
    gWifiSetting.setting.ipaddr[0] = atoi(strtok_r(buf,  ".", &saveptr));
    gWifiSetting.setting.ipaddr[1] = atoi(strtok_r(NULL, ".", &saveptr));
    gWifiSetting.setting.ipaddr[2] = atoi(strtok_r(NULL, ".", &saveptr));
    gWifiSetting.setting.ipaddr[3] = atoi(strtok_r(NULL, " ", &saveptr));

    // netmask
    strcpy(buf, theConfig.netmask);
    gWifiSetting.setting.netmask[0] = atoi(strtok_r(buf,  ".", &saveptr));
    gWifiSetting.setting.netmask[1] = atoi(strtok_r(NULL, ".", &saveptr));
    gWifiSetting.setting.netmask[2] = atoi(strtok_r(NULL, ".", &saveptr));
    gWifiSetting.setting.netmask[3] = atoi(strtok_r(NULL, " ", &saveptr));

    // gateway
    strcpy(buf, theConfig.gw);
    gWifiSetting.setting.gw[0] = atoi(strtok_r(buf,  ".", &saveptr));
    gWifiSetting.setting.gw[1] = atoi(strtok_r(NULL, ".", &saveptr));
    gWifiSetting.setting.gw[2] = atoi(strtok_r(NULL, ".", &saveptr));
    gWifiSetting.setting.gw[3] = atoi(strtok_r(NULL, " ", &saveptr));
}

#if defined(CFG_NET_WIFI_SDIO_VND_RTK)

void* NetworkWifiReinit(void *arg)
{
    int i;

    printf("NetworkReinit \n");

    wifiMgr_terminate();
    usleep(200 * 1000);

    wifi_off();
    ioctl(ITP_DEVICE_SDIO, ITP_IOCTL_EXIT, NULL);
#if 1
/*
    ithLockMutex(ithStorMutex);

    ithSdSetGpioLow(ITH_STOR_SD1);

    ithWIFICardPowerOff();
    // wait 200 ms
    usleep(200 * 1000);

    ithWIFICardPowerOn();
    // wait 200 ms
    usleep(200 * 1000);

    ithUnlockMutex (ithStorMutex);
*/
    ithWIFICardPowerProcess();

#endif

    ioctl(ITP_DEVICE_SDIO, ITP_IOCTL_INIT, NULL);

    wifi_on(RTW_MODE_STA);
    //ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_INIT, NULL);

    usleep(200 * 1000);

    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL); //determine wifi client mode
    wifiMgr_init(WIFIMGR_MODE_CLIENT, 0, gWifiSetting);


}


int createWifiReinitThread(void)
{
    pthread_t       task;
    pthread_attr_t  attr;
    sleep(5);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, (255 * 1024));
    pthread_create(&task, &attr, NetworkWifiReinit, NULL);
    return 0;
}

#endif

static int wifiCallbackFucntion(int nState)
{
    switch (nState)
    {
        case WIFIMGR_STATE_CALLBACK_CONNECTION_FINISH:
            printf("[Ctrlboard] WifiCallback connection finish \n");
            networkWifiIsReady = true;
#if TEST_WIFI_DOWNLOAD
            sleep(5);
            createHttpThread();
#endif
#if TEST_WIFI_REINIT
	sleep(5);
	createWifiReinitThread();
#endif
#if 1// defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6212)
            if (gWebServerInit ==0){
                WebServerInit();
            }
            gWebServerInit++;
#else
            if (Ethernet_Wifi_DualMAC == 0)
                WebServerInit();
#endif

#ifdef CFG_NET_FTP_SERVER
		    ftpd_setlogin(theConfig.user_id, theConfig.user_password);
		    ftpd_init();
#endif

#if defined(CFG_NET_WIFI_SDIO)
            if (theConfig.wifi_mode == WIFIMGR_MODE_SOFTAP){
                strcpy(gWifiSetting.ssid,theConfig.ssid);
                strcpy(gWifiSetting.password,theConfig.password);
	#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
                gWifiSetting.secumode = WifiMgr_Secu_ITE_To_RTL(theConfig.secumode);
    #endif
            }
#endif
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_DISCONNECT_30S:
            printf("[Ctrlboard] WifiCallback connection disconnect 30s \n");
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_RECONNECTION:
            printf("[Ctrlboard] WifiCallback connection reconnection \n");
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_TEMP_DISCONNECT:
            printf("[Ctrlboard] WifiCallback connection temp disconnect \n");
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_CONNECTING_FAIL:
            printf("[Ctrlboard] WifiCallback connecting fail, please check ssid,password,secmode \n");
			WifiMgr_Sta_Disconnect();
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_CONNECTING_CANCEL:
            printf("[Ctrlboard] WifiCallback connecting end to sleep/cancel \n");
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_SLEEP_SAVE_INFO:
            strcpy(gWifiSetting.ssid,theConfig.ssid);
            strcpy(gWifiSetting.password,theConfig.password);
#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
            gWifiSetting.secumode = WifiMgr_Secu_ITE_To_RTL(theConfig.secumode);
#else
            snprintf(gWifiSetting.secumode, WIFI_SECUMODE_MAXLEN,   theConfig.secumode);
#endif
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_SLEEP_CLEAN_INFO:
            printf("[Ctrlboard] WifiCallback clean connecting info \n");
            snprintf(gWifiSetting.ssid,     WIFI_SSID_MAXLEN,       "");
            snprintf(gWifiSetting.password, WIFI_PASSWORD_MAXLEN,   "");
#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
            gWifiSetting.secumode = 0;
#else
            snprintf(gWifiSetting.secumode, WIFI_SECUMODE_MAXLEN,   "");
#endif
        break;

        default:
            printf("[Ctrlboard] WifiCallback unknown %d state  \n",nState);
        break;
    }
}


static int NetworkWifiPowerSleep(void)
{
    int  ret = WIFIMGR_ECODE_NOT_INIT;

#if defined(CFG_NET_WIFI_SDIO_VND_RTK)

    /* ======================  For 8189FTV/8723DS  ====================== */
    /* Confirm current status of sleep mode */
    if (ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFI_SLEEP_STATUS, NULL) == sleep_to_wakeup) {
        ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_SLEEP, (void *)default_no_sleep_or_wakeup);

        if (theConfig.wifi_mode == WIFIMGR_MODE_SOFTAP) {
            ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFIAP_ENABLE, NULL); //determine wifi softAP mode
            ret = WifiMgr_Init(WIFIMGR_MODE_SOFTAP, 0, gWifiSetting);
        } else {
            ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL); //determine wifi client mode
            ret = WifiMgr_Init(WIFIMGR_MODE_CLIENT, 0, gWifiSetting);
        }
    }
    /* ====================================================== */
#elif defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6212) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6203) || defined(CFG_NET_WIFI_SDIO_NGPL_ATBM6031) 

    /* ======================  For AP6256/AP6236/AP6212 ====================== */
    /* ======================  For ATBM6031                           ====================== */

    /* Confirm current status of sleep mode */
    if (ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFI_SLEEP_STATUS, NULL) == sleep_to_wakeup) {
        printf("NetworkWifiPowerSleep sleep_to_wakeup \n");
        ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_SLEEP, (void *)default_no_sleep_or_wakeup);

        if (theConfig.wifi_mode == WIFIMGR_MODE_SOFTAP) {
            ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_WIFIAP_ENABLE, NULL); //determine wifi softAP mode
            ret = WifiMgr_Init(WIFIMGR_MODE_SOFTAP, 0, gWifiSetting);
        } else {
            ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL); //determine wifi client mode
            ret = WifiMgr_Init(WIFIMGR_MODE_CLIENT, 0, gWifiSetting);
        }
    }


    /* ====================================================== */

#endif

    return ret;
}




/* ======================================================================================= */

/**
  *
  *Network Functions
  *
  */

/* ======================================================================================= */

void NetworkWifiPreSetting(void)
{
    networkWifiIsReady = false;

    strcpy(gWifiSetting.ssid,theConfig.ssid);
    strcpy(gWifiSetting.password,theConfig.password);
#if defined(CFG_NET_WIFI_SDIO)
	#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
    gWifiSetting.secumode = WifiMgr_Secu_ITE_To_RTL(theConfig.secumode);
    #endif

    if (theConfig.wifi_mode == WIFIMGR_MODE_SOFTAP){
        strcpy(gWifiSetting.ssid,theConfig.ap_ssid);
        strcpy(gWifiSetting.password,theConfig.ap_password);
    }
#else
    snprintf(gWifiSetting.secumode, WIFI_SECUMODE_MAXLEN,   theConfig.secumode);
#endif

    gWifiSetting.wifiCallback = wifiCallbackFucntion;

    /* Reset WIFI IP*/
    ResetWifi();

    gettimeofday(&tvStart, NULL);
}


/* For WIFI main task in network_main.c  */
void NetworkWifiProcess(void)
{
    int process_tv; //msec

    gettimeofday(&tvEnd, NULL);

    process_tv = (int)itpTimevalDiff(&tvStart, &tvEnd);

    if (process_tv > Network_Time_Delay && process_set == false) {
#if !defined(CFG_NET_WIFI_SDIO)
        while(!ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_IS_DEVICE_READY, NULL)) {
            usleep(100*1000);
            printf("Wait WIFI device(USB) get ready... \n");
        }
#endif

        WifiMgr_Sta_Switch(theConfig.wifi_on_off);
        printf("[%s] WIFI set %s mode, switch %s\n", __FUNCTION__,
            (theConfig.wifi_mode == 0) ? "Station":
            (theConfig.wifi_mode == 1) ? "HostAP":
            (theConfig.wifi_mode == 2) ? "Concurrent":"Unknown",
                theConfig.wifi_on_off ? "ON":"OFF");

        if (theConfig.wifi_mode == WIFIMGR_MODE_SOFTAP){
#if defined(CFG_NET_WIFI_SDIO)
            WifiMgr_HostAP_First_Start();
#endif
            wifimgr_init = WifiMgr_Init(WIFIMGR_MODE_SOFTAP, 0, gWifiSetting);
        } else if (theConfig.wifi_mode == WIFIMGR_MODE_MAX) {
#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
            WifiMgr_Concurrent_First_Start();
#endif
            wifimgr_init = WifiMgr_Init(WIFIMGR_MODE_MAX, 0, gWifiSetting);
        } else {
#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
            ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_ENABLE, NULL); //determine wifi client mode
#endif

            WifiMgr_Sta_Switch(theConfig.wifi_on_off);
            wifimgr_init = WifiMgr_Init(WIFIMGR_MODE_CLIENT, 0, gWifiSetting);
        }

#if TEST_PING_WIFI
        while(!networkWifiIsReady){
            usleep(100*1000);
        }
        ping_main();
#endif

        process_set = true;
    } else if (process_set == true) {
        networkWifiIsReady = WifiMgr_Get_Sta_Connect_Complete();

#if defined(CFG_POWER_SLEEP)

        wifimgr_init = NetworkWifiPowerSleep();
#endif

#if TEST_CHANGE_AP
        if (networkWifiIsReady)
            link_differnet_ap();
#endif

#if TEST_SCAN_AP
        scan_ap_loop();
#endif
    }
}


bool NetworkWifiIsReady(void)
{
    return networkWifiIsReady;
}


void NetworkWifiModeSwitch(void)
{
	int ret;

	ret = WifiMgr_Sta_HostAP_Switch(gWifiSetting);

    /* Err code definition: sdk/include/linux/errno.h */
    printf("[NetworkWifiModeSwitch] Sta HostAP Switch task(%d)\n", ret);
}

/* ======================================================================================= */




/* Useless!! For only build WIFI in indoor project */
#if !defined(CFG_NET_ETHERNET)
bool NetworkIsReady(void)
{
    return false;
}

bool NetworkServerIsReady(void)
{
    return false;
}

void NetworkExit(void)
{
    //Not implement in network_wifi.c
}

void NetworkSntpUpdate(void)
{
    //Not implement in network_wifi.c
}
#endif

