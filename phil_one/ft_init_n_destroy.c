#include "phil_one.h"

int	free_struct(void *s) //here the argument is void to free variables easily!
{
	if (s)
		free(s);
	s = 0;
	return (1);
}

int	free_all(int ret)
{
	t_vars *vars;
	int i;

	vars = get_vars();
	i = 0;
	while (i < vars->n_philo)
		pthread_mutex_destroy(&vars->forks[i++]);
	i = 0;
	while (i < vars->n_philo)
		pthread_mutex_destroy(&vars->eats[i++]);
	pthread_mutex_destroy(&vars->alive);
	pthread_mutex_destroy(&vars->print);
	pthread_mutex_destroy(&vars->someone_died);
	pthread_mutex_destroy(&vars->putdown);
	pthread_mutex_destroy(&vars->pickup);
	free_struct((void *)vars->philo);
	free_struct((void *)vars->eats);
	free_struct((void *)vars->forks);
	return (ret);
}

int	init_mutexes(t_vars *vars)
{
	int i;

	i = 0;
	vars->n_alive = vars->n_philo; //생존자 수 == 철학자 수
	if(!(vars->forks = malloc(sizeof(pthread_mutex_t) * vars->n_philo)))
		return (0); //포크 수에 구조체크기 * 철학자 수만큼 malloc
	if(!(vars->eats = malloc(sizeof(pthread_mutex_t) * vars->n_philo)))
		return (0); //먹는 행위에 구조체크기 * 철학자 수만큼 malloc
	//mutex 객체 초기화
	//성공시 0 return , 아래 if 조건 default가 != 0, 따라서 실패시 return(0)이라는 뜻
	i = 0;
	while (i < vars->n_philo)
		if (pthread_mutex_init(&vars->forks[i++], 0))
			return (0);
	i = 0;
	while (i < vars->n_philo)
		if (pthread_mutex_init(&vars->eats[i++], 0))
			return (0);
	if (pthread_mutex_init(&vars->pickup, 0))
		return (0);
	if (pthread_mutex_init(&vars->putdown, 0))
		return (0);
	if (pthread_mutex_init(&vars->alive, 0))
		return (0);
	if (pthread_mutex_init(&vars->print, 0))
		return (0);
	if (pthread_mutex_init(&vars->someone_died, 0))
		return (0);
	return (1);
}

int	init(int argc, char **argv)
{
	t_vars *vars;

	vars = get_vars();
	//arg 예외처리, 진짜 초기화는 init mutexes에서
	vars->n_philo = ft_atoi(argv[1]);
	vars->t_die = ft_atoi(argv[2]);
	vars->t_eat = ft_atoi(argv[3]);
	vars->t_sleep = ft_atoi(argv[4]);
	if (argc == 6)
	{
		if ((vars->n_must_eat = ft_atoi(argv[5])) <= 0)
			return (0);
	}
	else
		vars->n_must_eat = -1;
	if (vars->n_philo < 2)
		return (0);
	if (vars->t_die < 0 || vars->t_eat < 0 || vars->t_sleep < 0)
		return (0);
	if (!(init_mutexes(vars)))
		return (0);
	return (1);
}
