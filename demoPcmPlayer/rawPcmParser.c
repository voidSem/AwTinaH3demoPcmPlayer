#define LOG_TAG         "rawParser"
#include                <rawPcmParser.h>


static CdxPlaybkCfg initPlybkCfg =
{
    .nRoutine = 0,
    .nNeedDirect = 0,
    .nChannels = 2,
    .nSamplerate = 44100,
    .nBitpersample = 16,
    .nDataType = AUDIO_RAW_DATA_PCM,
};

/*
 * user call this first
 * input: file and rawPcmParserT
 *
 * */
int RawPcmParserInit(rawPcmParserT *p, const char *file)
{
    if(NULL == p || NULL == file) {
        loge("parmer wrong p:%p, file:%p", p, file);
        return -1;
    }
    p->pcmFp = fopen(file, "rb");
    if(NULL == p->pcmFp) {
        loge("open %s failed %s",file, strerror(errno));
        return -2;
    }
    p->mBuffer = malloc(MAX_BUFFER_SIZE);
    if(NULL == p->mBuffer) {
        return -3;
    }

    fseek(p->pcmFp, 0, SEEK_END);
    p->fileSize = ftell(p->pcmFp);
    fseek(p->pcmFp, 0, SEEK_SET);
    p->mSampleInterval = SAMPLE_INTERVAL;

    p->status = RAW_PCM_IDLE;
    /*init cfg*/
    p->mPlaybkcfg = initPlybkCfg;
    p->mUnitSize = (p->mPlaybkcfg.nSamplerate *
                   (p->mPlaybkcfg.nBitpersample >> 3) *
                   p->mPlaybkcfg.nChannels *
                   p->mSampleInterval / 1000);
    loge("file:%s size:%lld ch:%d rate:%d bit:%d type:%d, unitSize:%d",
            file,p->fileSize, p->mPlaybkcfg.nChannels,
            p->mPlaybkcfg.nSamplerate,
            p->mPlaybkcfg.nBitpersample,
            p->mPlaybkcfg.nDataType,
            p->mUnitSize);
    return 0;
}

/*
 * get media info
 *
 *
 */
int RawPcmParserGetCfg(rawPcmParserT *p, CdxPlaybkCfg *pCfg)
{
    if(NULL == p || NULL == pCfg) {
        loge("parmer wrong p:%p pCfg:%p", p, pCfg);
        return -1;
    }

    if(p->status == RAW_PCM_UNKNOWN) {
        loge("status unknown");
        return -2;
    }

    /*set cfg*/
    *pCfg = p->mPlaybkcfg;
    return 0;
}

/*
 * prefet data
 * return data len
 *
 */

int RawPcmParserPrefetch(rawPcmParserT *p)
{
    if(NULL == p || NULL == p->mBuffer) {
        loge("parmer wrong p:%p", p);
        return -1;
    }

    if(p->status != RAW_PCM_IDLE) {
        loge("status not idel");
        return -2;
    }
    p->status = RAW_PCM_PREFETCHING;
    int len = fread(p->mBuffer, 1, p->mUnitSize, p->pcmFp);
    //if (len != p->mUnitSize) {
    if (len <= 0) {
        loge("read ret:(%d)%s",len, strerror(errno));
        return -1;
    }
    p->status = RAW_PCM_PREFETCHED;
    return len;
}

/*get pcm data*/
unsigned char* RawPcmParserRead(rawPcmParserT *p)
{
    if(NULL == p) {
        loge("parmer wrong p:%p", p);
        return NULL;
    }
    if(p->status != RAW_PCM_PREFETCHED) {
        loge("have not prefetched");
        return NULL;
    }
    p->status = RAW_PCM_IDLE;

    return p->mBuffer;
}


/*
 * must call this when exit
 * close file fp
 *
 */
int RawPcmParserDestroy(rawPcmParserT *p)
{
    if( p ) {
        if ( p->pcmFp) {
            fclose(p->pcmFp);
        }
        if(p->mBuffer)
            free(p->mBuffer);
        p->status = RAW_PCM_UNKNOWN;
        return 0;
    }
    return -1;
}

