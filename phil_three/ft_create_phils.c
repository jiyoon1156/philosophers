#include "phil_three.h"

void    *monitoring(void *v_philo)
{
	t_vars  *vars;
	t_philo *philo;

	philo = (t_philo *)v_philo;
	vars = get_vars();
	while (1)
	{
		if ((sem_wait(vars->eats) == -1))
			ft_error("error: sem_wait\n", 1);
		if (get_time() - philo->last_eat_time > (unsigned)vars->t_die)
		{
			print_status(vars, philo, DIED, 1);
			exit(1);
		}
		if ((sem_post(vars->eats) == -1))
			ft_error("error: sem_wait\n", 1);
		ft_usleep(2000);
	}
	return (philo);
}

void    *philosophing(void *v_philo)
{
	t_vars  *vars;
	t_philo *philo;

	vars = get_vars();
	philo = (t_philo *)v_philo;
	if (pthread_create(&philo->m_thread, 0, &monitoring, philo))
		exit(ft_error("Error: cannot create pthread", 1));
	if (pthread_detach((philo->m_thread)))
		exit(ft_error("Error: cannot create pthread", 1));
	while (1)
	{
		print_status(vars, philo, THINKING, 0);
		if (!taken_fork_and_eat(vars, philo))//(taken_fork_and_eat(vars,philo) == 0) 즉, 최소먹는 횟수를 다 채웠다면
			exit(0);
			//break ;
		print_status(vars, philo, SLEEPING, 0);
		ft_usleep(vars->t_sleep);
	}
	exit(0);
	// if ((sem_wait(vars->alive) == -1))
	// 	ft_error("error: sem_wait\n", 1);
	// vars->n_alive--;
	// if ((sem_post(vars->alive) == -1))
	// 	ft_error("error: sem_post\n", 1);
	// return (philo);
}

int     create_philo(t_vars *vars)
{
	unsigned long	start_time;
	int				i;

	if (!(vars->philo = malloc(sizeof(t_philo) * vars->n_philo)))
		return (0);
	start_time = get_time();
	i = 0;
	while (i < vars->n_philo)
	{
		vars->philo[i].p_idx = i;
		vars->philo[i].n_eat = 0;
		vars->philo[i].start_time = start_time;
		vars->philo[i].last_eat_time = vars->philo[i].start_time;
		//철학자마다 fork를 해서 프로세스 생성, 여기선 철학자가 곧 프로세스
		//근데 main process는 철학자가 아니어야 한다?!
		if ((vars->philo[i].pid = fork()) == 0)
			philosophing(&vars->philo[i]);
		i++;
	}
	return (1);
}
