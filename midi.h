int octave;
int instrument;
int channel;

/* midi commands */
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define PRG_CHANGE 0xC0

/* notes values on -1 octave */
#define C 0
#define C_SHARP 1
#define D 2
#define D_SHARP 3
#define E 4
#define F 5
#define F_SHARP 6
#define G 7
#define G_SHARP 8
#define A 9
#define A_SHARP 10
#define B 11

/* value increased by one octave */
#define OCTAVE_VALUE 12

/* limits */
#define NOTE_MIN 0
#define NOTE_MAX 127
#define OCTAVE_MIN -1
#define OCTAVE_MAX 9
