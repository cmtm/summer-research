#define UART 0x10009000

void kernel_main() {
	volatile char* str = "hi there!";
	for (volatile int i = 0; i < 9; i++)
		*((char*) UART) = str[i];
}
