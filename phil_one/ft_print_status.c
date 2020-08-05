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

void			print_status_body(t_vars *vars, t_philo *philo, t_status status, char *phrase)
{
	unsigned long	time;
	int				philo_no;

	//인덱스는 0부터 시작하니까
	philo_no = philo->p_idx + 1;
	pthread_mutex_lock((&vars->print));//print 스레드 lock
	time = get_time() - philo->start_time;//현재 시간 - 최초 시작 시간, 즉 흐른시간
	pthread_mutex_lock(&vars->someone_died);
	if (vars->flag_died)
	{
		pthread_mutex_unlock(&vars->someone_died);
		return ;
	}
	pthread_mutex_unlock(&vars->someone_died);
	ft_putnbr(time);
	ft_putstr(" ");
	ft_putnbr(philo_no);//철학자 번호
	ft_putstr(phrase);//뭐하고 있는지
	free(phrase);
	if (!(status == DIED))
		pthread_mutex_unlock(&vars->print);
}

int				print_status(t_vars *vars, t_philo *philo, t_status status)
{
	char *phrase;

	phrase = 0;
	if (status == THINKING)
		phrase = ft_strdup(" is thinking\n");
	else if (status == EATING)
		phrase = ft_strdup(" is eating\n");
	else if (status == SLEEPING)
		phrase = ft_strdup(" is sleeping\n");
	else if (status == DIED)
		phrase = ft_strdup(" died\n");
	else if (status == FORK_TAKEN)
		phrase = ft_strdup(" has taken a fork\n");
	if (phrase == 0)
		return (0);
	print_status_body(vars, philo, status, phrase);
	return (1);
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

int				taken_fork_and_eat(t_vars *vars, t_philo *philo)
{
	int idx;

	idx = philo->p_idx;
	
	pthread_mutex_lock(&vars->pickup);//포크 하나 들었어
	pthread_mutex_lock(&vars->forks[idx]);//해당 포크 lock 걸어
	print_status(vars, philo, FORK_TAKEN); //그 포크 taken 됨!!!
	pthread_mutex_lock(&vars->forks[(idx + 1) % vars->n_philo]);//바로 옆에 있는 포크 lock 걸게 함
	print_status(vars, philo, FORK_TAKEN);//그것도 포크 taken 됨!!!
	pthread_mutex_unlock(&vars->pickup);//두개 들었으니까 pickup 은 lock 해제

	pthread_mutex_lock(&vars->eats[idx]);//이제 먹는거 lock 걸어
	philo->last_eat_time = get_time();//마지막으로 먹은 시간 구해
	pthread_mutex_unlock(&vars->eats[idx]);//구했으니 lock 해제
	print_status(vars, philo, EATING);// 이제 먹음!!!
	ft_usleep(vars->t_eat);//먹는 행동을 한다

	pthread_mutex_lock(&vars->putdown);//포크 이제 내려놔
	pthread_mutex_unlock(&vars->forks[idx]);//해당 포크 lock 해제
	pthread_mutex_unlock(&vars->forks[(idx + 1) % vars->n_philo]);//바로 옆에 있는 포크 lock 해제
	pthread_mutex_unlock(&vars->putdown);//포크 내려놨으니까 putdown은 lock 해제
	if ((++(philo->n_eat) == vars->n_must_eat))//최소 먹어야 하는 숫자 채우면 시뮬레이션 끝내
		return (0);
	return (1);
}
