/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

#ifndef __console_toolchain_h__
#define __console_toolchain_h__

#include <stdbool.h>
#include "memoryHandleTGDS.h"
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "ipcfifoTGDS.h"


#define ENABLE_3D    (1<<3)
#define DISPLAY_ENABLE_SHIFT 8
#define DISPLAY_BG0_ACTIVE    (1 << 8)
#define DISPLAY_BG1_ACTIVE    (1 << 9)
#define DISPLAY_BG2_ACTIVE    (1 << 10)
#define DISPLAY_BG3_ACTIVE    (1 << 11)
#define DISPLAY_SPR_ACTIVE    (1 << 12)
#define DISPLAY_WIN0_ON       (1 << 13)
#define DISPLAY_WIN1_ON       (1 << 14)
#define DISPLAY_SPR_WIN_ON    (1 << 15)

#define BACKGROUND_SUB       (*((bg_attribute *)0x04001008))
#define BG_OFFSET_SUB ((bg_scroll *)(0x04001010))
#define BG_MAP_RAM_SUB(base)	((uint16*)(((base)*0x800) + 0x06200000))
#define BG_TILE_RAM_SUB(base)	((uint16*)(((base)*0x4000) + 0x06200000))
#define BG_BMP_RAM_SUB(base)	((uint16*)(((base)*0x4000) + 0x06200000))
#define SCREEN_BASE_BLOCK_SUB(n)	(((n)*0x800) + 0x06200000)
#define CHAR_BASE_BLOCK_SUB(n)		(((n)*0x4000)+ 0x06200000)
#define	BGCTRL_SUB				( (vuint16*)0x4001008)
#define	REG_BG0CNT_SUB		(*(vuint16*)0x4001008)
#define	REG_BG1CNT_SUB		(*(vuint16*)0x400100A)
#define	REG_BG2CNT_SUB		(*(vuint16*)0x400100C)
#define	REG_BG3CNT_SUB		(*(vuint16*)0x400100E)

//Recursive IO 
#define	REG_BGXCNT(i)			*(vuint16*)(0x04000008 + ((sint32)i*2))
#define	REG_BGXCNT_SUB(i)		*(vuint16*)(0x04001008 + ((sint32)i*2))

#define	REG_BGOFFSETS_SUB	( (vuint16*)0x4001010)
#define	REG_BG0HOFS_SUB		(*(vuint16*)0x4001010)
#define	REG_BG0VOFS_SUB		(*(vuint16*)0x4001012)
#define	REG_BG1HOFS_SUB		(*(vuint16*)0x4001014)
#define	REG_BG1VOFS_SUB		(*(vuint16*)0x4001016)
#define	REG_BG2HOFS_SUB		(*(vuint16*)0x4001018)
#define	REG_BG2VOFS_SUB		(*(vuint16*)0x400101A)
#define	REG_BG3HOFS_SUB		(*(vuint16*)0x400101C)
#define	REG_BG3VOFS_SUB		(*(vuint16*)0x400101E)

#define	REG_BG2PA_SUB		(*(vsint16*)0x4001020)
#define	REG_BG2PB_SUB		(*(vsint16*)0x4001022)
#define	REG_BG2PC_SUB		(*(vsint16*)0x4001024)
#define	REG_BG2PD_SUB		(*(vsint16*)0x4001026)

#define	REG_BG2X_SUB		(*(vsint32*)0x4001028)
#define	REG_BG2Y_SUB		(*(vsint32*)0x400102C)

#define	REG_BG3PA_SUB		(*(vsint16*)0x4001030)
#define	REG_BG3PB_SUB		(*(vsint16*)0x4001032)
#define	REG_BG3PC_SUB		(*(vsint16*)0x4001034)
#define	REG_BG3PD_SUB		(*(vsint16*)0x4001036)

#define	REG_BG3X_SUB		(*(vsint32*)0x4001038)
#define	REG_BG3Y_SUB		(*(vsint32*)0x400103C)


//Console uses 2D. used by REG_DISPCNT / REG_DISPCNT_SUB
typedef enum {
	MODE_0_2D = 0x10000, 
	MODE_1_2D = 0x10001, 
	MODE_2_2D = 0x10002, 
	MODE_3_2D = 0x10003, 
	MODE_4_2D = 0x10004, 
	MODE_5_2D = 0x10005, 
	MODE_6_2D = 0x10006
	//todo 3D and framebuffer modes
} VideoMode;

#define backgroundsPerEngine 4
typedef struct EngineBGStatus
{
	uint32 tile_base;
	uint32 map_base;
	sint32 BGNUM;
	uint32 REGBGCNT; 	//every backgroundsPerEngine unit
}EngineBGStatus;

typedef struct EngineStatus
{
	uint32 ENGINE_DISPCNT;
	EngineBGStatus EngineBGS[4];
}EngineStatus;

typedef enum {
	mainEngine = (0x1<<0), 
	subEngine = (0x0<<0)
} PPUEngine;

//each is a Console Instance. The idea is to re-use SnemulDS console render so we are not limited to a single background - tile format, but takeover an engine as whole for rendering.
typedef struct ConsoleInstance
{
	PPUEngine ppuMainEngine;
	uint16* gfx;
	EngineStatus ConsoleEngineStatus;
}ConsoleInstance;

//font
typedef struct
{
    int		width;
    int		height;
    int		bpp;
    uint8	*palette;   
    uint8   data[];
} t_GUIGlyph;

typedef struct
{
    int		width;
    int		height;
    int		bpp;
    uint8	*palette;   
    uint16   data[];
} t_GUIGlyph16;

typedef struct
{
    int height;
    int	offset;
    
    int space;
    int interline;

    t_GUIGlyph **glyphs;
} t_GUIFont;

// GUI Colors

#define GUI_PAL			216
//#define GUI_BLACK		0
#define GUI_BLACK		(GUI_PAL+0)
#define GUI_DARKGREY2	(GUI_PAL+1)
#define GUI_DARKGREY	(GUI_PAL+2)
#define GUI_GREY		(GUI_PAL+3)

#define GUI_DARKRED		(GUI_PAL+4)
#define GUI_RED			(GUI_PAL+5)
#define GUI_LIGHTRED	(GUI_PAL+6)

#define GUI_DARKGREEN	(GUI_PAL+12)
#define GUI_GREEN		(GUI_PAL+13)
#define GUI_LIGHTGREEN	(GUI_PAL+14)

#define GUI_DARKBLUE	(GUI_PAL+8)
#define GUI_BLUE		(GUI_PAL+9)
#define GUI_LIGHTBLUE	(GUI_PAL+10)

#define GUI_DARKYELLOW	(GUI_PAL+17)
#define GUI_YELLOW		(GUI_PAL+18)
#define GUI_LIGHTYELLOW	(GUI_PAL+19)

#define GUI_LIGHTGREY	(GUI_WHITE-2)
#define GUI_LIGHTGREY2	(GUI_WHITE-1)
#define GUI_WHITE		255


//GUI defs

#define GUI_EVENT			1
#define GUI_DRAW			2
#define GUI_COMMAND			3

#define GUI_EVENT_STYLUS	100
#define GUI_EVENT_BUTTON	101

#define GUI_EVENT_ENTERZONE	110
#define GUI_EVENT_LEAVEZONE	111
#define	GUI_EVENT_FOCUS		112
#define	GUI_EVENT_UNFOCUS	113

#define	EVENT_STYLUS_PRESSED 	1000
#define	EVENT_STYLUS_RELEASED	1001
#define	EVENT_STYLUS_DRAGGED 	1002

#define	EVENT_BUTTON_ANY	 	2000
#define	EVENT_BUTTON_PRESSED	2001
#define	EVENT_BUTTON_RELEASED	2002
#define	EVENT_BUTTON_HELD	 	2003

#define IMG_IN_MEMORY	0
#define IMG_IN_VRAM		1
#define IMG_NOLOAD		2

#define GUI_TEXT_ALIGN_CENTER	0
#define GUI_TEXT_ALIGN_LEFT		1
#define GUI_TEXT_ALIGN_RIGHT	2

#define GUI_ST_PRESSED			1
#define GUI_ST_SELECTED			2
#define GUI_ST_FOCUSED			4
#define GUI_ST_HIDDEN			8
#define GUI_ST_DISABLED			16

#define GUI_HANDLE_JOYPAD		1

#define GUI_PARAM(a) (void *)(a)
#define GUI_PARAM2(a, b) (void *)((((uint16)(a)) << 16) | ((uint16)(b)))
#define GUI_PARAM3(s, n, c) (void *)((s) | ((n) << 16) | ((c) << 24))

#define _STR(x) GUI.string[x]

typedef struct
{
	int x;
	int y;
	
	int	dx;
	int dy;
} t_GUIStylusEvent;

typedef struct
{
	uint32	buttons;
	uint32  pressed;
	uint32	repeated;
	uint32	released;
} t_GUIJoypadEvent;

typedef struct
{
	int	event;
	union
	{
		t_GUIStylusEvent stl;
		t_GUIJoypadEvent joy;
	};
		
} t_GUIEvent;

typedef struct s_GUIZone t_GUIZone;
typedef int (*t_GUIHandler)(t_GUIZone *zone, int message, int param, void *arg);

struct s_GUIZone
{
	int				x1;
	int 			y1;
	int 			x2;
	int				y2;

	uint8			id;	
	uint8			state;	
	uint16			keymask;

	t_GUIHandler	handler;
	void			*data;
	
	t_GUIFont		*font;
};

typedef struct
{
	uint16	width;
	uint8	height;
	uint8	flags;
	
	void	*data;
} t_GUIImage;

typedef struct
{
	int			nb;
	int			cnt;
	t_GUIImage	*img[];
} t_GUIImgList;

typedef struct
{
	int			nb_zones;
	int			curs;
	int			stylus_zone;
	int			flags;
	t_GUIHandler	handler;
	t_GUIImgList	*img_list;
	
	uint16		last_focus; // Number of focusable zones 
	uint16		incr_focus; // Increment factor for joypad focus
	
	t_GUIZone	zones[];
} t_GUIScreen;



typedef struct
{
// TODO : put in one flag field
	int	log;
	int exit;
	int hide;	
	int ScanJoypad; 
	
	uint16	*DSFrameBuffer; // Frame Buffer Layer
	uint16	*DSText; // Text Layer	/ not used
	uint16	*DSBack; // Back Text Layer;	/ not used
	uint16	*DSTileMemory;	// not used
	
	uint16	*Palette;
	
	t_GUIScreen	*screen;
	
	sint8	**string;
	
	t_GUIImgList	*img_list;
	
	uint16	printfy;
} t_GUI;

typedef struct
{
	t_GUIScreen	*scr;
	int			msg;
	int 		param;
	void		*arg;
} t_GUIMessage;


#endif


#ifdef __cplusplus
extern "C" {
#endif

extern	t_GUI	GUI;

extern ConsoleInstance DefaultConsole;	//default console
extern ConsoleInstance CustomConsole;	//project specific
extern ConsoleInstance * DefaultSessionConsole;

//weak symbols : the implementation of this is project-defined
extern __attribute__((weak))	bool InitProjectSpecificConsole();

extern bool InitializeConsole(ConsoleInstance * ConsoleInst);
extern void UpdateConsoleSettings(ConsoleInstance * ConsoleInst);
extern void SetEngineConsole(PPUEngine engine,ConsoleInstance * ConsoleInst);
extern void consoleClear(ConsoleInstance * ConsoleInst);
extern t_GUIFont katakana_12_font;

extern void	GUI_console_printf(int cx, int cy, sint8 *fmt, ...);
extern void	GUI_align_printf(int flags, sint8 *fmt, ...);
extern void GUI_printf(sint8 *fmt, ...);
extern void GUI_printf2(int cx, int cy, sint8 *fmt, ...);
extern void	GUI_clearScreen(int color);
extern void	GUI_clear();
extern volatile sint8 g_printfbuf[consolebuf_size];

//font 
extern t_GUIFont trebuchet_9_font;
extern t_GUIFont smallfont_7_font;

extern void _glyph_loadline_8(uint8 *dst, uint8 *data, int pos, int size, uint8 *pal);
extern void _glyph_loadline_2(uint8 *dst, uint8 *data, int pos, int size, uint8 *pal);
extern void _glyph_loadline_1(uint8 *dst, uint8 *data, int pos, int size, uint8 *pal);

extern int 	GUI_drawVChar(t_GUIZone *zone, t_GUIFont *font, uint16 x, uint16 y, int col, uint8 text);
extern uint8 g_katana_jisx0201_conv[];
extern int 	GUI_drawText(t_GUIZone *zone, uint16 x, uint16 y, int col, sint8 *text);
extern int 	GUI_getStrWidth(t_GUIZone *zone, sint8 *text);
extern int 	GUI_getFontHeight(t_GUIZone *zone);
extern int	GUI_getZoneTextHeight(t_GUIZone *zone);
extern int 	GUI_drawAlignText(t_GUIZone *zone, int flags, int y, int col, sint8 *text);
extern void clrscr();
#ifdef __cplusplus
}
#endif