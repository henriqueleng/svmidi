#include <alsa/asoundlib.h>

/* device */
extern snd_seq_t *hdl;

/* functions */
void midisend(unsigned char [], size_t);
int midiopen(void);
void midiclose(void);
