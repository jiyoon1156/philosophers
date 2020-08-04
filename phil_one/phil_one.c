#include "phil_one.h"

unsigned long get_time(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, 0))
		return (0);
	//miliseconds(1000ms == 1sec)로 표현
	//sec-km / ms-m / microsec(usec)-mm
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

t_vars	*get_vars(void)
{
	static t_vars vars;
	return(&vars);
}


void	print_status_body(t_vars *vars, t_philo *philo, t_status status, char *phrase)
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

int	print_status(t_vars *vars, t_philo *philo, t_status status)
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
void    ft_usleep(unsigned long time)
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
int	taken_fork_and_eat(t_vars *vars, t_philo *philo)
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

void *philosophing(void *v_philo)
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

void *monitoring(void *v_philo)
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

int     free_struct(void *s) //here the argument is void to free variables easily!
{
	if (s)
		free(s);
	s = 0;
	return (1);
}

int free_all(int ret)
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

int	create_philo_even(t_vars *vars, unsigned long start_time)
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

int	create_philo_odd(t_vars *vars, unsigned long start_time)
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

int	create_philo(t_vars *vars)
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

int	main(int argc, char **argv)
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