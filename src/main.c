#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include "interface.h"

int main() {
	iface_init   ();
	iface_display();
	iface_swap();
	Sleep(1000);
	iface_display();
	iface_swap();
	Sleep(2000);
	iface_cleanup();
	return 0;
}
