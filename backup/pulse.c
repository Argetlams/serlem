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
	gpioSetMode(4, PI_OUTPUT);
	gpioServo(4, 1500);
	signal(SIGINT, sigintHandler);
	while (1)
	{
		gpioWrite(4, 1); /* on */
		sleep(1);
		gpioWrite(4, 0); /* off */
		sleep(1);
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
