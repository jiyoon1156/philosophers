#include "phil_one.h"

void	*philosophing(void *v_philo)
{
	t_vars *vars;
	t_philo *philo;

	vars = get_vars();
	philo = (t_philo *)v_philo;
	while (1)
	{
		print_status(vars, philo, THINKING);//생각한다고 말해
		if (!taken_fork_and_eat(vars, philo))//포크를 들고 먹지 않으면 break
			break ;
		print_status(vars, philo, SLEEPING);//잠잔다고 말해
		ft_usleep(vars->t_sleep);//잠을 자
	}
	pthread_mutex_lock(&vars->alive);//삶과 죽음을 주관하는 함수를 lock
	vars->n_alive -= 1;//먹어야 하는 최소횟수 다 채우고 죽음(죽을 때 되어서 죽었다)
	pthread_mutex_unlock(&vars->alive);//삶과 죽음을 주관하는 함수를 lock 해제
	return (philo);
}

void	*monitoring(void *v_philo)
{
	t_vars *vars;
	t_philo *philo;

	philo = (t_philo *)v_philo;
	vars = get_vars();
	while(1)
	{
		pthread_mutex_lock(&vars->eats[philo->p_idx]);//먹는 스레드 lock
		//(현재 - 마지막 식사시간)이 죽을때까지의 시간보다 크다면
		if (get_time() - philo->last_eat_time > (unsigned long)vars->t_die)
		{
			print_status(vars, philo, DIED); //죽는다 프린트
			pthread_mutex_lock(&vars->someone_died);//죽음의 함수 lock
			vars->flag_died = 1; //사망 플래그 세우고 죽어.
			pthread_mutex_unlock(&vars->someone_died); //죽음의 함수 unlock
		}
		pthread_mutex_unlock(&vars->eats[philo->p_idx]);//먹는 스레드 unlock
		ft_usleep(5);//모니터링 하는 주기
	}
}

int		create_philo_even(t_vars *vars, unsigned long start_time)
{
	int i;
	int j;

	i = 0;
	while ((j = 2 * i) < vars-> n_philo)
	{
		vars->philo[j].p_idx = j;
		vars->philo[j].n_eat = 0;
		vars->philo[j].start_time = start_time;
		vars->philo[j].last_eat_time = vars->philo[j].start_time;
		//philosophing 하는 스레드 따로
		if (pthread_create(&vars->philo[j].thread, 0, &philosophing, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: cannot create pthread"));
		//monitoring 하는 스레드 따로
		if (pthread_create(&vars->philo[j].m_thread, 0, &monitoring, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: cannot create pthread"));
		i++;
	}
	return (1);
}

int		create_philo_odd(t_vars *vars, unsigned long start_time)
{
	int i;
	int j;

	i = 0;
	while ((j = 2 * i + 1) < vars-> n_philo)
	{
		vars->philo[j].p_idx = j;
		vars->philo[j].n_eat = 0;
		vars->philo[j].start_time = start_time;
		vars->philo[j].last_eat_time = vars->philo[j].start_time;
		if (pthread_create(&vars->philo[j].thread, 0, &philosophing, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread"));
		//스레드가 독립적으로 동작, 나는 기다려주지 않으니 끝나면 알아서 끝내도록 해라
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_create(&vars->philo[j].m_thread, 0, &monitoring, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: cannot create pthread"));
		i++;
	}
	return (1);
}

int		create_philo(t_vars *vars)
{
	unsigned long start_time;

	if (!(vars->philo = malloc(sizeof(t_philo) * vars->n_philo)))
		return (0);
	start_time = get_time();
	if (!(create_philo_even(vars, start_time)))
		return (0);
	if (!(create_philo_odd(vars, start_time)))
		return (0);
	return (1);
}
