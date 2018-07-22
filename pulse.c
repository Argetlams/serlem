#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include "pulse.h"
#include <stdlib.h>
#include <pthread.h>

static void		close_all(t_data *data)
{
	printf("Terminating gpio operations.\n");
	gpioWrite(data->gpioclk, 0);
	gpioWrite(data->gpiodata, 0);
	printf("Shutting down leds\n");
	gpioWrite(26, 0);
	gpioTerminate();
	printf("Unallocating memory slots.\n");
	printf("Operation done, have a good day.\n");
}

void			*f_input(void *dta)
{
	char		c;

	(void)dta;
	while (1)
	{
		read(STDIN_FILENO, &c, 1);
		if (c == 27)
		{
			ctrl_c_pressed = 1;
			break;
		}
		if (c == 'a')
		{
			if (a_is_pressed == 1)
				a_is_pressed = 0;
			else
				a_is_pressed = 1;
		}
	}
	return (NULL);
}

static t_data		initialise()
{
	t_data		data;

	if (gpioInitialise() < 0)
	{
		fprintf(stderr, "pigpio initialisation failed\n");
		exit(EXIT_FAILURE);
	}
	ctrl_c_pressed = 0;
	a_is_pressed = 0;
	data.gpioclk = 27;
	data.gpiodata = 17;
	gpioSetMode(data.gpioclk, PI_OUTPUT);
	gpioWrite(data.gpioclk, HIGH);
	gpioWrite(data.gpiodata, HIGH);
	data.av = (int *)malloc(sizeof(int) * IT_MAX);
	set_input_mode();
	return (data);
}

int				main()
{
	t_data		data;
	pthread_t	input_thread;
	char		input_reg;
	int			i;
	char		res;

	data = initialise();
	pthread_create(&input_thread, NULL, &f_input, &data);
	i = 0;
	gpioDelay(10);
	while (1)
	{
		if (a_is_pressed == 1)
			input_reg = 1;
		else
			input_reg = 0;
		data.res = read_analog(input_reg, data.gpioclk, data.gpiodata, 8);
		data.av[i] = data.res;
		i >= 10 ? i = 0 : i++;
		res = average(data.av, IT_MAX);
		printf("a = %d rec = %d\n", a_is_pressed, res);
		gpioDelay(500);
		gpioPWM(26, res);
		gpioDelay(500);
		if (ctrl_c_pressed)
		{
			close_all(&data);
			break;
		}
	}
   return (0);
}
