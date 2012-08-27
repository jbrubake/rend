#include "interface.h"

#ifdef WINDOWS

#include <windows.h>

enum Key
{
  // Simple keys
  KEY_SIMPLE_BEGIN = 0,

  KEY_UNKNOWN = 0,
  KEY_NUMPAD_0, KEY_NUMPAD_1,
  KEY_NUMPAD_2, KEY_NUMPAD_3,
  KEY_NUMPAD_4, KEY_NUMPAD_5,
  KEY_NUMPAD_6, KEY_NUMPAD_7,
  KEY_NUMPAD_8, KEY_NUMPAD_9,

  KEY_SIMPLE_END,
  KEY_SIMPLE_MASK = 0x0000ffff,
  // Flags
  KEY_SHIFT =       0x00010000,
  KEY_CONTROL =     0x00020000,
  KEY_ALT =         0x00040000,

  KEY_FLAG_MASK =   0xffff0000
};

static HANDLE inputHandle;
static HANDLE outputHandle;
static HANDLE buffers[2];
static int u = 0;

#define EX  80
#define WHY 25

static CHAR_INFO one[EX*WHY];
static CHAR_INFO two[10*10];
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
	iface_swap();
}

void iface_cleanup(void) {}

void iface_display() {
	static int v = 0;
	COORD bufsize = {EX, WHY};
	COORD source = {0, 0};
	SMALL_RECT dest = {0, 0, EX-1, WHY-1};
	WriteConsoleOutput(buffers[u], one, bufsize, source, &dest);
	if (v) {WriteConsoleOutput(buffers[u], two, bufsize, source, &(SMALL_RECT){5,5, 15, 15});}
	v++;
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
		if (result == KEY_UNKNOWN) {result = asciiToKey[ascii];}
		if (result != KEY_UNKNOWN) {
			if ((control & CAPSLOCK_ON) || (control & SHIFT_PRESSED))            {result = result | KEY_SHIFT;}
			if ((control & LEFT_CTRL_PRESSED) || (control & RIGHT_CTRL_PRESSED)) {result = result | KEY_CONTROL;}
			if ((control & LEFT_ALT_PRESSED) || (control & RIGHT_ALT_PRESSED))   {result = result | KEY_ALT;}
		}
	}
	return result;
}
/////
#endif
