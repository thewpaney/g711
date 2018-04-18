#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
using namespace std;

extern "C" {
#include "g711.h"
}

/* see: https://stackoverflow.com/questions/16075233/reading-and-processing-wav-file-data-in-c-c */
/* also helpful: http://soundfile.sapp.org/doc/WaveFormat/ */
namespace WavChunkIDs {
  uint32_t RiffHeader = 0x46464952;
  uint32_t WavRiff = 0x45564157;
  uint32_t Format = 0x020746d66;
  uint32_t LabeledText = 0x478747C6;
  uint32_t Instrumentation = 0x478747C6;
  uint32_t Sample = 0x6C706D73;
  uint32_t Fact = 0x47361666;
  uint32_t Data = 0x61746164;
  uint32_t Junk = 0x4b4e554a;
};

void help() {
  cout << "usage: g711conv [-u|-a] infile outfile" << endl;
  cout << "Pass -u for u-law conversion and -a for a-law conversion." << endl;
  exit(1);
}

void wave_file_transform(string infile, string outfile, short (* fn)(unsigned char)) {
  /* Get input file object */
  ifstream fin;
  fin.open(infile.c_str(), ios_base::in | ios_base::binary);
  if (!fin.is_open()) {
    cout << "Error opening file " << infile << " for input" << endl;
    exit(2);
  }
  /* Get output file object */
  ofstream fout;
  fout.open(outfile.c_str(), ios_base::out | ios_base::binary | ios_base::trunc);
  if (!fout.is_open()) {
    cout << "Error opening file " << outfile << " for output" << endl;
    exit(2);
  }
  uint32_t hdr_data = 0, size = 0, inputNumSamples = 0;
  /* Read input metadata */
  fin.read((char*)&hdr_data, 4); /* = WavChunkIDs::RiffHeader */
  fin.read((char*)&hdr_data, 4); /* = total input size */
  fin.read((char*)&hdr_data, 4); /* = WavChunkIDs::WavRiff */
  fin.read((char*)&hdr_data, 4); /* = WavChunkIDs::Format */
  fin.read((char*)&size, 4); /* = size of format chunk */
  /* Skip all of sub-chunk 1 */
  while (size > 0) {
    fin.read((char*)&hdr_data, 1);
    size--;
  }
  fin.read((char*)&hdr_data, 4); /* = WavChunkIDs::Fact */
  fin.read((char*)&hdr_data, 4); /* = size of fact chunk (always 4) */
  fin.read((char*)&inputNumSamples, 4); /* = # of samples*/
  fin.read((char*)&hdr_data, 4); /* = WavChunkIDs::Data */
  fin.read((char*)&hdr_data, 4); /* = # of uncompressed samples (same in this case) */
  /* Next comes actual data! */
  uint8_t * input_buf = (uint8_t*)malloc(inputNumSamples);
  uint16_t * output_buf = (uint16_t*)malloc(2 * inputNumSamples);
  cout << inputNumSamples << endl;
  /* Read data */
  for (uint32_t i = 0; i < inputNumSamples; i++) {
    fin.read((char*)(input_buf + i), 1);
  }
  /* Do conversion */
  for (uint32_t i = 0; i < inputNumSamples; i++) {
    output_buf[i] = fn(input_buf[i]);
  }
  /* Output metadata */
  uint32_t outputFormatChunkSize = 16;
  uint16_t outputAudioFormat = 1; /* PCM = 1 */
  uint16_t outputNumChannels = 1; /* Mono */
  uint32_t outputSampleRate = 16000;
  uint32_t outputByteRate = 32000;
  uint16_t outputBlockAlign = 2;
  uint16_t outputBitsPerSample = 16;
  uint32_t outputDataSize = inputNumSamples * outputNumChannels * outputBitsPerSample / 8;
  uint32_t outputSizeTotal = 36 + outputDataSize;
  /* WAVE/RIFF Header */
  fout.write((char*)&(WavChunkIDs::RiffHeader), 4);
  fout.write((char*)&outputSizeTotal, 4);
  fout.write((char*)&(WavChunkIDs::WavRiff), 4);
  /* fmt header */
  fout.write((char*)&(WavChunkIDs::Format), 4);
  fout.write((char*)&outputFormatChunkSize, 4);
  fout.write((char*)&outputAudioFormat, 2);
  fout.write((char*)&outputNumChannels, 2);
  fout.write((char*)&outputSampleRate, 4);
  fout.write((char*)&outputByteRate, 4);
  fout.write((char*)&outputBlockAlign, 2);
  fout.write((char*)&outputBitsPerSample, 2);
  /* data header */
  fout.write((char*)&(WavChunkIDs::Data), 4);
  fout.write((char*)&outputDataSize, 4);
  /* actual data */
  for (uint32_t i = 0; i < inputNumSamples; i++) {
    fout.write((char*)(output_buf + i), 2);
  }
  /* clean up */
  free(input_buf);
  free(output_buf);
  fin.close();
  fout.close();
  return;
}

int main(int argc, char **argv) {
  char mode = 'a';
  string infile, outfile;
  if (argc < 3 || argc > 4) help();
  if (argc == 4) {
    if (strcmp(argv[1], "-u") == 0) {
      mode = 'u';
    } else if (strcmp(argv[1], "-a") == 0) {
      mode = 'a';
    } else {
      help();
    }
    infile = argv[2];
    outfile = argv[3];
  } else { /* argc == 3 */
    infile = argv[1];
    outfile = argv[2];
  }
  switch (mode) {
  case 'a':
    cout << "Starting A-law to PCM conversion..." << endl;
    wave_file_transform(infile, outfile, Snack_Alaw2Lin);
    cout << "Done." << endl;
    break;
  case 'u':
    cout << "Starting u-law to PCM conversion..." << endl;
    wave_file_transform(infile, outfile, Snack_Mulaw2Lin);
    cout << "Done." << endl;
    break;
  default:
    help(); /* should never be hit */
  }
  return 0;
}
