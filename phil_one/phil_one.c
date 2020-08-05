#include "phil_one.h"

t_vars	*get_vars(void)
{
	static t_vars vars;
	return(&vars);
}

int		main(int argc, char **argv)
{
	t_vars *vars;

	if (argc != 5 && argc != 6)
		return (-1);
	vars = get_vars();
	if (!init(argc, argv) || !create_philo(vars))
		return (-1);
	while (1)
	{
		pthread_mutex_lock(&vars->alive);
		if (vars->n_alive == 0)//먹어야 하는 최소 횟수를 다 채워서 시뮬레이션 종료되는 경우
			break ;
		pthread_mutex_unlock(&vars->alive);
		pthread_mutex_lock(&vars->someone_died);
		if (vars->flag_died == 1)//한 명이라도 죽으면 시뮬레이션 종료 (못 먹어서 죽는 경우)
			return (free_all(0));
		pthread_mutex_unlock(&vars->someone_died);
		ft_usleep(5);
	}
	if (vars->flag_died == 0)//먹어야하는 최소횟수 다 채우면 종료
		ft_putstr("Every philosopher ate enough!\n");
	return(free_all(0));
}
