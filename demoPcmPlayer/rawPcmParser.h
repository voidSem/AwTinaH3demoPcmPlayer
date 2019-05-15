#ifndef         RAW_PCM_PARSER
#define         RAW_PCM_PARSER

#include <vdecoder.h>
#include <CdxParser.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define         MAX_BUFFER_SIZE ( 1 << 20)

    /**************ms**************/
#define         SAMPLE_INTERVAL (20)
    typedef enum PcmParserStatus {
        RAW_PCM_UNKNOWN,
        RAW_PCM_INITIALIZED,
        RAW_PCM_IDLE,
        RAW_PCM_PREFETCHING,
        RAW_PCM_PREFETCHED,
        RAW_PCM_SEEKING,
        RAW_PCM_READING,
    }rawPcmStatus_t;

    typedef struct rawPcmParser{
        FILE *pcmFp;
        rawPcmStatus_t status;
        int64_t fileSize;
        /*ms*/
        int mSampleInterval;
        int mUnitSize;
        unsigned char *mBuffer;
        CdxPlaybkCfg mPlaybkcfg;
    }rawPcmParserT;

/*
 * user call this first
 * input: file and rawPcmParserT
 *
 * */
extern int RawPcmParserInit(rawPcmParserT *p, const char *file);

/*
 * get video info
 * success return 0 else less than 0
 * */
extern int RawPcmParserGetCfg(rawPcmParserT *p, CdxPlaybkCfg *pCfg);

/*
 * read file data
 * success return 0 else less than 0
 * */
extern int RawPcmParserPrefetch(rawPcmParserT *p);

/*
 * get frame data
 * success return 0 else less than 0
 * */
extern unsigned char *RawPcmParserRead(rawPcmParserT *p);


/*
 * must call this when exit
 * close file fp
 *
 */
extern int RawPcmParserDestroy(rawPcmParserT *p);

#ifdef  __cplusplus
}
#endif

#endif /*RAW_PCM_PARSER*/

