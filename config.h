/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
#include "include_patches.h"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int gappx     = 15;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10", "Noto Color Emoji:style=Regular"  };
static const char dmenufont[]       = "Inconsolata:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_black[]       = "#000000";
static const char col_red[]         = "#ff0000";
static const char col_yellow[]      = "#ffff00";
static const char col_white[]       = "#ffffff";
static const char col_green[]       = "#00ff00";

#if PYWAL
#include "/home/chinmay/.cache/wal/colors-wal-dwm.h"
#else
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
	[SchemeWarn] = { col_black, col_yellow, col_red },
	[SchemeUrgent]={ col_white, col_red,    col_red },
	[4]= { col_black, col_green,    col_gray2 },
};
#endif

#if SCRATCHPADS
typedef struct {
       const char *name;
       const void *cmd;
} Sp;
const char *spcmd1[] = { "ibus-ui-emojier-plasma" };		/* Plasma emoji picker */
/* const char *spcmd2[] = {"st", "-n", "spfm", "-g", "144x41", "-e", "ranger", NULL }; */
/* const char *spcmd3[] = {"keepassxc", NULL }; */
static Sp scratchpads[] = {
       /* name          cmd  */
       {"spterm",      spcmd1},
       /* {"spranger",    spcmd2}, */
       /* {"keepassxc",   spcmd3}, */
};
#endif


/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	/* { "dolphin",  NULL,       NULL,       0,            1,           -1 }, */
	{ "yakuake",  NULL,       NULL,       0,            1,           -1 },
	{ "Microsoft Teams - Preview",
		      NULL,       NULL,       0,            1,           -1 },
	{ "Steam",    NULL,       NULL,       0,            1,           -1 },
	{ "Gnome-calendar",NULL,  NULL,       0,            1,           -1 },
	{ "okular",   NULL,       NULL,       1<<2 /* 3rd*/,0,           -1 },
#if SCRATCHPADS
	{ NULL,      "ibus-ui-emojier-plasma",
                                  NULL,       SPTAG(0),     1,           -1 },
#endif
       /* { NULL,           "spfm",               NULL,           SPTAG(1),               1,                       -1 }, */
       /* { NULL,           "keepassxc",  NULL,           SPTAG(2),               0,                       -1 }, */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
/* 0 */	{ "[]=",      tile },    /* first entry is default */
/* 1 */	{ "><>",      NULL },    /* no layout function means floating behavior */
/* 2 */	{ "[M]",      monocle },
/* 3 */	{ "TTT",      bstack },
/* 4 */	{ "===",      bstackhoriz },
#if CENTERED_MASTER
/* 5 */	{ "|M|",      centeredmaster },
/* 6 */	{ ">M>",      centeredfloatingmaster },
#endif  // CENTERED_MASTER
};

/* key definitions */
#define CONTROLKEY ControlMask
#define MODKEY Mod4Mask     /* Mod 4 --> Super */
#define HOLDKEY 0xffeb	    /* Keysym for Super_L. Check other keysyms with `xev'. */
#define TAGKEYS(KEY,TAG) \
{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };

#if STATUSCMD
/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
static char *statuscmds[] = { "clickbar.sh -n", /* Bandwidth */
                              "clickbar.sh -v", /* Volume change */
                              "clickbar.sh",    /* Battery */
                              "clickbar.sh -s", /* System usage. */
                              "clickbar.sh -d", /* Date time */
                              "clickbar.sh",    /* Caps Lock, Num lock Indicator */
                            };
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };
#else
static const char *termcmd[]  = { "konsole", NULL };
#endif

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,		XK_a,		spawn,		SHCMD("new_wall.sh") },
	{ MODKEY|ShiftMask,	XK_a,		spawn,		SHCMD("disp_config.sh") },
	{ MODKEY,		XK_b,		setlayout,	{.v = &layouts[3]} },
	/* { MODKEY|ShiftMask,	XK_b,		,		{} }, */
	{ MODKEY,		XK_c,		spawn,		SHCMD("startpy3.sh") },
	/* { MODKEY|ShiftMask,	XK_c,		,		{} }, */
	/* { MODKEY,		XK_d,		incnmaster,	{.i = -1 } }, */
	{ MODKEY|ShiftMask,	XK_d,		spawn,		SHCMD("dolphin") },
	{ MODKEY,		XK_e,		spawn,		SHCMD("editor.sh") },
	/* { MODKEY|ShiftMask,	XK_e,		quit,		{0} }, */
	{ MODKEY|ControlMask,	XK_e,		quit,		{0} },
#if FULL_SCREEN
        { MODKEY,		XK_f,		fullscreen,     {0} },
#endif  //FULL_SCREEN
        { MODKEY,		XK_f,		setlayout,     {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,	XK_f,		spawn,		SHCMD("dmenu_input.sh -t") },
#if CENTERED_MASTER
	{ MODKEY,		XK_g,		setlayout,	{.v = &layouts[5]} },
	{ MODKEY|ShiftMask,	XK_g,		setlayout,	{.v = &layouts[6]} },
#endif  // CENTERED_MASTER
	{ MODKEY,		XK_h,		setmfact,	{.f = -0.05} },
	{ MODKEY|ShiftMask,	XK_h,		incnmaster,	{.i = +1 } },
	/* { MODKEY,		XK_i,		incnmaster,	{.i = +1 } }, */
	/* { MODKEY|ShiftMask,	XK_i,		,		{} }, */
	{ MODKEY,		XK_j,		focusstack,	{.i = +1 } },
	/* { MODKEY|ShiftMask,	XK_j,		,		{} }, */
	{ MODKEY,		XK_k,		focusstack,	{.i = -1 } },
	/* { MODKEY|ShiftMask,	XK_k,		,		{} }, */
	{ MODKEY,		XK_l,		setmfact,	{.f = +0.05} },
	{ MODKEY|ShiftMask,	XK_l,		incnmaster,	{.i = -1 } },
	/* { MODKEY,		XK_m,		setlayout,	{.v = &layouts[2]} }, */
	{ MODKEY|ShiftMask,	XK_m,		spawn,		SHCMD("dmenu_input.sh -m") },
	/* { MODKEY,		XK_n,		,		{} */
	{ MODKEY|ShiftMask,	XK_n,		spawn,  	SHCMD("dmenu_input.sh -d") },
	{ MODKEY,		XK_o,		spawn,		SHCMD("dmenu_input.sh -r") },
	{ MODKEY|ShiftMask,	XK_o,		spawn,		SHCMD("dmenu_input.sh -f") },
	{ MODKEY,       	XK_p,		spawn,		{.v = dmenucmd } },
	/* { MODKEY|ShiftMask,	XK_p,		,		{} }, */
	{ MODKEY,		XK_q,		killclient,	{0} },
	/* { MODKEY|ShiftMask,	XK_q,		spawn,		SHCMD("blur_lock.sh") }, */
	{ MODKEY|ControlMask,	XK_q,		spawn,		SHCMD("blur_lock.sh") },
	/* { MODKEY,		XK_r,		,		{} */
	/* { MODKEY|ShiftMask,	XK_r,		quit,		{1} }, */
	{ MODKEY|ControlMask,	XK_r,		quit,		{1} },
	{ MODKEY,		XK_s,		spawn,		SHCMD("dmenu_input.sh -g") },
	{ MODKEY|ShiftMask,	XK_s,		spawn,		SHCMD("screenkey_toggle.sh") },
	{ MODKEY,		XK_t,		setlayout,	{.v = &layouts[0]} },
	/* { MODKEY|ShiftMask,	XK_t,		,		{} }, */
	/* { MODKEY,		XK_u,		,		{} */
	/* { MODKEY|ShiftMask,	XK_u,		,		{} }, */
	/* { MODKEY,		XK_v,		,		{} */
	/* { MODKEY|ShiftMask,	XK_v,		,		{} }, */
	{ MODKEY,		XK_w,		spawn,		SHCMD("dmenu_input.sh -w") },
	/* { MODKEY|ShiftMask,	XK_w,		,		{} }, */
	/* { MODKEY,		XK_x,		setgaps,	{.i = gappx  } }, */
	/* { MODKEY|ShiftMask,	XK_x,		setgaps,	{.i = 0  } }, */
	/* { MODKEY,		XK_y,		,		{} */
	/* { MODKEY|ShiftMask,	XK_y,		,		{} }, */
	{ MODKEY,		XK_z,		setgaps,	{.i = +1 } },
	{ MODKEY|ShiftMask,	XK_z,		setgaps,	{.i = -1 } },

        /* Non-alphabets. */

	{ MODKEY,		XK_Down,	focusstack,	{.i = +1 } },
	/* { MODKEY|ShiftMask,	XK_Down,	focusstack,	{.i = +1 } }, */
	{ MODKEY,		XK_Up,		focusstack,	{.i = -1 } },
	/* { MODKEY,		XK_Up,		focusstack,	{.i = -1 } }, */
	{ MODKEY,		XK_Left,	setmfact,	{.f = -0.05} },
	/* { MODKEY,		XK_Left,	setmfact,	{.f = -0.05} }, */
	{ MODKEY,		XK_Right,	setmfact,	{.f = +0.05} },
	/* { MODKEY,		XK_Right,	setmfact,	{.f = +0.05} }, */
	{ MODKEY,		XK_Escape,	togglebar,	{0} },
	{ MODKEY|ShiftMask,	XK_Escape,	spawn,		SHCMD("wifi_toggle.sh") },
        { MODKEY,		XK_Delete,	spawn,		SHCMD("$TERMINAL -e htop") },
	/* { MODKEY|ShiftMask,	XK_Delete,	spawn,		SHCMD("samedir.sh") }, */
	{ MODKEY,		XK_Return,	zoom,		{0} },
	/* { MODKEY|ShiftMask,	XK_Return,	spawn,		SHCMD("samedir.sh") }, */
	{ MODKEY,		XK_KP_Enter,	zoom,		{0} },
	{ MODKEY|ShiftMask,	XK_KP_Enter,	spawn,		SHCMD("samedir.sh") },
	{ MODKEY,		XK_Tab,		view,		{0} },
	/* { MODKEY|ShiftMask,	XK_backslash,	,		{} }, */
	{ MODKEY,		XK_space,	spawn,		SHCMD("samedir.sh") },
	{ MODKEY|ShiftMask,	XK_space,	togglefloating,	{0} },
	{ MODKEY,		XK_BackSpace,	view,		{.ui = ~0 } },
	{ MODKEY|ShiftMask,	XK_BackSpace,	tag,		{.ui = ~0 } },
	{ MODKEY,		XK_comma,	focusmon,	{.i = -1 } },
	{ MODKEY|ShiftMask,	XK_comma,	tagmon,		{.i = -1 } },
	{ MODKEY|ControlMask,	XK_comma,	tagswapmon,	{.i = +1 } },
	{ MODKEY,		XK_period,	focusmon,	{.i = +1 } },
	{ MODKEY|ShiftMask,	XK_period,	tagmon,		{.i = +1 } },
	{ MODKEY|ControlMask,	XK_period,	tagswapmon,	{.i = -1 } },
#if SCRATCHPADS
	{ MODKEY,		XK_semicolon,	togglescratch,	{.ui = 0 } },
#else
	{ MODKEY,		XK_semicolon,	spawn,		SHCMD("ibus-ui-emojier-plasma") },
#endif
	/* { MODKEY|ShiftMask,	XK_semicolon,	spawn,		SHCMD("pgrep plasma && ibus-ui-emojier-plasma") }, */
	{ MODKEY,		XK_grave,	spawn,		SHCMD("focus_window.sh") },
	/* { MODKEY|ShiftMask,	XK_grave,	spawn,		SHCMD("notification_toggle.sh") }, */
	{ MODKEY|ControlMask,	XK_grave,	spawn,		SHCMD("notification_toggle.sh") },
	{ MODKEY,		XK_Print,	spawn,		SHCMD("screenshot.sh -s") },    /* Area select. */
	{ MODKEY|ShiftMask,	XK_Print,	spawn,		SHCMD("screenshot.sh") },       /* Fullscreen */
	{ MODKEY,		XK_F2,		spawn,		SHCMD("firefox") },
	{ MODKEY|ShiftMask,	XK_F2,		spawn,		SHCMD("firefox -private-window") },

	/* Control Masks. */

	/* { CONTROLKEY,	XK_q,		spawn,		SHCMD("blur_lock.sh") }, */
	{ CONTROLKEY|ShiftMask,	XK_q,		spawn,		SHCMD("dmenu_input.sh -e") },

	/* No masks. */

	{ 0,			XK_Print,	spawn,		SHCMD("screenshot.sh -w") },    /* Current window. */
	{ 0,			XK_Caps_Lock,	spawn,		SHCMD("restartbar.sh") },
	{ 0,			XK_Num_Lock,	spawn,		SHCMD("restartbar.sh") },
	{ 0,		XF86XK_AudioRaiseVolume,spawn,		SHCMD("volume_change.sh -i") },
	{ 0,		XF86XK_AudioLowerVolume,spawn,		SHCMD("volume_change.sh -d") },
	{ 0,		    XF86XK_AudioMute,   spawn,		SHCMD("volume_change.sh -t") },
	{ 0,		    XF86XK_AudioMicMute,spawn,		SHCMD("volume_change.sh -mm") },
	{ 0,	      XF86XK_MonBrightnessDown,	spawn,		SHCMD("brightness_change.sh -dec") },
	{ 0,		XF86XK_MonBrightnessUp,	spawn,		SHCMD("brightness_change.sh -inc") },
	{ 0,		HOLDKEY,		holdbar,	{0} },
	TAGKEYS(		XK_1,		0)
	TAGKEYS(		XK_2,		1)
	TAGKEYS(		XK_3,		2)
	TAGKEYS(		XK_4,		3)
	TAGKEYS(		XK_5,		4)
	TAGKEYS(		XK_6,		5)
	TAGKEYS(		XK_7,		6)
	TAGKEYS(		XK_8,		7)
	TAGKEYS(		XK_9,		8)
	TAGKEYS(		XK_0,		9)
	TAGKEYS(		XK_KP_1,	0)
	TAGKEYS(		XK_KP_2,	1)
	TAGKEYS(		XK_KP_3,	2)
	TAGKEYS(		XK_KP_4,	3)
	TAGKEYS(		XK_KP_5,	4)
	TAGKEYS(		XK_KP_6,	5)
	TAGKEYS(		XK_KP_7,	6)
	TAGKEYS(		XK_KP_8,	7)
	TAGKEYS(		XK_KP_9,	8)
	TAGKEYS(		XK_KP_0,	9)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
#if STATUSCMD
	{ ClkStatusText,        0,              Button1,        spawn,          {.v = statuscmd } },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = statuscmd } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = statuscmd } },
#else
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
#endif
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

