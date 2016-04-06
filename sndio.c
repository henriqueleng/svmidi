#include <sndio.h>

struct mio_hdl *hdl;

/* open */
int
midiopen(void)
{
	hdl = mio_open(MIO_PORTANY, MIO_OUT, 0);
	if (hdl == NULL) {
		return 1;
	} else {
		return 0;
	}
}

/* send message */
void
midisend(u_char message[], size_t size)
{
	mio_write(hdl, message, size);
}

/* close */
void
midiclose(void)
{
	mio_close(hdl);
}
