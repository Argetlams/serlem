#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include "pulse.h"

void sigintHandler(int sig);

int main()
{
	int		i;
	int		tmp;

	i = 16;
	tmp = 0;
	ctrl_c_pressed = 0;
	if (gpioInitialise() < 0)
	{
		fprintf(stderr, "pigpio initialisation failed\n");
		return 1;
	}
	while (i <= 26)
	{
		if (gpioSetMode(i, PI_OUTPUT) != 0)
		{
			printf("pin mode failed. error : PI_OUTPUT FAILED\n");
			return (0);
		}
		else
		{
			printf("Turning on pin %d\n", i);
			gpioWrite(i, 1);
		}
		i++;
	}
	signal(SIGINT, sigintHandler);
	i = 16;
	while (1)
	{
		gpioWrite(i++, tmp);
		gpioDelay(10000);
		if (i > 26)
		{
			i = 16;
			if (tmp == 0)
				tmp = 1;
			else
				tmp = 0;
		}
		if (ctrl_c_pressed)
		{
			printf("TRL+C is pressed\nterminating program\n");
			printf("Deallocating memory...\n");
			printf("Turning off leds...\n");
			i = 16;
			while (i <= 26)
				gpioWrite(i++, 1);
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
