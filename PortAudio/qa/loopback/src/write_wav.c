 
#include <stdio.h>
#include <stdlib.h>
#include "write_wav.h"
static void WriteLongLE( unsigned char **addrPtr, unsigned long data )
{
unsigned char *addr = *addrPtr;
*addr++ =  (unsigned char) data;
*addr++ =  (unsigned char) (data>>8);
*addr++ =  (unsigned char) (data>>16);
*addr++ =  (unsigned char) (data>>24);
*addrPtr = addr;
}
static void WriteShortLE( unsigned char **addrPtr,  unsigned short data )
{
unsigned char *addr = *addrPtr;
*addr++ =  (unsigned char) data;
*addr++ =  (unsigned char) (data>>8);
*addrPtr = addr;
}
static void WriteChunkType( unsigned char **addrPtr, unsigned long cktyp )
{
unsigned char *addr = *addrPtr;
*addr++ =  (unsigned char) (cktyp>>24);
*addr++ =  (unsigned char) (cktyp>>16);
*addr++ =  (unsigned char) (cktyp>>8);
*addr++ =  (unsigned char) cktyp;
*addrPtr = addr;
}
#define WAV_HEADER_SIZE (4 + 4 + 4 +   \
4 + 4 + 16 +   \
4 + 4 )  
long Audio_WAV_OpenWriter( WAV_Writer *writer, const char *fileName, int frameRate, int samplesPerFrame )
{
unsigned int  bytesPerSecond;
unsigned char header[ WAV_HEADER_SIZE ];
unsigned char *addr = header;
int numWritten;
writer->dataSize = 0;
writer->dataSizeOffset = 0;
writer->fid = fopen( fileName, "wb" );
if( writer->fid == NULL )
{
return -1;
}
WriteChunkType( &addr, RIFF_ID );
WriteLongLE( &addr, 0 );
WriteChunkType( &addr, WAVE_ID );
WriteChunkType( &addr, FMT_ID );
WriteLongLE( &addr, 16 );
WriteShortLE( &addr, WAVE_FORMAT_PCM );
bytesPerSecond = frameRate * samplesPerFrame * sizeof( short);
WriteShortLE( &addr, (short) samplesPerFrame );
WriteLongLE( &addr, frameRate );
WriteLongLE( &addr,  bytesPerSecond );
WriteShortLE( &addr, (short) (samplesPerFrame * sizeof( short)) );  
WriteShortLE( &addr, (short) 16 );  
WriteChunkType( &addr, DATA_ID );
writer->dataSizeOffset = (int) (addr - header);
WriteLongLE( &addr, 0 );
numWritten = fwrite( header, 1, sizeof(header), writer->fid );
if( numWritten != sizeof(header) ) return -1;
return (int) numWritten;
}
long Audio_WAV_WriteShorts( WAV_Writer *writer,
short *samples,
int numSamples
)
{
unsigned char buffer[2];
unsigned char *bufferPtr;
int i;
short *p = samples;
int numWritten;
int bytesWritten;
if( numSamples <= 0 )
{
return -1;
}
for( i=0; i<numSamples; i++ )
{
bufferPtr = buffer;
WriteShortLE( &bufferPtr, *p++ );
numWritten = fwrite( buffer, 1, sizeof( buffer), writer->fid );
if( numWritten != sizeof(buffer) ) return -1;
}
bytesWritten = numSamples * sizeof(short);
writer->dataSize += bytesWritten;
return (int) bytesWritten;
}
long Audio_WAV_CloseWriter( WAV_Writer *writer )
{
unsigned char buffer[4];
unsigned char *bufferPtr;
int numWritten;
int riffSize;
int result = fseek( writer->fid, writer->dataSizeOffset, SEEK_SET );
if( result < 0 ) return result;
bufferPtr = buffer;
WriteLongLE( &bufferPtr, writer->dataSize );
numWritten = fwrite( buffer, 1, sizeof( buffer), writer->fid );
if( numWritten != sizeof(buffer) ) return -1;
result = fseek( writer->fid, 4, SEEK_SET );
if( result < 0 ) return result;
riffSize = writer->dataSize + (WAV_HEADER_SIZE - 8);
bufferPtr = buffer;
WriteLongLE( &bufferPtr, riffSize );
numWritten = fwrite( buffer, 1, sizeof( buffer), writer->fid );
if( numWritten != sizeof(buffer) ) return -1;
fclose( writer->fid );
writer->fid = NULL;
return writer->dataSize;
}
#if 0
int main( void )
{
int i;
WAV_Writer writer;
int result;
#define NUM_SAMPLES  (200)
short data[NUM_SAMPLES];
short saw = 0;
for( i=0; i<NUM_SAMPLES; i++ )
{
data[i] = saw;
saw += 293;
}
result =  Audio_WAV_OpenWriter( &writer, "rendered_midi.wav", 44100, 1 );
if( result < 0 ) goto error;
for( i=0; i<15; i++ )
{
result =  Audio_WAV_WriteShorts( &writer, data, NUM_SAMPLES );
if( result < 0 ) goto error;
}
result =  Audio_WAV_CloseWriter( &writer );
if( result < 0 ) goto error;
return 0;
error:
printf("ERROR: result = %d\n", result );
return result;
}
#endif
