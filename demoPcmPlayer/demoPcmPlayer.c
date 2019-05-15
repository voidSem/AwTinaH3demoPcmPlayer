/*
 *
 * File : demoPcmPlayer.c
 * Description :demoPcmPlayer
 * History :
 *
 */
#define LOG_TAG "demoPcmPlayer"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include "cdx_config.h"
#include <cdx_log.h>
#include <CdxParser.h>

/********************/
#include <tinasoundcontrol.h>
#include <rawPcmParser.h>
/*******************/

#define PCM_FILE_KEY            (".pcm")

#define DEMO_FILE_NAME_LEN      (1024)

#define DEMO_AUDIO_ERROR        (1 << 1)
#define DEMO_AUDIO_EXIT         (1 << 2)



typedef struct PcmPlayerDemo
{
    char *pInputFile;
    char *pOutputFile;
    /*********************/
    SoundCtrl *pSoundCtrl;
    rawPcmParserT mRawPcmParser;
    int64_t mPlayFrame;
    CdxPlaybkCfg mPlaybkCfg;
    /*********************/

}PcmPlayerDemo;

typedef enum {
    INPUT,
    HELP,
    INVALID
}ARGUMENT_T;

typedef struct {
    char Short[16];
    char Name[128];
    ARGUMENT_T argument;
    char Description[512];
}argument_t;

static const argument_t ArgumentMapping[] =
{
    { "-h",  "--help",    HELP,
        "Print this help" },
    { "-i",  "--input",   INPUT,
        "Input file" },
};

static void PrintDemoUsage(void)
{
    int i = 0;
    int num = sizeof(ArgumentMapping) / sizeof(argument_t);
    logd("Usage:");
    while(i < num)
    {
        logd("%s %-32s  %s", ArgumentMapping[i].Short, ArgumentMapping[i].Name,
                ArgumentMapping[i].Description);
        i++;
    }
}

ARGUMENT_T GetArgument(char *name)
{
    int i = 0;
    int num = sizeof(ArgumentMapping) / sizeof(argument_t);
    while(i < num)
    {
        if((0 == strcmp(ArgumentMapping[i].Name, name)) ||
            ((0 == strcmp(ArgumentMapping[i].Short, name)) &&
             (0 != strcmp(ArgumentMapping[i].Short, "--"))))
        {
            return ArgumentMapping[i].argument;
        }
        i++;
    }
    return INVALID;
}

void ParseArgument(PcmPlayerDemo *demoPlayer, char *argument, char *value)
{
    ARGUMENT_T arg;
    int len = 0;
    if(len > DEMO_FILE_NAME_LEN)
        return;
    arg = GetArgument(argument);
    switch(arg)
    {
        case HELP:
            PrintDemoUsage();
            exit(-1);
        case INPUT:
            sscanf(value, "%1024s", demoPlayer->pInputFile);
            logd(" get input file: %s ", demoPlayer->pInputFile);
            break;
        case INVALID:
        default:
            logd("unknowed argument :  %s", argument);
            break;
    }
}


/*pcm audio player run*/
void *audioThread(void* param)
{
    PcmPlayerDemo *pPlayer = (PcmPlayerDemo *)param;
    rawPcmParserT *parser = &(pPlayer->mRawPcmParser);
    CdxPlaybkCfg *pCfg = &(pPlayer->mPlaybkCfg);

    int nRet = 0;
    pPlayer->mPlayFrame=0;

    /*****************************************************************/
    /*init parser*/
    if(RawPcmParserInit(parser, (const char*)pPlayer->pInputFile) < 0) {
        loge("init raw pcm parser failed");
        return NULL;
    }
    RawPcmParserGetCfg(parser, pCfg);

    /*init soundCtrl*/
    pPlayer->pSoundCtrl = TinaSoundDeviceInit();
    if(NULL == pPlayer->pSoundCtrl) {
        loge("init sound dev failed");
        goto audio_exit;
    }

    TinaSoundDeviceSetFormat(pPlayer->pSoundCtrl, pCfg);
    if(TinaSoundDeviceStart(pPlayer->pSoundCtrl) < 0) {
        loge("start sound dev failed");
        goto audio_exit;
    }

    /****************************************************************/
    loge("start run!");
    while ( (nRet = RawPcmParserPrefetch(parser)) > 0)
    {
        usleep(100);

        unsigned char *pcmData = RawPcmParserRead(parser);
        if(NULL == pcmData) {
            loge("read pcm data error ");
            goto audio_exit;
        }
        /*send pcm data to sound dev*/
        if(TinaSoundDeviceWrite(pPlayer->pSoundCtrl, (void*)pcmData, nRet) <= 0) {
            loge("write pcm data error ");
            goto audio_exit;
        }
    }
    logw("get pcm end");

audio_exit:
    /***************/
    if(pPlayer->pSoundCtrl) {
        loge("destroy sound dev and rawpcm parser");
        TinaSoundDeviceStop(pPlayer->pSoundCtrl);
        TinaSoundDeviceDestroy(pPlayer->pSoundCtrl);
    }
    RawPcmParserDestroy(parser);
    /*************/
    logw("exit..... ");
    return NULL;
}


void DemoHelpInfo(void)
{
    logd(" ==== CedarX linux decoder demo help start ===== \n"
        "-h or --help to show the demo usage\n"
        " demo created by voidsem, airfly\n"
        " email: liuxueneng@iairfly.com\n"
        " ===== CedarX linux pcm player demo help end ====== \n");
}

int main(int argc, char** argv)
{
    int nRet = 0;
    int i = 0;
    pthread_t audioPid;
    PcmPlayerDemo  demoPlayer;

    DemoHelpInfo();

    if(argc < 2) {
        loge(" we need more arguments ");
        PrintDemoUsage();
        return -1;
    }

    /*get memory*/
    char *pInputFile = (char *)calloc(DEMO_FILE_NAME_LEN, 1);
    if(pInputFile == NULL) {
        loge(" input file. calloc memory fail. ");
        return 0;
    }

    memset(&demoPlayer, 0, sizeof(PcmPlayerDemo));
    demoPlayer.pInputFile = pInputFile;

    /*parse arg*/
    for(i = 1; i < (int)argc; i += 2) {
        ParseArgument(&demoPlayer, argv[i], argv[i + 1]);
    }

    /*check file type*/
    if(NULL == strstr(demoPlayer.pInputFile, PCM_FILE_KEY)) {
        loge("not pcm file");
        goto END;
    }
    pthread_create(&audioPid, NULL, audioThread, (void*)(&demoPlayer));

    /*block wait*/
    pthread_join(audioPid, (void**)&nRet);
END:
    if(pInputFile != NULL)
        free(pInputFile);

    logw("demoPcmPlayer exit successful");
    return 0;
}
