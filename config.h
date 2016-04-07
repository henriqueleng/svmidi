static const char font[] = "-*-terminus-medium-r-normal-*-16-*-*-*-*-*-*-*";
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
	{  XK_z,              C,               RELEASED },
	{  XK_x,              D,               RELEASED },
	{  XK_c,              E,               RELEASED },
	{  XK_v,              F,               RELEASED },
	{  XK_b,              G,               RELEASED },
	{  XK_n,              A,               RELEASED },
	{  XK_m,              B,               RELEASED },
	{  XK_comma,          C + 12,          RELEASED },
	{  XK_period,         D + 12,          RELEASED },
	{  XK_q,              E + 12,          RELEASED },
	{  XK_w,              F + 12,          RELEASED },
	{  XK_e,              G + 12,          RELEASED },
	{  XK_r,              A + 12,          RELEASED },
	{  XK_t,              B + 12,          RELEASED },
	{  XK_y,              C + 24,          RELEASED },
	{  XK_u,              D + 24,          RELEASED },
	{  XK_i,              E + 24,          RELEASED },
	{  XK_o,              F + 24,          RELEASED },
	{  XK_p,              G + 24,          RELEASED },
};

static Key blackkeys[] = {
	{  XK_s,              C_SHARP,         RELEASED },
	{  XK_d,              D_SHARP,         RELEASED },
	{  XK_g,              F_SHARP,         RELEASED },
	{  XK_h,              G_SHARP,         RELEASED },
	{  XK_j,              A_SHARP,         RELEASED },
	{  XK_l,              C_SHARP + 12,    RELEASED },
	{  XK_1,              D_SHARP + 12,    RELEASED },
	{  XK_3,              F_SHARP + 12,    RELEASED },
	{  XK_4,              G_SHARP + 12,    RELEASED },
	{  XK_5,              A_SHARP + 12,    RELEASED },
	{  XK_7,              C_SHARP + 24,    RELEASED },
	{  XK_8,              D_SHARP + 24,    RELEASED },
};
