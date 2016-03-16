/* device */
extern struct mio_hdl *hdl;

/* functions */
int midiopen(void);
void midisend(u_char []);
void midiclose(void);
