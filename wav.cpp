/*
 * wav.cpp
 * 
 * @author Matthew Haney
 */

#include <string>
#include <fstream>
#include <iostream>

#include "wav.hpp"

using namespace std;

template<typename ChunkType>
WaveChunk<ChunkType>::WaveChunk(uint32_t i, uint32_t s, ChunkType d) {
  id = i;
  size = s; /* size is in bytes */
  /* deep copy of data */
  data = (ChunkType)malloc(size);
  memcpy(data, d, size);
}

template<typename ChunkType>
WaveChunk<ChunkType>::~WaveChunk() {
  if (data != NULL) {
    cout << "WC d" << endl;
    free(data);
  }
  data = NULL;
}

template<typename ChunkType>
WaveChunk<ChunkType>::WaveChunk(const WaveChunk & old) {
  this->id = old.id;
  this->size = old.size;
  this->data = (ChunkType)malloc(this->size);
  memcpy(this->data, old.data, this->size);
}

RiffHeader::RiffHeader() {
  id = WavChunkIDs::RiffHeader;
  size = 0;
  data = (RiffHeaderChunkData_t)malloc(sizeof(RiffHeaderChunkData));
  data->format = WavChunkIDs::WavRiff;
}

RiffHeader::~RiffHeader() {
  if (data != NULL) {
    cout << "RH d" << endl;
    free(data);
  }
  data = NULL;
}

WaveFile::WaveFile() { }

WavError WaveFile::read(string infile) {
  ifstream fin;
  fin.open(infile.c_str(), ios_base::in | ios_base::binary);
  if (!fin.is_open()) {
    return WAV_BAD_INFILE;
  }
  uint32_t size = 0, fmt = 0;
  uint32_t * data;
  fin.read((char*)&fmt, 4); /* = WavChunkIDs::RiffHeader */
  if (fmt != WavChunkIDs::RiffHeader) return WAV_MALFORMED_INFILE;
  fin.read((char*)&size, 4); /* = total input size */
  riff_header.size = size;
  fin.read((char*)&fmt, 4); /* = WavChunkIDs::WavRiff */
  if (fmt != WavChunkIDs::WavRiff) return WAV_MALFORMED_INFILE;
  /* Tricky part: iterate over data chunks */
  while (fin.good()) {
    fin.read((char*)&fmt, 4); /* Format code */
    fin.read((char*)&size, 4); /* Chunk size */
    /* Read in chunk data */
    data = (uint32_t*)malloc(4 * size);
    fin.read((char*)data, 4 * size);
    /* Make new chunk */
    chunks.push_back(WaveChunk<DataChunkData_t>(fmt, size, data));
    free(data);
    cout << "Made chunk with ID " << hex << fmt << dec << endl;
    switch (fmt) {
    case WavChunkIDs::Format:
    case WavChunkIDs::Fact:
    case WavChunkIDs::LabeledText:
    case WavChunkIDs::Sample:
    case WavChunkIDs::Data:
    case WavChunkIDs::Junk:
      break;
    case WavChunkIDs::RiffHeader:
    case WavChunkIDs::WavRiff:
      return WAV_ILLEGAL_CHUNK;
    default:
      cout << fmt << endl;
      return WAV_UNKNOWN_CHUNK;
    }
  }
  return WAV_SUCCESS;
}

WaveFile::~WaveFile() { }

WavError WaveFile::write(string outfile) {
  ofstream fout;
  fout.open(outfile.c_str(), ios_base::out | ios_base::binary | ios_base::trunc);
  if (!fout.is_open()) {
    return WAV_BAD_OUTFILE;
  }
  

  return WAV_SUCCESS;
}

ostream & operator<<(ostream & os, WaveFile wav) {
  os << "WAVE/RIFF file size " << wav.riff_header.size << endl;
  for (vector<WaveChunk<DataChunkData_t> >::size_type i = 0; i != wav.chunks.size(); i++) {
    os << "  Chunk " << i << ": ";
    switch (wav.chunks[i].id) {
    case WavChunkIDs::Format:
      os << "format" << endl;
      
      break;
    case WavChunkIDs::Fact:
      os << "fact" << endl;
      break;
    case WavChunkIDs::LabeledText:
      os << "labeled text" << endl;
      break;
    case WavChunkIDs::Sample:
      os << "sample" << endl;
      break;
    case WavChunkIDs::Data:
      os << "data" << endl;
      break;
    case WavChunkIDs::Junk:
      os << "junk" << endl;
      break;
    case WavChunkIDs::RiffHeader: /* should never be hit */
      os << "riff header (WARNING: misplaced chunk)" << endl;
      os << "  total size: " << wav.chunks[i].size;
      break;
    case WavChunkIDs::WavRiff: /* should never be hit */
      os << "wave/riff (WARNING: misplaced chunk)" << endl;
      break;
    default:
      os << "unrecognized format 0x" << hex << wav.chunks[i].id << dec << endl;
      break;
    }
  }
  //  wav.chunks
  return os;
}

