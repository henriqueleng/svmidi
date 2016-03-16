#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	/* usleep */
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>	/* XkbSetDetectableAutoRepeat() */
#include <X11/extensions/Xdbe.h> /* double buffer */

#include "arg.h"
#include "midi.h"
#include "util.h"

/* future: implement alsa */
#ifdef ALSA
#include "alsa.h"
#endif
#ifdef SNDIO
#include "sndio.h"
#endif

/* macros */
#define LENGTH(x)        (sizeof x / sizeof x[0])
#define PRESSED  1
#define RELEASED 0

/* variables */
unsigned int winwidth;
unsigned int winheight;
const unsigned int keywidth = 50;
unsigned int keyheight;
unsigned int fontheight;
char *argv0;

typedef struct {
	KeySym keysym;
	int note;
	int status;
} Key;

typedef struct {
	int number;
	char *name;
} Instrument;

#include "config.h"

/* Xlib */
static int screen;
static Display *dpy;
static Window win;
GC gc;
Colormap colormap;
unsigned long xkeycolor, xkeypressedcolor, xsharpkeycolor, xsharpkeypressedcolor, xkeybordercolor, xfontcolor;
XFontStruct* font_info;
XWindowAttributes wa;

/* Xdbe */
XdbeBackBuffer buf;
XdbeBackBufferAttributes  *bufattr;
XdbeSwapInfo swapinfo;

void
changeinstrument(int channel, int number)
{
	u_char message[] = {PRG_CHANGE | channel, number};
	midisend(message);
}

void
sendnote(int action, int note, int channel, int speed)
{
	int mult;
	mult = octave;

	if (octave == 9) {
		switch (note) {
			case G:
			case G_SHARP:
			case A:
			case A_SHARP:
			case B:
				mult--;
				break;
		}
	}

	mult++;
	u_char message[] = {action | channel, note + (OCTAVE_VALUE * mult), speed};

	midisend(message);
}

/* idea from svkbd */
unsigned long
getcolor(const char *colstr)
{
	Colormap cmap = DefaultColormap(dpy, screen);
	XColor color;

	if(!XAllocNamedColor(dpy, cmap, colstr, &color, &color)) {
		fprintf(stderr, "error, cannot allocate color '%s'\n", colstr);
		exit(EXIT_FAILURE);
	}
	return color.pixel;
}

void
startwin(unsigned int initial_width, unsigned int initial_height)
{
	/* open connection with the server */
	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(EXIT_FAILURE);
	}

	screen = DefaultScreen(dpy);

	/* create window */
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0, initial_width, 
		initial_height, 1, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

	colormap = DefaultColormap(dpy, screen);

	/* colors */
	xkeycolor = getcolor(keycolor);
	xkeypressedcolor = getcolor(keypressedcolor);
	xsharpkeycolor = getcolor(sharpkeycolor);
	xsharpkeypressedcolor = getcolor(sharpkeypressedcolor);
	xkeybordercolor = getcolor(keybordercolor);
	xfontcolor = getcolor(fontcolor);

	/* try to load the given font. */
	font_info = XLoadQueryFont(dpy, font);
	if (!font_info) {
	    fprintf(stderr, "XLoadQueryFont: failed loading font: %s\n", font);
		exit(EXIT_FAILURE);
	}

	fontheight = font_info->ascent + font_info->descent;
	//printf("font height: %i\n", fontheight);

	XGCValues values;
	unsigned long valuemask = 0;
	gc = XCreateGC(dpy, win, valuemask, &values);
	XSetLineAttributes(dpy, gc, 1, LineSolid, CapRound, JoinMiter);
	XSetFont(dpy, gc, font_info->fid);

	/* Select kind of events we are interested in. */
	XSelectInput(dpy, win, KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask);

	/* Xdbe: back buffer */
	buf = XdbeAllocateBackBufferName(dpy, win, XdbeUndefined);
	bufattr = XdbeGetBackBufferAttributes(dpy, buf);
	swapinfo.swap_window = bufattr->window;
	swapinfo.swap_action = XdbeUndefined;
	XFree(bufattr);

	/* show the window */
	XMapWindow(dpy, win);

	/* fuck autorepeat */
	XAutoRepeatOn(dpy);
	XkbSetDetectableAutoRepeat(dpy, True, NULL);
}

void
drawkeyboard(/* winheight */)
{
	char string[30];
	sprintf(&string, "octave: %i", octave);

	XSetForeground(dpy, gc, xkeycolor);
	/* 
	 * TODO: is there really a need to fill a rectangle bellow text?
	 */
	XFillRectangle(dpy, buf, gc, 0, 0, winwidth, fontheight);
//	XFillRectangle(dpy, buf, gc, 0, 0, winwidth, winheight);

	XSetForeground(dpy, gc, xfontcolor);
	XDrawString(dpy, buf, gc, 0, fontheight - 2, string, strlen(string));

	unsigned int usedspace = 0, totalkeys = 0;
	unsigned int i = 0;

	keyheight = winheight - 100;
	unsigned int nwhitekeys = (int)LENGTH(whitekeys);
	unsigned int nblackkeys = (int)LENGTH(blackkeys);

	/* 
	 * White keys must be drawn first so they stay beneath black ones. 
	 */
	while (i < nwhitekeys) {
		if (whitekeys[totalkeys].status == PRESSED) {
			XSetForeground(dpy, gc, xkeypressedcolor);
			XFillRectangle(dpy, buf, gc, usedspace, fontheight, keywidth, keyheight);
		} else {
			/* normal color */
			XSetForeground(dpy, gc, xkeycolor);
			XFillRectangle(dpy, buf, gc, usedspace, fontheight, keywidth, keyheight);
		}
		XSetForeground(dpy, gc, xkeybordercolor);
		XDrawRectangle(dpy, buf, gc, usedspace, fontheight, keywidth, keyheight);
		i++;
		usedspace += keywidth;
		totalkeys++;
	}

	usedspace = 0;
	totalkeys = 0;

	unsigned int subskeys = 0;
	unsigned int alternate = 1;

	/* draw black keys */
	while (nblackkeys > 1) {
		/* alternate between 3 and 2 subsequent keys */
		if (alternate == 1 && subskeys == 2) {
			usedspace += keywidth;
			subskeys = 0;
			alternate = 2;
		} else if (subskeys == 3) {
			usedspace += keywidth;
			subskeys = 0;
			alternate = 1;
		}

		/* draw */
		if (blackkeys[totalkeys].status == PRESSED) {
			XSetForeground(dpy, gc, xsharpkeypressedcolor);
			XFillRectangle(dpy, buf, gc, usedspace + ((keywidth * 2) / 3), 
				fontheight, (keywidth * 2) / 3 , keyheight / 2);
		} else if (blackkeys[totalkeys].status == RELEASED) {
    		XSetForeground(dpy, gc, xsharpkeycolor);
			XFillRectangle(dpy, buf, gc, usedspace + ((keywidth * 2) / 3), 
				fontheight, (keywidth * 2) / 3 , keyheight / 2); 
		}
			usedspace += keywidth;
			subskeys++;
			totalkeys++;
			nblackkeys--;
	}

	XdbeSwapBuffers(dpy, &swapinfo, 1);
}

void
run(void)
{
    unsigned int nwhitekeys = (int)LENGTH(whitekeys);
    unsigned int nblackkeys = (int)LENGTH(blackkeys);
	KeySym keysym;
	XEvent e;
	unsigned int lastpress;
	unsigned int i;
	int ha;

	while (1) {
		XNextEvent(dpy, &e);
		switch (e.type) {

		char string;
		int len;

		case KeyPress:
			if (lastpress != e.xkey.keycode || lastpress == 0) {
				/*
				 * Handle keyboard inside this field.
				 */
				keysym = XLookupKeysym (&e.xkey, 0);
				//printf("p: %i\n", (int)keysym);

				if (keysym == XK_i && e.xkey.state & ControlMask) {
					return 0;
				}

				/* match key xkeypressedcolor with a member of whitekeys[] or blackkeys[] */
				i = 0;
				for (i = 0; i < nwhitekeys; i++) {
					if (whitekeys[i].keysym == keysym) {
						sendnote(NOTE_ON, whitekeys[i].note, 1, 100);
						whitekeys[i].status = PRESSED;
						break;
					}
				}
	
				for (i = 0; i < nblackkeys; i++) {
					if (blackkeys[i].keysym == keysym) {
						sendnote(NOTE_ON, blackkeys[i].note, 1, 100);
						//printf("Sent note: %i\n", blackkeys[i].note);
						blackkeys[i].status = PRESSED;
						break;
					}
				}
	
				drawkeyboard();
			}
			lastpress = e.xkey.keycode;
			break;

		case KeyRelease:
			/* magic to avoid key repetition */
			if (e.xkey.keycode == lastpress)
				lastpress = 0;
				keysym = XLookupKeysym (&e.xkey, 0);
				//printf("r: %i\n", (int)keysym);
				i = 0;

                for (i = 0; i < nwhitekeys; i++) {
                    if (whitekeys[i].keysym == keysym) {
						sendnote(NOTE_OFF, whitekeys[i].note, 1, 100);
                        whitekeys[i].status = RELEASED;
                        break;
                    }
                }

                for (i = 0; i < nblackkeys; i++) {
                    if (blackkeys[i].keysym == keysym) {
                        blackkeys[i].status = RELEASED;
						sendnote(NOTE_OFF, blackkeys[i].note, 1, 100);
                        break;
                    }
                }
			drawkeyboard();
			lastpress = 0;
			break;


		case ButtonPress:
			XGetWindowAttributes(dpy, win, &wa);
			winwidth = wa.width;
			winheight = wa.height;
			//printf("got a Button Press event %u\n", e.xkey.keycode);
			octave++;
			drawkeyboard();
			break;

		case ButtonRelease:
			break;

		case ConfigureNotify:
			winheight = e.xconfigure.height;
			drawkeyboard();
			break;

		case MotionNotify:
			break;

		default:
			//printf("Event being thrown away\n");
			break;
		}
	}
}

void
quit(void)
{
	/* X11 close */
	XdbeDeallocateBackBufferName(dpy, buf);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);

	/* midi close */
	midiclose();
}

int 
main(int argc, char *argv[])
{
/*
	rootwidth = DisplayHeight(dpy, screen);
	rootheight = DisplayWidth(dpy, screen);
*/

	ARGBEGIN {
	case 's':
		printf("s flag \n");
		return 0;
		break;
	} ARGEND

	octave = 1;

	/* initialization */
	/* start midi */
	if (midiopen()) {
		perror("failed to open MIDI output");
		exit(EXIT_FAILURE);
	}
	changeinstrument(1, 23);
	startwin(640, 400);

	run();
	quit();
	return 0;
}
