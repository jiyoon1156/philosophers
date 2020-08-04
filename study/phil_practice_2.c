#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

typedef struct		s_philo
{
	pthread_t		thread;
	pthread_t		m_thread;
	int				p_idx;
	int				n_eat;
	unsigned long	start_time;
	unsigned long	last_eat_time;
}					t_philo;

typedef struct		s_vars
{
	int				n_philo;
	int				t_die;
	int				t_eat;
	int				t_sleep;
	int				n_must_eat;
	int				n_alive;
	int				flag_died;
	t_philo			*philo;
	sem_t			*forks;
	sem_t			*eats;
	sem_t			*pickup;
	sem_t			*putdown;
	sem_t			*alive;
	sem_t			*print;
	sem_t			*print_error;
	sem_t			*someone_died;
}					t_vars;

typedef enum		e_status
{
	THINKING,
	EATING,
	SLEEPING,
	FORK_TAKEN,
	DIED
}
					t_status;
//phil_one 이랑 차이점: mutex -> semaphore 로 변경
//mutex lock == sem wait, mutex unlock == sem post

t_vars	*get_vars(void)
{
	static t_vars vars;
	return(&vars);
}

int ft_strlen(char *s)
{
	int i;

	i = 0;
	while (s[i] != '\0')
		++i;
	return (i);
}

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

unsigned long    get_time(void)
{
	struct timeval  tv;

	if (gettimeofday(&tv, NULL))
		return (0);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000); // in ms.
}

int ft_error(char *str, int ret)
{
	t_vars *vars;
	
	vars = get_vars();
	sem_wait(vars->print_error);
	write(2, str, ft_strlen(str));
	sem_post(vars->print_error);
	return (ret);
}

void	ft_putnbr(long n)
{
	long	nb;

	nb = n;
	if (nb < 0)
	{
		ft_putchar('-');
		nb *= -1;
	}
	if (nb >= 10)
	{
		ft_putnbr(nb / 10); 
		ft_putnbr(nb % 10);
	}
	else
		ft_putchar(nb + '0');
}
char	*ft_strdup(const char *s1)
{
	int		i;
	int		length;
	char	*result;

	length = 0;
	while (s1[length])
		length++;
	if (!(result = (char *)malloc(sizeof(char) * (length + 1))))
		return (0);
	i = 0;
	while (i < length)
	{
		result[i] = s1[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}

void ft_putstr(char *str)
{
	write(1, str, ft_strlen(str));
}

void ft_putstr_fd(char *str, int fd)
{
	write(fd, str, ft_strlen(str));
}

int	ft_atoi(const char *str)
{
	int sign;
	int result;
	int i;

	i = 0;
	while ((9 <= str[i] && str[i] <= 13) || str[i] == 32)
		i++;
	sign = 1;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign *= -1;
		i++;
	}
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
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

void	clean_shm(void)
{
	sem_unlink("/forks");
	sem_unlink("/eats");
	sem_unlink("/pickup");
	sem_unlink("/putdown");
	sem_unlink("/print");
	sem_unlink("/alive");
	sem_unlink("/someone_died");
	sem_unlink("/print_error");
}

int						ft_unlink(int ret)
{
	if ((sem_unlink("/forks") == -1))
		ft_error("error: sem_unlink\n", 1);
	if ((sem_unlink("/eats") == -1))
		ft_error("error: sem_unlink\n", 1);
	if ((sem_unlink("/pickup") == -1))
		ft_error("error: sem_unlink\n", 1);
	if ((sem_unlink("/putdown") == -1))
		ft_error("error: sem_unlink\n", 1);
	if ((sem_unlink("/print") == -1))
		ft_error("error: sem_unlink\n", 1);
	if ((sem_unlink("/print_error") == -1))
		ft_error("error: sem_unlink\n", 1);
	if ((sem_unlink("/alive") == -1))
		ft_error("error: sem_unlink\n", 1);
	if ((sem_unlink("/someone_died") == -1))
		ft_error("error: sem_unlink\n", 1);
	return (ret);
}

int	init_semaphore(t_vars *vars)
{
	vars->n_alive = vars->n_philo;
	clean_shm();
	if ((vars->forks = sem_open("/forks", O_CREAT, 0660, vars->n_philo)) == SEM_FAILED)
		return (0);
	if ((vars->eats = sem_open("/eats", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->pickup = sem_open("/pickup", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->putdown = sem_open("/putdown", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->print = sem_open("/print", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->alive = sem_open("/alive", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->someone_died = sem_open("/someone_died", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->print_error = sem_open("/print_error", O_CREAT, 0660, 1)) == SEM_FAILED)
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
	if (!(init_semaphore(vars)))
		return (0);
	return (1);
}

void	print_status_body(t_vars *vars, t_philo *philo, t_status status, char *phrase)
{
	unsigned long	time;
	int				philo_no;

	//인덱스는 0부터 시작하니까
	philo_no = philo->p_idx + 1;
	time = get_time() - philo->start_time;//현재 시간 - 최초 시작 시간, 즉 흐른시간
	if ((sem_wait(vars->someone_died) == -1))
		ft_error("error: sem_wait\n", 1);
	if (vars->flag_died)
	{
		if ((sem_post(vars->someone_died) == -1))
			ft_error("error: sem_post\n", 1);
		return ;
	}
	if ((sem_post(vars->someone_died) == -1))
		ft_error("error: sem_post\n", 1);
	if ((sem_wait(vars->print) == -1))
		ft_error("error: sem_wait\n", 1);
	ft_putnbr(time);
	ft_putstr_fd(" ", 1);
	ft_putnbr(philo_no);//철학자 번호
	ft_putstr_fd(phrase, 1);//뭐하고 있는지
	if (!(status == DIED))
	{
		if ((sem_post(vars->print) == -1))
			ft_error("error: sem_post\n", 1);
	}
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
	free(phrase);
	return (1);
}

int	taken_fork_and_eat(t_vars *vars, t_philo *philo)
{
	if ((sem_wait(vars->pickup) == -1))
		ft_error("error: sem_wait\n", 1);
	if ((sem_wait(vars->forks) == -1))
		ft_error("error: sem_wait\n", 1);
	print_status(vars, philo, FORK_TAKEN);
	if ((sem_wait(vars->forks) == -1))
		ft_error("error: sem_wait\n", 1);
	print_status(vars, philo, FORK_TAKEN);
	if ((sem_post(vars->pickup) == -1))
		ft_error("error: sem_post\n", 1);
	if ((sem_wait(vars->eats) == -1))
		ft_error("error: sem_wait\n", 1);
	philo->last_eat_time = get_time();
	if ((sem_post(vars->eats) == -1))
		ft_error("error: sem_post\n", 1);
	print_status(vars, philo, EATING);
	ft_usleep(vars->t_eat);
	if ((sem_wait(vars->putdown) == -1))
		ft_error("error: sem_wait\n", 1);
	if ((sem_post(vars->forks) == -1) || (sem_post(vars->forks) == -1))
		ft_error("error: sem_wait\n", 1);
	if ((sem_post(vars->putdown) == -1))
		ft_error("error: sem_post\n", 1);
	if ((++(philo->n_eat) == vars->n_must_eat))
		return (0);
	return (1);
}
// ㅠ 
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
	if ((sem_wait(vars->alive) == -1))//삶과 죽음을 주관하는 함수를 lock
		ft_error("error: sem_wait\n", 1);
	// vars->n_alive--;
	vars->n_alive -= 1;
	if ((sem_post(vars->alive) == -1))//삶과 죽음을 주관하는 함수를 lock 해제
		ft_error("error: sem_post\n", 1);
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
		if ((sem_wait(vars->eats) == -1))//먹는 스레드 lock
			ft_error("error: sem_wait\n", 1);
		//(현재 - 마지막 식사시간)이 죽을때까지의 시간보다 크다면
		if (get_time() - philo->last_eat_time > (unsigned)vars->t_die)
		{
			print_status(vars, philo, DIED); //죽는다 프린트
			if ((sem_wait(vars->someone_died) == -1))//죽음의 함수 lock
				ft_error("error: sem_wait\n", 1);
			vars->flag_died = 1; //사망 플래그 세우고 죽어.
			if ((sem_post(vars->someone_died) == -1))//죽음의 함수 unlock
				ft_error("error: sem_post\n", 1);
		}
		if ((sem_post(vars->eats) == -1))//먹는 스레드 unlock
			ft_error("error: sem_post\n", 1);
		ft_usleep(200);//모니터링 하는 주기
	}
	return (philo);
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
			return (ft_error("Error: cannot create pthread", 0));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: cannot create pthread", 0));
		//monitoring 하는 스레드 따로
		if (pthread_create(&vars->philo[j].m_thread, 0, &monitoring, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread", 0));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: cannot create pthread", 0));
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
			return (ft_error("Error: cannot create pthread", 0));
		//스레드가 독립적으로 동작, 나는 기다려주지 않으니 끝나면 알아서 끝내도록 해라
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: cannot create pthread", 0));
		if (pthread_create(&vars->philo[j].m_thread, 0, &monitoring, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread", 0));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: cannot create pthread", 0));
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

int     free_struct(void *s) //here the argument is void to free variables easily!
{
	if (s)
		free(s);
	s = 0;
	return (1);
}

int free_all(char *s, int ret)
{
	t_vars *vars;

	if (s != 0)
		ft_putstr_fd(s, 2);
	vars = get_vars();
	sem_close(vars->forks);
	sem_close(vars->eats);
	sem_close(vars->pickup);
	sem_close(vars->putdown);
	sem_close(vars->alive);
	sem_close(vars->print);
	sem_close(vars->someone_died);
	sem_close(vars->print_error);
	ft_unlink(0);
	free(vars->philo);

	return (ret);
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
		if ((sem_wait(vars->alive) == -1))
			return (free_all("error: sem_wait\n", -1));
		if (vars->n_alive == 0)//먹어야 하는 최소 횟수를 다 채워서 시뮬레이션 종료되는 경우
			break ;
		if ((sem_post(vars->alive) == -1))
			return (free_all("error: sem_post\n", -1));
		if ((sem_wait(vars->someone_died) == -1))
			return (free_all("error: sem_wait\n", -1));
		if (vars->flag_died == 1)//한 명이라도 죽으면 시뮬레이션 종료 (못 먹어서 죽는 경우)
			return (free_all(0, 0));
		if ((sem_post(vars->someone_died) == -1))
			return (free_all("error: sem_wait\n", -1));
		ft_usleep(100);
	}
	//putstr_fd로 쓰는 이유??이걸 쓰지 않으면 error:sem_wait, post 이런게 뜬다.
	if (vars->flag_died == 0)//먹어야하는 최소횟수 다 채우면 종료
		ft_putstr_fd("Every philosopher ate enough!\n", 1);
	return(free_all(0, 0));
}