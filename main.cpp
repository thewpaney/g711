#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
using namespace std;

extern "C" {
#include "g711.h"
}

#include "wav.hpp"

/* see: https://stackoverflow.com/questions/16075233/reading-and-processing-wav-file-data-in-c-c */
/* also helpful: http://soundfile.sapp.org/doc/WaveFormat/ */

void help() {
  cout << "usage: [-u|-a] infile outfile" << endl;
  cout << "Pass -u for u-law conversion and -a for a-law conversion." << endl;
  exit(1);
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
  cout << "Making file..." << endl;
  WaveFile wf;
  cout << "Reading file..." << endl;
  switch (wf.read(infile)) {
  case WAV_BAD_INFILE:
    cout << argv[0] << ": error: bad input file" << endl;
    break;
  case WAV_MALFORMED_INFILE:
    cout << argv[0] << ": error: input file is not a WAV file" << endl;
    break;
  case WAV_ILLEGAL_CHUNK:
    cout << argv[0] << ": error: illegal chunk in input file" << endl;
    break;
  case WAV_UNKNOWN_CHUNK:
    cout << argv[0] << ": error: unknown chunk in input file" << endl;
    break;
  case WAV_SUCCESS:
    break;
  default:
    cout << argv[0] << ": error: unexpected error occurred" << endl;
    break;
  }
  cout << "Input WAV file: " << wf << endl;
  switch (mode) {
  case 'a':
    cout << "Starting A-law to PCM conversion..." << endl;
    wf.data_transform(Snack_Alaw2Lin);
    cout << "Done." << endl;
    break;
  case 'u':
    cout << "Starting u-law to PCM conversion..." << endl;
    wf.data_transform(Snack_Mulaw2Lin);
    cout << "Done." << endl;
    break;
  default:
    help(); /* should never be hit */
  }
  cout << "Output WAV file: " << wf << endl;
  cout << "Writing " << outfile << "...";
  wf.write(outfile);
  cout << "done." << endl;
  return 0;
}
