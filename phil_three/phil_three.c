#include "phil_three.h"

//왜 ft_putstr_fd 를 쓰는지 알아내기!! ->재활용...

t_vars  *get_vars(void)
{
	static t_vars vars;
	return (&vars);
}

void	wait_and_kill(t_vars *vars)
{
	int i;
	int status;

	status = 0;
	//waitpid(pid_t pid, int *stat_loc, int options);
	while (waitpid(-1, &status, 0) > 0)
	{
		//정상종료되고 아사한 경우 exit code 1
		if (WIFEXITED(status) && (WEXITSTATUS(status) == 1))
		{
			i = 0;
			while (i < vars->n_philo)
				kill(vars->philo[i++].pid, SIGINT);
		}
		//정상종료되고 최소 먹는 횟수 채운 경우 exit code 0
		else if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
		{
			if (--vars->n_philo == 0)
			{
				ft_putstr_fd("Every philosopher ate enough\n", 1);
				return ;
			}
		}
	}
}

int	main(int argc, char **argv)
{
	t_vars *vars;

	if (argc != 5 && argc != 6)
		return (-1);
	vars = get_vars();
	if (!init(argc, argv) || !create_philo(vars))
		return (-1);
	wait_and_kill(vars);
	return (free_all(0, 0));
}
