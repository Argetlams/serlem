#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include "pulse.h"

void sigintHandler(int sig);

int main()
{
	int		handle;
	unsigned char	val;

	ctrl_c_pressed = 0;
	if (gpioInitialise() < 0)
	{
		fprintf(stderr, "pigpio initialisation failed\n");
		return (-1);
	}
	if ((handle = i2cOpen(1, 0x48, 0)) < 0)
	{
		fprintf(stderr, "i2c handle initialisation failed");
		gpioTerminate();
		return (-1);
	}
	else
		printf("Handle nb %d opened\n", handle);
//	gpioSetMode(17, PI_OUTPUT);
	gpioSetPWMrange(17, 255);
	gpioPWM(17, 0);
	signal(SIGINT, sigintHandler);
	while (1)
	{
		val = i2cReadByte(handle);
		printf("read byte = %d\n", val);
		gpioPWM(17, val);
		gpioDelay(100);
		if (ctrl_c_pressed)
		{
			printf("TRL+C is pressed\nterminating program\n");
			printf("Closing i2c handle %d...\n", handle);
			i2cClose(handle);
			printf("Terminating gpio operations.\n");
			gpioTerminate();
			printf("Operation done, have a good day.\n");
			break;
		}
	}
   return (0);
}

void sigintHandler(int sig)
{
	signal(SIGINT, sigintHandler);
	ctrl_c_pressed = 1;
	(void)sig;
}
