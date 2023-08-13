#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	/* getopt */
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>	/* XkbSetDetectableAutoRepeat() */
#include <X11/extensions/Xdbe.h> /* double buffer */

#ifdef ALSA
#include "alsa.h"
#elif SNDIO
#include "sndio.h"
#endif

/* macros */
#define LENGTH(x)        (sizeof x / sizeof x[0])

typedef unsigned int uint;
typedef unsigned long ulong;

/* variables */
uint winwidth, winheight;
const uint keywidth = 50;
uint keyheight;
uint fontheight;
uint nwhitekeys, nblackkeys;
/* status of white and black keys */
int wstatus = 0, bstatus = 0;

typedef struct {
	KeySym keysym;
	int note;
} Key;

#include "midi.h"
#include "config.h"

/* Xlib */
static int screen;
static Display *dpy;
static Window win;
GC gc;
Colormap colormap;
ulong xkeycolor, xkeypressedcolor, xsharpkeycolor,
	xsharpkeypressedcolor, xkeybordercolor, xfontcolor, xbgcolor;
XFontStruct* font_info;
XWindowAttributes wa;
Atom wm_delete_win;

/* Xdbe */
XdbeBackBuffer buf;
XdbeBackBufferAttributes  *bufattr;
XdbeSwapInfo swapinfo;

void
quit(void)
{
	/* X11 close */
	XFreeFont(dpy, font_info);
	XdbeDeallocateBackBufferName(dpy, buf);
	XFreeGC(dpy, gc);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);

	/* midi close */
	midiclose();
}

void
changeinstrument(int number)
{
	unsigned char message[] = {PRG_CHANGE | channel, number};
	midisend(message, sizeof(message));
}

void
sendnote(int action, int note, int speed)
{
	int mult = octave;
	mult++;
	unsigned char message[] = {action | channel, note + (OCTAVE * mult),
	    speed};
	midisend(message, sizeof(message));
}

/* idea from svkbd */
ulong
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
startwin(uint initial_width, uint initial_height)
{
	/* open connection with the server */
	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(EXIT_FAILURE);
	}

	screen = DefaultScreen(dpy);

	/* create window */
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0,
	    initial_width, initial_height, 1, BlackPixel(dpy, screen),
	    WhitePixel(dpy, screen));

	colormap = DefaultColormap(dpy, screen);

	/* set window name */
	XStoreName(dpy, win, "svmidi");

	/* colors */
	xkeycolor = getcolor(keycolor);
	xkeypressedcolor = getcolor(keypressedcolor);
	xsharpkeycolor = getcolor(sharpkeycolor);
	xsharpkeypressedcolor = getcolor(sharpkeypressedcolor);
	xkeybordercolor = getcolor(keybordercolor);
	xfontcolor = getcolor(fontcolor);
	xbgcolor = getcolor(bgcolor);

	/* try to load the given font. */
	font_info = XLoadQueryFont(dpy, font);
	if (!font_info) {
		fprintf(stderr, "XLoadQueryFont: failed loading font: %s\n",
		    font);
		exit(EXIT_FAILURE);
	}

	fontheight = font_info->ascent + font_info->descent;

	XGCValues values;
	ulong valuemask = 0;
	gc = XCreateGC(dpy, win, valuemask, &values);
	XSetLineAttributes(dpy, gc, 1, LineSolid, CapRound, JoinMiter);
	XSetFont(dpy, gc, font_info->fid);

	/* Select kind of events we are interested in. */
	XSelectInput(dpy, win, KeyPressMask | KeyReleaseMask |
	    StructureNotifyMask | ExposureMask);

	wm_delete_win = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &wm_delete_win, 1);

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

	/* get initial window size */
	XGetWindowAttributes(dpy, win, &wa);
	winwidth = wa.width;
	winheight = wa.height;
}

void
cleanwindow(void) /* winheight, winwidth */
{
	/* just fill a rectangle with the size of the window */
	XSetForeground(dpy, gc, xbgcolor);
	XFillRectangle(dpy, buf, gc, 0, 0, winwidth, winheight);
}


void
drawkeyboard(void)
{
	/* draw top bar */
	char string[100];
	snprintf(string, (size_t)100,
	    "octave: %i   channel: %i    instrument: %i - %s",
	    octave, channel, instrument, instruments[instrument].name);

	cleanwindow();
	XSetForeground(dpy, gc, xfontcolor);
	XDrawString(dpy, buf, gc, 0, fontheight - 2, string, strlen(string));

	uint usedspace = 0, totalkeys = 0;
	uint i = 0;

	keyheight = winheight - 10;

	/*
	 * White keys must be drawn first so they stay beneath black ones.
	 */
	for (i = 0; i < nwhitekeys; i++) {
		if (wstatus & (1<<i))
			XSetForeground(dpy, gc, xkeypressedcolor);
		else
			XSetForeground(dpy, gc, xkeycolor);

		/* draw key */
		XFillRectangle(dpy, buf, gc, usedspace,
		    fontheight, keywidth, keyheight);

		/* draw key boarder */
		XSetForeground(dpy, gc, xkeybordercolor);
		XDrawRectangle(dpy, buf, gc, usedspace, fontheight, keywidth,
		    keyheight);
		usedspace += keywidth;
		totalkeys++;
	}

	usedspace = 0;

	uint subskeys  = 0;
	uint alternate = 0;

	int note0 = whitekeys[0].note % 12;

	if (note0 < 0)
		note0 += 12;

	switch (note0) {
		case 0:
		case 2:
		case 4:
			alternate = 1;
			subskeys  = note0 / 2;
			break;

		case 5:
		case 7:
		case 9:
		case 11:
			alternate = 0;
			subskeys = (note0 - 5) / 2;
			break;

		default:
			fprintf(stderr,
			    "first white note %d is not valid white key\n",
			    whitekeys[0].note);
			exit(EXIT_FAILURE);
			quit();
	}

	/* draw black keys */
	for (i = 0; i < nblackkeys; i++) {
		/* alternate between 3 and 2 subsequent keys */
		if (alternate == 1 && subskeys == 2) {
			usedspace += keywidth;
			subskeys = 0;
			alternate = 0;
		} else if (subskeys == 3) {
			usedspace += keywidth;
			subskeys = 0;
			alternate = 1;
		}

		/* set color */
		if (bstatus & (1<<i))
			XSetForeground(dpy, gc, xsharpkeypressedcolor);
		else
			XSetForeground(dpy, gc, xsharpkeycolor);

		/* draw */
		XFillRectangle(dpy, buf, gc,
		    usedspace + ((keywidth * 2) / 3), fontheight,
		    (keywidth * 2) / 3 , (keyheight * 2) / 3);

		usedspace += keywidth;
		subskeys++;
	}

	XdbeSwapBuffers(dpy, &swapinfo, 1);
}

void
drawinstruments(void)
{
	/*
	 * find biggest array in instrument list
	 */
	uint biggest = 0, length = 0;
	for (uint i = 0; i < LENGTH(instruments); i++) {
		if ((strlen(instruments[i].name)) > length) {
			length = strlen(instruments[i].name);
			biggest = i;
		}
	}

	/* strlen doesn't count \0, so the actual array length is one bigger */
	length++;

	uint textwidth = XTextWidth(font_info, instruments[biggest].name,
	    length);

	/* pixels occupied by one letter */
	uint letterwidth = XTextWidth(font_info, "E", 1);

	/*
	 * horizontal spacing for printing next
	 * column, add 10 for extra separation
	 */
	uint columnwidth = 4 * letterwidth + textwidth + 10;

	/*
	 * add four elements to array to fit the two
	 * numbers plus the spacing before instrument name
	 */
	length += 4;

	int spacey = 0, spacex = 0;
	for (uint i = 0; i < LENGTH(instruments); i++) {
		char string[length];
		snprintf(string, length, "%i: %s", instruments[i].number,
		    instruments[i].name);

		if (spacey >= winheight - (fontheight * 2)) {
			spacey = fontheight;
			spacex += columnwidth;
		} else {
			spacey += fontheight;
		}
		XSetForeground(dpy, gc, xfontcolor);
		XDrawString(dpy, buf, gc, spacex, spacey, string,
		    strlen(string));
	}
}

void
instrumentselection(void)
{
	cleanwindow();
	drawinstruments();

	/* vars */
	XEvent e2;
	KeySym tmpkeysym = NoSymbol;

	/* add prompt and calculate pixel size */
	char prompt[] = "> ";
	int promptwidth = XTextWidth(font_info,
			  prompt, strlen(prompt));

	/* draw prompt */
	XDrawString(dpy, buf, gc, 0, winheight - 5,
		prompt , strlen(prompt));
	XdbeSwapBuffers(dpy, &swapinfo, 1);

	int len = 0;
	char string[10] = {0};

	/* read input until enter or filling string */
	while (len < 10 - 1 &&
	       tmpkeysym != XK_Return &&
	       tmpkeysym != XK_KP_Enter) {
		XNextEvent(dpy, &e2);
		char input[1] = {0};

		/* start reading events */
		switch (e2.type) {
		case KeyPress:
			len = strlen(string);
			XLookupString(&e2.xkey, input, 
			    sizeof(input), &tmpkeysym, NULL);

			/* at any time if press Escape, return to keyboard
			 * without changing instrument */
			if (tmpkeysym == XK_Escape) {
				tmpkeysym = XK_Return;
				string[0] = '\0';
				break;
			}

			/* also quit if Ctrl + q on instrument list */
			if (tmpkeysym == XK_q && e2.xkey.state & ControlMask) {
				quit();
				exit(EXIT_SUCCESS);
			}

			if (isdigit(input[0])) {
				string[len] = input[0];
				string[len + 1] = '\0';
			} else if (tmpkeysym == XK_BackSpace) {
				string[len - 1] = '\0';
			}

			cleanwindow();
			drawinstruments();
			XDrawString(dpy, buf, gc,
				0, winheight - 5,
				prompt , strlen(prompt));
			XDrawString(dpy, buf, gc,
				promptwidth, winheight - 5,
				string, strlen(string));
			XdbeSwapBuffers(dpy, &swapinfo, 1);
			break;

		case Expose:
			cleanwindow();
			drawinstruments();
			XDrawString(dpy, buf, gc, 0, winheight - 5,
				prompt , strlen(prompt));
			XDrawString(dpy, buf, gc,
				promptwidth, winheight - 5,
				string, strlen(string));
			XdbeSwapBuffers(dpy, &swapinfo, 1);
			break;

		case ConfigureNotify:
			winheight = e2.xconfigure.height;
			winwidth = e2.xconfigure.width;
			cleanwindow();
			drawinstruments();
			XDrawString(dpy, buf, gc, 0, winheight - 5,
				prompt , strlen(prompt));
			XDrawString(dpy, buf, gc,
				promptwidth, winheight - 5,
				string, strlen(string));
			XdbeSwapBuffers(dpy, &swapinfo, 1);
			break;

		case ClientMessage:
			if (e2.xclient.data.l[0] == wm_delete_win) {
				quit();
				exit(EXIT_SUCCESS);
			}
			break;
		}
	}

	int newinstrument;
	if (strlen(string) != 0) {
		newinstrument = atoi(string);
		if (newinstrument > 127 || newinstrument < 0) {
			/* print error and wait for key press */
			cleanwindow();
			drawinstruments();
			char tmpstring[] = "ERROR: number out of range";
			XSetForeground(dpy, gc, xfontcolor);
			XDrawString(dpy, buf, gc,
				0, winheight - 5,
				tmpstring, strlen(tmpstring));
			XdbeSwapBuffers(dpy, &swapinfo, 1);
			/* wait for keypress */
			e2.type = NoSymbol;
			while (e2.type != KeyPress) {
				XNextEvent(dpy, &e2);
			}
		} else {
			/* instrument changed and is a good number */
			instrument = newinstrument;
			changeinstrument(newinstrument);
		}
	}
}

void
run(void)
{
	KeySym keysym;
	XEvent e;

	while (1) {
		uint i = 0;
		XNextEvent(dpy, &e);
		switch (e.type) {

			case KeyPress:
				keysym = XLookupKeysym(&e.xkey, 0);

				if (keysym == XK_k &&
				    e.xkey.state & ControlMask &&
				    octave < 9) {
					octave++;
					break;
				}

				if (keysym == XK_j &&
				    e.xkey.state & ControlMask &&
				    octave > -1) {
					octave--;
					break;
				}

				if (keysym == XK_l &&
				    e.xkey.state & ControlMask) {
					channel++;
					break;
				}

				if (keysym == XK_h &&
				    e.xkey.state & ControlMask &&
				    channel > 0) {
					channel--;
					break;
				}

				/* enter instrument select loop if Ctrl + i */
				if (keysym == XK_i &&
				    e.xkey.state & ControlMask) {
					instrumentselection();
					break;
				}

				if (keysym == XK_q &&
				    e.xkey.state & ControlMask) {
					quit();
					exit(EXIT_SUCCESS);
				}

				 /* match key with a member 
				  * of whitekeys[] or blackkeys[] */
				for (i = 0; i < nwhitekeys; i++) {
					if (whitekeys[i].keysym == keysym &&
					    !(wstatus & (1<<i))) {
						sendnote(NOTE_ON,
						    whitekeys[i].note, 100);
						wstatus |= (1<<i);
						break;
					}
				}

				for (i = 0; i < nblackkeys; i++) {
					if (blackkeys[i].keysym == keysym &&
					    !(bstatus & (1<<i))) {
						sendnote(NOTE_ON,
						    blackkeys[i].note, 100);
						bstatus |= (1<<i);
						break;
					}
				}

				drawkeyboard();
			break;

		case KeyRelease:
			keysym = XLookupKeysym(&e.xkey, 0);

			for (i = 0; i < nwhitekeys; i++) {
				if (whitekeys[i].keysym == keysym &&
				    (wstatus & (1<<i))) {
					sendnote(NOTE_OFF,
					    whitekeys[i].note, 100);
					wstatus ^= (1<<i);
					break;
				}
			}

			for (i = 0; i < nblackkeys; i++) {
				if (blackkeys[i].keysym == keysym &&
				    (bstatus & (1<<i))) {
					sendnote(NOTE_OFF,
					    blackkeys[i].note, 100);
					bstatus ^= (1<<i);
					break;
				}
			}
			drawkeyboard();
			break;

		case ConfigureNotify:
			winheight = e.xconfigure.height;
			winwidth = e.xconfigure.width;
			drawkeyboard();
			break;

		case Expose:
			drawkeyboard();
			break;

		case ClientMessage:
			if (e.xclient.data.l[0] == wm_delete_win) {
				quit();
				exit(EXIT_SUCCESS);
			}
			break;

		default:
			break;
		}
	}
}

void
usage(void)
{
	fprintf(stderr, "usage: svmidi [-i instrument] [-o octave] [-v]\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int ch;

	while ((ch = getopt(argc, argv, "i:o:hv")) != -1) {
		switch (ch) {
		case 'i':
			instrument = atoi(optarg);
			break;
		case 'o':
			octave = atoi(optarg);
			break;
		case 'h':
			usage();
			break;
		case 'v':
			printf("svmidi-"VERSION"\n");
			return 0;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (instrument > INST_MAX || instrument < INST_MIN) {
		fprintf(stderr, "intrument number out of range\n");
		exit(EXIT_FAILURE);
	}

	if (octave > OCTAVE_MAX || octave < OCTAVE_MIN) {
		fprintf(stderr, "octave number out of range\n");
		exit(EXIT_FAILURE);
	}

	/* start midi */
	if (midiopen()) {
		fprintf(stderr, "failed to open MIDI device\n");
		exit(EXIT_FAILURE);
	}
	changeinstrument(instrument);

	/* assign some variables */
	channel = 1;
	nwhitekeys = (int)LENGTH(whitekeys);
	nblackkeys = (int)LENGTH(blackkeys);

	startwin(1000, 300);
	run();
	quit();
	return 0;
}
