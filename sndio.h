/* device */
extern struct mio_hdl *hdl;

/* functions */
int midiopen(void);
void midisend(unsigned char [], size_t);
void midiclose(void);
