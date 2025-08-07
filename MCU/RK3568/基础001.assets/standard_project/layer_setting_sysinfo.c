#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwip/ip.h"
#include "lwip/netif.h"
#include "ite/itp.h"
#include "scene.h"
#include "ctrlboard.h"
#include "wifiMgr.h"

static ITUText* settingSysInfoIPAddrText;
static ITUText* settingSysInfoMacText;
static ITUText* settingSysInfoHWVersionText;
static ITUText* settingSysInfoSWVersionText;

#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
#include "wifi_conf.h"
extern struct netif xnetif[NET_IF_NUM];
extern uint8_t* LwIP_GetIP(struct netif *pnetif);
#endif

#if defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6181)
#include "mhd_api.h"

#define FORMAT_IPADDR(x) ((unsigned char *)&x)[3], ((unsigned char *)&x)[2], ((unsigned char *)&x)[1], ((unsigned char *)&x)[0]

#endif

#if defined(CFG_NET_WIFI_SDIO_NGPL_ATBM6031)
#define FORMAT_IPADDR(x) ((unsigned char *)&x)[3], ((unsigned char *)&x)[2], ((unsigned char *)&x)[1], ((unsigned char *)&x)[0]
#endif


bool SettingSysInfoOnEnter(ITUWidget* widget, char* param)
{
#if defined(CFG_NET_WIFI)
#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
	unsigned char *ip_r = LwIP_GetIP(&xnetif[0]);
	unsigned char cMac [6];
    WifiMgr_Get_MAC_Address(cMac);
#else
    ITPWifiInfo netInfo;
#endif
#elif defined(CFG_NET_ETHERNET)
    ITPEthernetInfo netInfo;
#endif
#if defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6181)

    unsigned char cMac [6];
    uint32_t nIp;
    char msg_wifi[16];
    WifiMgr_Get_MAC_Address(cMac);
    nIp = htonl(mhd_sta_ipv4_ipaddr());

#endif

#if defined(CFG_NET_WIFI_SDIO_NGPL_ATBM6031)
    unsigned char cMac [6];
    uint32_t nIp;
    char msg_wifi[16];
    WifiMgr_Get_MAC_Address(cMac);
    nIp = htonl(WifiMgr_Get_WIFI_IP());
#endif

    char buf[32];
    char ip[16] = {0};

    if (!settingSysInfoIPAddrText)
    {
        settingSysInfoIPAddrText = ituSceneFindWidget(&theScene, "settingSysInfoIPAddrText");
        assert(settingSysInfoIPAddrText);

        settingSysInfoMacText = ituSceneFindWidget(&theScene, "settingSysInfoMacText");
        assert(settingSysInfoMacText);

        settingSysInfoHWVersionText = ituSceneFindWidget(&theScene, "settingSysInfoHWVersionText");
        assert(settingSysInfoHWVersionText);

        settingSysInfoSWVersionText = ituSceneFindWidget(&theScene, "settingSysInfoSWVersionText");
        assert(settingSysInfoSWVersionText);
    }

#ifdef CFG_NET_WIFI
#if defined(CFG_NET_WIFI_SDIO_VND_RTK)
	ipaddr_ntoa_r((const ip_addr_t*)ip_r, ip, sizeof(ip));
#elif defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6181)
    //ipaddr_ntoa_r((const ip_addr_t*)msg_wifi, ip, sizeof(ip));
    sprintf(ip, "%u.%u.%u.%u",FORMAT_IPADDR(nIp));
#elif defined(CFG_NET_WIFI_SDIO_NGPL_ATBM6031)
    sprintf(ip, "%u.%u.%u.%u",FORMAT_IPADDR(nIp));
#else
    ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_GET_INFO, &netInfo);
    ipaddr_ntoa_r((const ip_addr_t*)&netInfo.address, ip, sizeof(ip));
#endif
#elif defined(CFG_NET_ETHERNET)
    netInfo.index = 0;
    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &netInfo);
    ipaddr_ntoa_r((const ip_addr_t*)&netInfo.address, ip, sizeof(ip));
#else
    strcpy(ip, theConfig.ipaddr);
#endif // CFG_NET_WIFI

    ituTextSetString(settingSysInfoIPAddrText, ip);

    buf[0] = '\0';
#ifdef CFG_NET_ENABLE
#if defined(CFG_NET_WIFI_SDIO_VND_RTK) || \
    defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || \
    defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || \
    defined(CFG_NET_WIFI_SDIO_NGPL_AP6181)
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
        (uint8_t)cMac[0],
        (uint8_t)cMac[1],
        (uint8_t)cMac[2],
        (uint8_t)cMac[3],
        (uint8_t)cMac[4],
        (uint8_t)cMac[5]);
#else
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
        (uint8_t)netInfo.hardwareAddress[0],
        (uint8_t)netInfo.hardwareAddress[1],
        (uint8_t)netInfo.hardwareAddress[2],
        (uint8_t)netInfo.hardwareAddress[3],
        (uint8_t)netInfo.hardwareAddress[4],
        (uint8_t)netInfo.hardwareAddress[5]);
#endif
#endif // CFG_NET_ENABLE
    ituTextSetString(settingSysInfoMacText, buf);

    ituTextSetString(settingSysInfoHWVersionText, CFG_HW_VERSION);
    ituTextSetString(settingSysInfoSWVersionText, CFG_VERSION_MAJOR_STR "." CFG_VERSION_MINOR_STR "." CFG_VERSION_PATCH_STR "." CFG_VERSION_CUSTOM_STR "." CFG_VERSION_TWEAK_STR);

    return true;
}

void SettingSysInfoReset(void)
{
    settingSysInfoIPAddrText = NULL;
}
