/*
 * wav.h
 *
 * @author Matthew Haney
 */

#ifndef _WAV_HPP_
#define _WAV_HPP_

#include <string>
#include <vector>
#include <iostream>

#include <stdint.h>
#include <stddef.h>

/* save me v */

/****************************************
 * 
 ****************************************/

/**********************************
 * WAVE/RIFF big-endian chunk IDs *
 **********************************/

namespace WavChunkIDs {
  const uint32_t RiffHeader = 0x46464952;
  const uint32_t WavRiff = 0x45564157;
  const uint32_t Format = 0x20746d66;
  const uint32_t LabeledText = 0x478747C6;
  //const uint32_t Instrumentation = 0x478747C6; /* same as LabeledText */
  const uint32_t Sample = 0x6C706D73;
  const uint32_t Fact = 0x47361666;
  const uint32_t Data = 0x61746164;
  const uint32_t Junk = 0x4b4e554a;
};

/**************************
 * Wave file format codes *
 **************************/

namespace WavFormat {
  const uint16_t PCM = 0x0001;          /* PCM */
  const uint16_t IEEEFloat = 0x0003;    /* IEEE float */
  const uint16_t Alaw = 0x0006;         /* 8-bit ITU-T G.711 A-law */
  const uint16_t Mulaw = 0x0007;        /* 8-bit ITU-T G.711 mu-law */
  const uint16_t Extensible = 0xfffe;   /* Determined by sub format */
};

/***********************
 * Success/error codes *
 ***********************/

enum WavError {
  WAV_SUCCESS,
  WAV_BAD_OUTFILE,
  WAV_BAD_INFILE,
  WAV_MALFORMED_INFILE,
  WAV_UNKNOWN_FORMAT,
  WAV_UNKNOWN_CHUNK,
  WAV_MALFORMED_CHUNK,
  WAV_ILLEGAL_CHUNK
};

/**********************************
 * Base Wave Chunk template class *
 **********************************/

template <typename ChunkType>
class WaveChunk {
public:
  uint32_t id;
  uint32_t size;
  ChunkType data;
  WaveChunk() { id = 0; size = 0; data = NULL; }
  WaveChunk(uint32_t id, uint32_t size, ChunkType data);
  WaveChunk(uint32_t id, ChunkType data); /* auto-calculate size */
  WaveChunk(const WaveChunk & old); /* copy constructor */
  ~WaveChunk();
};

/**************************************************
 * Different structures for different chunk types *
 **************************************************/

typedef struct {
  uint32_t format; /* should always be WavChunkIDs::WavRiff */
} RiffHeaderChunkData;

typedef RiffHeaderChunkData* RiffHeaderChunkData_t;

typedef struct {
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  /* below may be invalid - check size (16, 18, 40)*/
  uint16_t extension_size;     /* should be 0 if size is 18; don't read if size is 16 */
  uint16_t valid_bits;         /* only read if size is 40 */
  uint32_t channel_mask;       /* only read if size is 40 */
  uint32_t sub_format[4];      /* only read if size is 40 */
} FormatChunkData;

typedef FormatChunkData* FormatChunkData_t;

typedef void* DataChunkData_t;

typedef struct {
  uint32_t sample_length;
} FactChunkData;

typedef FactChunkData* FactChunkData_t;

/*************************************
 * Classes for different chunk types *
 *************************************/

class RiffHeader : public WaveChunk<RiffHeaderChunkData_t> {
public:
  RiffHeader();
  ~RiffHeader();
};

class WaveFile {
public:
  RiffHeader riff_header;
  std::vector<WaveChunk<DataChunkData_t> > chunks;
  WaveFile();                                    /* bare init */
  ~WaveFile();                                   /* safely destroy chunk array */
  WavError read(std::string filename);           /* read from filename */
  WavError write(std::string filename);          /* write to filename */
  template <typename from_t, typename to_t>
  WavError data_transform(to_t (* fn)(from_t));  /* perform fn on all elements of data */
  friend std::ostream & operator<<(std::ostream & os, WaveFile wav);
};

template <typename from_t, typename to_t>
WavError WaveFile::data_transform(to_t (*fn)(from_t)) {
  /* iterate through chunks */
  /** on non-data chunks: */
  /*** continue */
  /** make new data buffer of to_t elements */
  /** perform fn on current data to populate new buffer */
  /** recalculate data chunk size */
  
  /* recalculate total size */

  return WAV_SUCCESS;
}


#endif
