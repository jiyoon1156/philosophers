#include "phil_two.h"

t_vars	*get_vars(void)
{
	static t_vars vars;
	return (&vars);
}

int		main(int argc, char **argv)
{
	t_vars	*vars;

	if (argc != 5 && argc != 6)
		return (-1);
	vars = get_vars();
	if (!init(argc, argv) || !create_philo(vars))
		return (-1);
	while (1)
	{
		if ((sem_wait(vars->alive) == -1))
			return (free_all("error: sem_wait\n", -1));
		if (vars->n_alive == 0)//create philo에서 철학자수만큼 philosophing, monitoring 하기 때문에 최소먹은 횟수를 다 채우면 n_alive가 0이 될 수밖에 없음
			break ;
		if ((sem_post(vars->alive) == -1))
			return (free_all("error: sem_post\n", -1));
		// if ((sem_wait(vars->someone_died) == -1))
		// 	return (free_all("error: sem_wait\n", -1));
		if (vars->flag_died == 1)
			return (ft_unlink(1));
		// if ((sem_post(vars->someone_died) == -1))
		// 	return (free_all("error: sem_post\n", -1));
		ft_usleep(10);
	}
	if (vars->flag_died == 0)
		ft_putstr_fd("Every philosopher ate enough!\n", 1);
	return (free_all(0, 0));
}
