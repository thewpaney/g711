/*
 * g711.h
 *
 * Prototypes for functions in `g711.c`.
 */

static short search(short val, short * table, short size);
unsigned char Snack_Lin2Alaw(short pcm_val);
short Snack_Alaw2Lin(unsigned char a_val);
unsigned char Snack_Lin2Mulaw(short pcm_val);
short Snack_Mulaw2Lin(unsigned char u_val);
unsigned char alaw2ulaw(unsigned char aval);
unsigned char ulaw2alaw(unsigned char uval);
