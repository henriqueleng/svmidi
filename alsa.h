#include <alsa/asoundlib.h>

/* device */
extern snd_seq_t *hdl;

/* functions */
void _midisend(unsigned char [], size_t);
#define midisend(msg) _midisend(msg, sizeof(msg))
int midiopen(void);
void midiclose(void);
