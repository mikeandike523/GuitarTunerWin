 
#ifndef _WAV_WRITER_H
#define _WAV_WRITER_H
#ifdef __cplusplus
extern "C" {
#endif
#define RIFF_ID   (('R'<<24) | ('I'<<16) | ('F'<<8) | 'F')
#define WAVE_ID   (('W'<<24) | ('A'<<16) | ('V'<<8) | 'E')
#define FMT_ID    (('f'<<24) | ('m'<<16) | ('t'<<8) | ' ')
#define DATA_ID   (('d'<<24) | ('a'<<16) | ('t'<<8) | 'a')
#define FACT_ID   (('f'<<24) | ('a'<<16) | ('c'<<8) | 't')
#define WAV_ERR_CHUNK_SIZE     (-1)    
#define WAV_ERR_FILE_TYPE      (-2)    
#define WAV_ERR_ILLEGAL_VALUE  (-3)    
#define WAV_ERR_FORMAT_TYPE    (-4)    
#define WAV_ERR_TRUNCATED      (-5)    
#define WAVE_FORMAT_PCM        (1)
#define WAVE_FORMAT_IMA_ADPCM  (0x0011)
typedef struct WAV_Writer_s
{
FILE *fid;
int   dataSizeOffset;
int   dataSize;
} WAV_Writer;
long Audio_WAV_OpenWriter( WAV_Writer *writer, const char *fileName, int frameRate, int samplesPerFrame );
long Audio_WAV_WriteShorts( WAV_Writer *writer,
short *samples,
int numSamples
);
long Audio_WAV_CloseWriter( WAV_Writer *writer );
#ifdef __cplusplus
};
#endif
#endif  
