static const char font[] = "-misc-fixed-medium-r-semicondensed-*-13-*-*-*-*-*-iso10646-*";
static const char keycolor[] = "#ffffff";
static const char keypressedcolor[] = "#6666ff";
static const char sharpkeycolor[] = "#323232";
static const char sharpkeypressedcolor[] = "#6666ff";
static const char keybordercolor[] = "#000000";
static const char fontcolor[] = "#000000";
static const char bgcolor[] = "#ffffff";

/*
 * Keys will be drawn in order, first all the white keys 
 * and then the blacks ones top.
 */
static Key whitekeys[] = {
	/* keysym             note   */
	{  XK_z,              B - 12 },
	{  XK_x,              C      },
	{  XK_c,              D      },
	{  XK_v,              E      },
	{  XK_b,              F      },
	{  XK_n,              G      },
	{  XK_m,              A      },
	{  XK_comma,          B      },
	{  XK_period,         C + 12 },
	{  XK_q,              D + 12 },
	{  XK_w,              E + 12 },
	{  XK_e,              F + 12 },
	{  XK_r,              G + 12 },
	{  XK_t,              A + 12 },
	{  XK_y,              B + 12 },
	{  XK_u,              C + 24 },
	{  XK_i,              D + 24 },
	{  XK_o,              E + 24 },
	{  XK_p,              F + 24 },
};

static Key blackkeys[] = {
	/* keysym             note         */
	{  XK_d,              C_SHARP      },
	{  XK_f,              D_SHARP      },
	{  XK_h,              F_SHARP      },
	{  XK_j,              G_SHARP      },
	{  XK_k,              A_SHARP      },
	{  XK_1,              C_SHARP + 12 },
	{  XK_2,              D_SHARP + 12 },
	{  XK_4,              F_SHARP + 12 },
	{  XK_5,              G_SHARP + 12 },
	{  XK_6,              A_SHARP + 12 },
	{  XK_8,              C_SHARP + 24 },
	{  XK_9,              D_SHARP + 24 },
};
