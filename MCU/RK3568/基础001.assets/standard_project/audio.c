#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "ite/audio.h"
#include "ctrlboard.h"

#ifdef __OPENRTOS__
#include "flower.h"
IteFlower *flower = NULL;
#endif
 
extern bool                       audioKeySoundPaused;
 
void AudioInit(void)
{
    #ifdef __OPENRTOS__
    audioKeySoundPaused = false;
    flower = IteStreamInit();
    flow_init_AD();
    flow_init_DA();
    flow_set_dac_level(50);
    flow_set_adc_level(50);
    #endif
}

void AudioExit(void){
    
}

int AudioPlayMusic(char* filename, AudioPlayCallback func)
{
    #ifdef __OPENRTOS__
    flow_set_dac_level(theConfig.audiolevel);
    //flow_start_quick_play(flower,filename,Normal,func);
    flow_start_sound_play(flower,filename,Normal,func);
    #endif
    return 0;
}

void AudioPlayKeySound(void)
{
    #ifdef __OPENRTOS__
    char filepath[PATH_MAX];
    
    if(audioKeySoundPaused)
        return;
 
    strcpy(filepath, CFG_PRIVATE_DRIVE ":/sounds/");
    strcat(filepath, theConfig.keysound);
    flow_set_dac_level(theConfig.keylevel);
    flow_start_sound_play(flower,filepath,Normal,NULL);
    #endif
}
 
void AudioStop(void)
{
    #ifdef __OPENRTOS__
    //flow_stop_quick_play(flower);
    flow_stop_sound_play(flower);
    #endif
}
 
void AudioPauseKeySound(void)
{
    audioKeySoundPaused = true;
}
 
void AudioResumeKeySound(void)
{
    audioKeySoundPaused = false;
}
 
void AudioSetKeyLevel(int level)
{
    #ifdef __OPENRTOS__
    theConfig.keylevel = level;
    flow_set_dac_level(theConfig.keylevel);
    #endif
}
 
void AudioMute(void)
{
}
 
void AudioUnMute(void)
{
}
 
bool AudioIsMuted(void)
{
}
 
bool AudioIsPlaying(void)
{
}
 
void AudioSetVolume(int level)
{
    #ifdef __OPENRTOS__
    theConfig.audiolevel = level;
    flow_set_dac_level(theConfig.audiolevel);
    #endif
}
 
void AudioSetLevel(int level)
{
    theConfig.audiolevel = level; 
}
 
int AudioGetVolume(void)
{
    return theConfig.audiolevel;
}
 
void AudioSetRecLevel(int level)
{

}


unsigned int AudioGetDecodeTime(
    void)
{
    // unit : ms
    uint32_t time;
    #ifdef __OPENRTOS__
    if(flower->audiocase==AudioPlayFile)
        PlayFlowGetTime(flower,&time);
    else{
        //if(flower->audiocase==QuickPlay)
        iteAudioGetDecodeTime((uint32_t *)&time);
    }
    #endif
    return time;
} 
