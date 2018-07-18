#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include "pulse.h"

void sigintHandler(int sig);

int main()
{
	ctrl_c_pressed = 0;
	if (gpioInitialise() < 0)
	{
		fprintf(stderr, "pigpio initialisation failed\n");
		return 1;
	}
	gpioSetMode(17, PI_OUTPUT);
	gpioSetMode(18, PI_INPUT);
	signal(SIGINT, sigintHandler);
	while (1)
	{
		if (gpioRead(18) == 1)
			gpioWrite(17, 1); /* on */
		else if (gpioRead(18) == 0)
			gpioWrite(17, 0); /* off */
		else if (gpioRead(18) == PI_BAD_GPIO)
			fprintf(stderr, "Read failed\n");
		if (ctrl_c_pressed)
		{
			printf("TRL+C is pressed\nterminating program\n");
			gpioTerminate();
			printf("Deallocating memory...\n");
			printf("Operation done, have a good day.\n");
			break;
		}
	}
   return 0;
}

void sigintHandler(int sig)
{
	signal(SIGINT, sigintHandler);
	ctrl_c_pressed = 1;
	(void)sig;
}
