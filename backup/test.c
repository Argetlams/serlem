#include <stdio.h>

int		main()
{
	int		i;
	char	byte;

	byte = 0x48;
	printf("byte = %d\n", byte);
	byte = byte << 1;
	byte = byte | 1;
	printf("first byte = %d\n", byte);
	for (i = 0; i < 8; i++)
		printf("byte = %d\n", (byte << i) & 0x80);
}
