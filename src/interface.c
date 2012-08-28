#include "interface.h"

#ifdef WINDOWS

#include <windows.h>
#include "map.h"

static HANDLE inputHandle;
static HANDLE outputHandle;
static HANDLE buffers[2];
static int u = 0;

#define EX  80
#define WHY 25
#define tableSize 256
static uint vkToKey[tableSize];
static uint asciiToKey[tableSize];
static CHAR_INFO one[EX*WHY];
static CHAR_INFO two[EX*WHY];
WORD white = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
void iface_init(void) {
	inputHandle  = GetStdHandle(STD_INPUT_HANDLE);
	outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(inputHandle,  ENABLE_PROCESSED_INPUT);
	SetConsoleMode(outputHandle, ENABLE_PROCESSED_OUTPUT);
	COORD bufferSize;
	bufferSize.X = EX;
	bufferSize.Y = WHY;
	SetConsoleScreenBufferSize(outputHandle, bufferSize);
	int i;
	for (i=0; i<EX*WHY; i++) {
		one[i].Char.AsciiChar = (i%2) ? '#' : '=';
		one[i].Attributes = white;
	}
	for (i=0; i<100; i++) {
		two[i].Char.AsciiChar = ' ';
		two[i].Attributes = white;
	}
	buffers[0] = CreateConsoleScreenBuffer(GENERIC_WRITE, 0, 0, CONSOLE_TEXTMODE_BUFFER, 0);
	buffers[1] = CreateConsoleScreenBuffer(GENERIC_WRITE, 0, 0, CONSOLE_TEXTMODE_BUFFER, 0);

	for (i = 0; i < tableSize; i++)
	{
		vkToKey[i] = KEY_UNKNOWN;
		asciiToKey[i] = KEY_UNKNOWN;
	}
	vkToKey[VK_NUMPAD0] = KEY_NUMPAD_0;
	vkToKey[VK_NUMPAD1] = KEY_NUMPAD_1;
	vkToKey[VK_NUMPAD2] = KEY_NUMPAD_2;
	vkToKey[VK_NUMPAD3] = KEY_NUMPAD_3;
	vkToKey[VK_NUMPAD4] = KEY_NUMPAD_4;
	vkToKey[VK_NUMPAD5] = KEY_NUMPAD_5;
	vkToKey[VK_NUMPAD6] = KEY_NUMPAD_6;
	vkToKey[VK_NUMPAD7] = KEY_NUMPAD_7;
	vkToKey[VK_NUMPAD8] = KEY_NUMPAD_8;
	vkToKey[VK_NUMPAD9] = KEY_NUMPAD_9;

	iface_swap();
}

void iface_cleanup(void) {SetConsoleActiveScreenBuffer(outputHandle);}

#define COORD2INDEX(x, y, xmax) ((x) + (xmax) * (y))
void iface_drawmap(map_t* m) {
	CHAR_INFO map[m->size[0]*m->size[1]];
	CHAR_INFO* c;
	COORD bufsize = {m->size[0], m->size[1]};//	COORD bufsize = {m->size[0], m->size[1]};
	COORD source = {0, 0};
	SMALL_RECT dest = {0, 0, m->size[0]-1, m->size[1]-1};
	int i, j; tile_t* t;
	for (i=0; i<m->size[0]; i++) {
		for (j=0; j<m->size[1]; j++) {
			t = map_get_tile(m, i, j);
			c = map + COORD2INDEX(i, j, m->size[0]);
			c->Char.AsciiChar = t->symbol;
			c->Attributes	 = t->colour;
		}
	}
	WriteConsoleOutput(buffers[u], map, bufsize, source, &dest);
}

void iface_swap() {
	SetConsoleActiveScreenBuffer(buffers[u]);
	u=(u+1)%2;
}

uint iface_next_key (void) {
	uint result;
	INPUT_RECORD in;
	DWORD length = 1;
	DWORD numberRead = 0;
	BOOL success = ReadConsoleInput(inputHandle, &in, length, &numberRead);
	if (success && numberRead == 1 && in.EventType == KEY_EVENT && in.Event.KeyEvent.bKeyDown) {
		WORD vKeycode = in.Event.KeyEvent.wVirtualKeyCode;
		CHAR ascii = in.Event.KeyEvent.uChar.AsciiChar;
		DWORD control = in.Event.KeyEvent.dwControlKeyState;
		result = vkToKey[vKeycode];
		if (result == KEY_UNKNOWN) {result = asciiToKey[(int)ascii];}
		if (result != KEY_UNKNOWN) {
			if ((control & CAPSLOCK_ON) || (control & SHIFT_PRESSED))			{result = result | KEY_SHIFT;}
			if ((control & LEFT_CTRL_PRESSED) || (control & RIGHT_CTRL_PRESSED)) {result = result | KEY_CONTROL;}
			if ((control & LEFT_ALT_PRESSED) || (control & RIGHT_ALT_PRESSED))   {result = result | KEY_ALT;}
		}
	}
	return result;
}
/////
#endif
