#include <sys/ioctl.h>
#include <pthread.h>
#include "network_config.h"


/* Eason Refined in Mar. 2020 */

/////////////////////////////////////////////////
#if (TEST_PING_ETH || TEST_PING_WIFI)
#include "ping.h"

void ping_main(void)
{
   printf("====>ping test\n");
   ping_set_target("172.217.160.78"); //midea(101.37.128.72), google(172.217.160.78)
   ping_init();
}
#endif
/////////////////////////////////////////////////


/////////////////////////////////////////////////

#if TEST_SCAN_AP
static WIFI_MGR_SCANAP_LIST pList[64];
static int link_loop_time = 0;
void scan_ap_loop(void)
{
    int get_scan_count = 0;
	sleep(test_scan_interval);

	//ioctl(ITP_DEVICE_WIFI_NGPL, ITP_IOCTL_SCAN, NULL);
	get_scan_count = WifiMgr_Get_Scan_AP_Info(pList);
    printf("====>Scan count(%d)\n", get_scan_count);
};
#endif
/////////////////////////////////////////////////


/////////////////////////////////////////////////
#if TEST_CHANGE_AP
static unsigned int link_loop_time = 0;
void link_differnet_ap(void)
{
    ++link_loop_time;
    if((link_loop_time % (change_ap_time*2)) == 0){
        WifiMgr_Sta_Disconnect();
        usleep(500*1000);
        WifiMgr_Sta_Connect("SSID_1", "Password_1", ITE_WIFI_SEC_WPA_WPA2_MIXED);
    }else if((link_loop_time % (change_ap_time*2)) == change_ap_time){
        WifiMgr_Sta_Disconnect();
        usleep(500*1000);
        WifiMgr_Sta_Connect("SSID_2", "Password_2", ITE_WIFI_SEC_WPA_WPA2_MIXED);
    }else{
        printf("====>link_loop_time: %d\n", link_loop_time);
    }
};
#endif
/////////////////////////////////////////////////


/////////////////////////////////////////////////
#if TEST_WIFI_DOWNLOAD
#include "curl/curl.h"

struct FtpBuf
{
    uint8_t     *buf;
    uint32_t    pos;
};

static size_t FtpWrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpBuf   *out = (struct FtpBuf *)stream;
    size_t          s;
/*
    LOG_DEBUG "FtpWrite(0x%X,%d,%d,0x%X)\n", buffer, size, nmemb, stream LOG_END
 */
    // assert(out->buf);

    s           = size * nmemb;
    memcpy(&out->buf[out->pos], buffer, s);
    out->pos    += s;
    if (out->pos > 510000)
    {
        printf("FtpWrite(0x%X,%d,%d,0x%X)\n", buffer, size, nmemb, stream);
        out->pos = 0;
    }

    return s;
}

static int httpDownload(void)
{
    CURL            *curl;
    CURLcode        res;
    struct FtpBuf   ftpBuf;

    //InitNetwork();
    printf("[httpDownload] download : start\r\n");

    ftpBuf.buf  = malloc(0x200000);
    ftpBuf.pos  = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl        = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto error;
    }

    curl_easy_setopt(   curl,   CURLOPT_URL,            HTTP_DOWNLOAD_ADDRESS);
    //curl_easy_setopt(curl, CURLOPT_USERPWD, FTP_USER_NAME_PASSWORD);
    curl_easy_setopt(   curl,   CURLOPT_WRITEFUNCTION,  FtpWrite);
    curl_easy_setopt(   curl,   CURLOPT_WRITEDATA,      &ftpBuf);

    #ifndef NDEBUG
    curl_easy_setopt(   curl,   CURLOPT_VERBOSE,        1L);
    #endif

    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    if (CURLE_OK != res)
    {
        printf("curl fail: %d\n", res);
        goto error;
    }

    curl_global_cleanup();

    if (ftpBuf.buf)
    {
        free(ftpBuf.buf);
        ftpBuf.buf = NULL;
    }
    return 0;

error:
    curl_global_cleanup();
    if (ftpBuf.buf)
    {
        free(ftpBuf.buf);
        ftpBuf.buf = NULL;
    }
    return -1;
}

static void* TestHTTPDownload(void* arg)
{
    int nRet;
    int bIsAvail;
    do
    {
        //nRet = WifiMgr_Get_Sta_Work_State(&bIsAvail);
        if (WifiMgr_Get_Sta_Connect_Complete())
        {
            httpDownload();
        }
        usleep( 2000000);
        usleep( 2000000);
        printf("ready to download \n\n");
        usleep(2000000);
    } while (1);
}

int createHttpThread(void)
{
    pthread_t       task;
    pthread_attr_t  attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, (255 * 1024));
    pthread_create(&task, &attr, TestHTTPDownload, NULL);
    return 0;
}

#endif
/////////////////////////////////////////////////

