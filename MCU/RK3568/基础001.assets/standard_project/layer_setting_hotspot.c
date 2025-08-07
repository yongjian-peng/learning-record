#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"
#ifdef CFG_NET_WIFI
#include "wifiMgr.h"
#endif

static ITUSprite* settingHotspotSprite;
static ITUBackground* settingHotspotGrayBackground;
static ITURadioBox* settingHotspotDisableRadioBox;
static ITURadioBox* settingHotspotEnableRadioBox;

#ifdef CFG_NET_WIFI
 WIFI_MGR_SETTING gWifiSetting;
#endif

extern bool disableHotspot = true;

void SettingHotspotSwitch(bool disableHotspot)
{
	int ret;
#ifdef CFG_NET_WIFI
	
	if (disableHotspot == false){
		// Enable hotspot	
		strcpy(gWifiSetting.ssid,theConfig.ap_ssid);
		strcpy(gWifiSetting.password,theConfig.ap_password);
		printf("SettingHotspotSwitch Enable hotspot %s %s \n",gWifiSetting.ssid,gWifiSetting.password);

      } else {
		strcpy(gWifiSetting.ssid,theConfig.ssid);
		strcpy(gWifiSetting.password,theConfig.password);
		printf("SettingHotspotSwitch Disable hotspot %s %s \n",gWifiSetting.ssid,gWifiSetting.password);
		
      	}
	ret = WifiMgr_Sta_HostAP_Switch(gWifiSetting);
#endif    
}

bool SettingHotspotOnEnter(ITUWidget* widget, char* param)
{
	if(!settingHotspotSprite)
	{
		settingHotspotSprite = ituSceneFindWidget(&theScene, "settingHotspotSprite");
        assert(settingHotspotSprite);
		
		settingHotspotGrayBackground = ituSceneFindWidget(&theScene, "settingHotspotGrayBackground");
        assert(settingHotspotGrayBackground);
		
		settingHotspotDisableRadioBox = ituSceneFindWidget(&theScene, "settingHotspotDisableRadioBox");
        assert(settingHotspotDisableRadioBox);
		
		settingHotspotEnableRadioBox = ituSceneFindWidget(&theScene, "settingHotspotEnableRadioBox");
        assert(settingHotspotEnableRadioBox);
		
	}
	if(disableHotspot)
	{
		ituRadioBoxSetChecked(settingHotspotDisableRadioBox, true);
		ituSpriteGoto(settingHotspotSprite, 0);
	}
	else
	{
		ituRadioBoxSetChecked(settingHotspotEnableRadioBox, true);
		ituSpriteGoto(settingHotspotSprite, 1);
	}
    return true;
}

bool SettingHotspotRadioBoxOnPress(ITUWidget* widget, char* param)
{
	if (ituRadioBoxIsChecked(settingHotspotDisableRadioBox))
    {
		//Disable hotspot
		disableHotspot = true;
        ituSpriteGoto(settingHotspotSprite, 0);
        SettingHotspotSwitch(true);
    }
	else if (ituRadioBoxIsChecked(settingHotspotEnableRadioBox))
    {
        //Enable hotspot
		disableHotspot = false;
        ituSpriteGoto(settingHotspotSprite, 1);       
        SettingHotspotSwitch(false);
    }
    
    return true;
}

