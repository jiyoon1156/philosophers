#include "phil_one.h"

unsigned long	get_time(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, 0))
		return (0);
	//miliseconds(1000ms == 1sec)로 표현
	//sec-km / ms-m / microsec(usec)-mm
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void			ft_usleep(unsigned long time)
{
	unsigned long start;
	unsigned long time_elapsed;

	start = get_time();
	while (1)
	{
		time_elapsed = get_time() - start;
		if (time_elapsed >= time)
			break ;
		usleep(1);
	}
}
