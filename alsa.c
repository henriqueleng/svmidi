#include <alsa/asoundlib.h>

snd_seq_t *hdl;
static snd_midi_event_t *mbuf;
static int midiport;

/* open */
int
midiopen(void)
{
	snd_midi_event_new(32, &mbuf);
	if (snd_seq_open(&hdl, "default", SND_SEQ_OPEN_OUTPUT, 0) != 0) {
		return 1;
	} else {
		snd_seq_set_client_name(hdl, "svmidi");
		if ((midiport = snd_seq_create_simple_port(hdl, "svmidi",
			    SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
			    SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
			return 1;
		}
		return 0;
	}
}

/* send message */
void
midisend(unsigned char message[], size_t count)
{
	snd_seq_event_t ev;

	memset(&ev, 0, sizeof(snd_seq_event_t));
	snd_midi_event_encode(mbuf, message, count, &ev);

	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, midiport);
	snd_seq_event_output_direct(hdl, &ev);
}

/* close */
void
midiclose(void)
{
	snd_midi_event_free(mbuf);
	snd_seq_close(hdl);
	snd_config_update_free_global();
}
