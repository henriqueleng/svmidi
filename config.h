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
	/* keysym             note             status */
	{  XK_z,              B - 12,          RELEASED },
	{  XK_x,              C,               RELEASED },
	{  XK_c,              D,               RELEASED },
	{  XK_v,              E,               RELEASED },
	{  XK_b,              F,               RELEASED },
	{  XK_n,              G,               RELEASED },
	{  XK_m,              A,               RELEASED },
	{  XK_comma,          B,               RELEASED },
	{  XK_period,         C + 12,          RELEASED },
	{  XK_q,              D + 12,          RELEASED },
	{  XK_w,              E + 12,          RELEASED },
	{  XK_e,              F + 12,          RELEASED },
	{  XK_r,              G + 12,          RELEASED },
	{  XK_t,              A + 12,          RELEASED },
	{  XK_y,              B + 12,          RELEASED },
	{  XK_u,              C + 24,          RELEASED },
	{  XK_i,              D + 24,          RELEASED },
	{  XK_o,              E + 24,          RELEASED },
	{  XK_p,              F + 24,          RELEASED },
};

static Key blackkeys[] = {
	/* keysym             note             status */
	{  XK_d,              C_SHARP,         RELEASED },
	{  XK_f,              D_SHARP,         RELEASED },
	{  XK_h,              F_SHARP,         RELEASED },
	{  XK_j,              G_SHARP,         RELEASED },
	{  XK_k,              A_SHARP,         RELEASED },
	{  XK_1,              C_SHARP + 12,    RELEASED },
	{  XK_2,              D_SHARP + 12,    RELEASED },
	{  XK_4,              F_SHARP + 12,    RELEASED },
	{  XK_5,              G_SHARP + 12,    RELEASED },
	{  XK_6,              A_SHARP + 12,    RELEASED },
	{  XK_8,              C_SHARP + 24,    RELEASED },
	{  XK_9,              D_SHARP + 24,    RELEASED },
};
