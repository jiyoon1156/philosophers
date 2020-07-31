#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>

typedef struct  	s_philo
{
	int             p_idx;
	pthread_t       thread;
	pthread_t       m_thread;
	int             n_eat;
	unsigned long   start_time;
	unsigned long   last_eat_time;
}               	t_philo;

typedef struct  	s_vars
{
	int     		n_philo;
	int     		t_die;
	int     		t_eat;
	int     		t_sleep;
	int     		n_must_eat;
	int     		n_alive;
	int     		flag_died;
	t_philo 		*philo;
	pthread_mutex_t *forks;
	pthread_mutex_t *eats;
	pthread_mutex_t pickup;
	pthread_mutex_t putdown;
	pthread_mutex_t alive;
	pthread_mutex_t print;
	pthread_mutex_t someone_died;
}               	t_vars;

typedef enum	e_status
{
	THINKING,
	EATING,
	SLEEPING,
	FORK_TAKEN,
	DIED
}               t_status;

int ft_strlen(char *s)
{
	int count;

	count = 0;
	while (s[count] != '\0')
		++count;
	return (count);
}

void ft_putchar(char c)
{
	write(1, &c, 1);
}

void ft_putstr(char *str)
{
	write(1, str, ft_strlen(str));
}

void	ft_putnbr(long n)
{
	if (n >= 0 && n < 10)
		ft_putchar(n + '0');
	else if (n >= 10)
	{
		ft_putnbr(n / 10);
		ft_putchar(n % 10 + '0');
	}
	else
	{
		if (n == -2147483648)
			write(1, "-2147483648", 11);
		else
		{
			ft_putchar('-');
			n = n * -1;
			ft_putnbr(n);
		}
	}
}

char	*ft_strdup(const char *s)
{
	int		len;
	int		i;
	char	*temp;

	len = 0;
	while (s[len] != '\0')
		++len;
	temp = (char *)malloc(sizeof(char) * (len + 1));
	if (temp == 0)
		return (0);
	i = 0;
	while (i < len)
	{
		temp[i] = s[i];
		++i;
	}
	temp[i] = '\0';
	return (temp);
}

static int	ft_is_space(const char c)
{
	if (c == ' ' || c == '\f' || c == '\n' ||\
		c == '\r' || c == '\t' || c == '\v')
		return (1);
	return (0);
}

static int	ft_is_numeric(const char c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int			ft_atoi(const char *str)
{
	int res;
	int i;
	int sign;

	res = 0;
	i = 0;
	sign = 1;
	while (str[i] != '\0' && ft_is_space(str[i]))
		++i;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = sign * -1;
		++i;
	}
	while (str[i] != '\0' && ft_is_numeric(str[i]))
	{
		res = res * 10 + (str[i] - 48);
		++i;
	}
	return (sign * res);
}

t_vars  *get_vars(void)
{
	static t_vars vars;
	//static으로 선언하면 함수 내에서 부를 수 있다! 굳이 인자로 계속 넘겨주지 않아도 된다.
	//minishell signal 함수 처리 때도 썼으면 괜찮았을 듯
	return (&vars);
}

int     init_mutexes(t_vars *vars)
{
	int     i;

	vars->n_alive = vars->n_philo;
	if (!(vars->forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * vars->n_philo)))
		return (0);
	if (!(vars->eats = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * vars->n_philo)))
		return (0);
	i = 0;
	while (i < vars->n_philo)
		if (pthread_mutex_init(&vars->forks[i++], NULL)) // mutex 객체 초기화
			return (0);
	i = 0;
	while (i < vars->n_philo)
		if (pthread_mutex_init(&vars->eats[i++], NULL))
			return (0);
	if (pthread_mutex_init(&vars->pickup, NULL))
		return (0);
	if (pthread_mutex_init(&vars->putdown, NULL))
		return (0);
	if (pthread_mutex_init(&vars->alive, NULL))
		return (0);
	if (pthread_mutex_init(&vars->print, NULL))
		return (0);
	if (pthread_mutex_init(&vars->someone_died, NULL))
		return (0);
	return (1);
}

int		init(int argc, char **argv)
{
	t_vars  *vars;

	vars = get_vars();
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

unsigned long    get_time(void)
{
	struct timeval  tv;

	if (gettimeofday(&tv, NULL))
		return (0);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000); // in ms.
}

int ft_error(char *str)
{
	write(2, str, ft_strlen(str));
	return (0);
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

void    print_status_body(t_vars *vars, t_philo *philo, t_status status, char *phrase)
{
	unsigned long	time;
	int				philo_no;

	philo_no = philo->p_idx + 1;
	pthread_mutex_lock((&vars->print));
	time = get_time() - philo->start_time;
	pthread_mutex_lock(&vars->someone_died);
	if (vars->flag_died)
	{
		pthread_mutex_unlock(&vars->someone_died);
		return ;
	}
	pthread_mutex_unlock(&vars->someone_died);
	ft_putnbr(time);
	ft_putstr(" ");
	ft_putnbr(philo_no);
	ft_putstr(phrase);
	free(phrase);
	if (!(status == DIED))
		pthread_mutex_unlock(&vars->print);
}

int    print_status(t_vars *vars, t_philo *philo, t_status status)
{
	char	*phrase;

	phrase = 0;
	if (status == THINKING)
		phrase = ft_strdup(" is thinking\n");
	else if (status == EATING)
		phrase = ft_strdup(" is eating\n");
	else if (status == SLEEPING)
		phrase = ft_strdup(" is sleeping\n");
	else if (status == FORK_TAKEN)
		phrase = ft_strdup(" has taken a fork\n");
	else if (status == DIED)
		phrase = ft_strdup(" died\n");
	if (phrase == 0)
		return (0);
	print_status_body(vars, philo, status, phrase);
	return (1);
}

int     taken_fork_and_eat(t_vars *vars, t_philo *philo)
{
	int idx;

	idx = philo->p_idx;
	pthread_mutex_lock(&vars->pickup);
	pthread_mutex_lock(&vars->forks[idx]);
	print_status(vars, philo, FORK_TAKEN);
	pthread_mutex_lock(&vars->forks[(idx + 1) % vars->n_philo]);
	print_status(vars, philo, FORK_TAKEN);
	pthread_mutex_unlock(&vars->pickup);
	pthread_mutex_lock(&vars->eats[idx]);
	philo->last_eat_time = get_time();
	pthread_mutex_unlock(&vars->eats[idx]);
	print_status(vars, philo, EATING);
	ft_usleep(vars->t_eat);
	pthread_mutex_lock(&vars->putdown);
	pthread_mutex_unlock(&vars->forks[idx]);
	pthread_mutex_unlock(&vars->forks[(idx + 1) % vars->n_philo]);
	pthread_mutex_unlock(&vars->putdown);
	if ((++(philo->n_eat) == vars->n_must_eat))
		return (0);
	return (1);
}

void    *philosophing(void *v_philo)
{
	t_vars  *vars;
	t_philo *philo;

	vars = get_vars();
	philo = (t_philo *)v_philo;
	while (1)
	{
		print_status(vars, philo, THINKING);
		if (!taken_fork_and_eat(vars, philo))
			break ;
		print_status(vars, philo, SLEEPING);
		ft_usleep(vars->t_sleep);
	}
	pthread_mutex_lock(&vars->alive);
	vars->n_alive -= 1;
	pthread_mutex_unlock(&vars->alive);
	return (philo);
}

void    *monitoring(void *v_philo)
{
	t_vars  *vars;
	t_philo *philo;

	philo = (t_philo *)v_philo;
	vars = get_vars();
	while (1)
	{
		pthread_mutex_lock(&vars->eats[philo->p_idx]);
		if (get_time() - philo->last_eat_time > (unsigned long)vars->t_die)
		{
			print_status(vars, philo, DIED);
			pthread_mutex_lock(&vars->someone_died);
			vars->flag_died = 1;
			pthread_mutex_unlock(&vars->someone_died);
		}
		pthread_mutex_unlock(&vars->eats[philo->p_idx]);
		ft_usleep(5);
	}
}

int create_philo_even(t_vars * vars, unsigned long start_time)
{
	int i;
	int j;

	i = 0;
	while ((j = 2 * i) < vars->n_philo)
	{
		vars->philo[j].p_idx = j;
		vars->philo[j].n_eat = 0;
		vars->philo[j].start_time = start_time;
		vars->philo[j].last_eat_time = vars->philo[j].start_time;
		if (pthread_create(&vars->philo[j].thread, NULL, &philosophing, &vars->philo[j]))
			return (ft_error("Error: can not create pthread"));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: can not deatch pthread"));
		if (pthread_create(&vars->philo[j].m_thread, NULL, &monitoring, &vars->philo[j]))
			return (ft_error("Error: can not create pthread"));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: can not deatch pthread"));
		++i;
	}
	return (1);
}

int create_philo_odd(t_vars * vars, unsigned long start_time)
{
	int i;
	int j;

	i = 0;
	while ((j = 2 * i + 1) < vars->n_philo)
	{
		vars->philo[j].p_idx = j;
		vars->philo[j].n_eat = 0;
		vars->philo[j].start_time = start_time;
		vars->philo[j].last_eat_time = vars->philo[j].start_time;
		if (pthread_create(&vars->philo[j].thread, NULL, &philosophing, &vars->philo[j]))
			return (ft_error("Error: can not create pthread"));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: can not deatch pthread"));
		if (pthread_create(&vars->philo[j].m_thread, NULL, &monitoring, &vars->philo[j]))
			return (ft_error("Error: can not create pthread"));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: can not deatch pthread"));
		++i;
	}
	return (1);
}

int     create_philo(t_vars *vars)
{
	unsigned long    start_time;

	if (!(vars->philo = (t_philo *)malloc(sizeof(t_philo) * vars->n_philo)))
		return (0);
	start_time = get_time();
	if (!create_philo_even(vars, start_time))
		return (0);
	if (!create_philo_odd(vars, start_time))
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

int     free_all(int ret)
{
	t_vars	*vars;
	int		i;

	vars = get_vars();
	i = 0;
	while (i < vars->n_philo)
		pthread_mutex_destroy(&vars->forks[i++]); //mutex_destroy는 ptherad_mutex_t 객체를 없앤다!!
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

int     main(int argc, char **argv)
{
	t_vars	*vars;

	if (argc != 5 && argc != 6)
		return (-1);
	vars = get_vars();
	if (!init(argc, argv) || !create_philo(vars))
		return (-1);
	//printf("====init done===\n");
	//create philo 안에서 detach를 했기 때문에 thread functions가
	//while문이라 하더라도 문제 없이 아래 while문으로 들어갈 수 있다!!!
	while (1)
	{
		pthread_mutex_lock(&vars->alive);
		if (vars->n_alive == 0)
			break ;
		pthread_mutex_unlock(&vars->alive);
		pthread_mutex_lock(&vars->someone_died);
		if (vars->flag_died == 1)
			return (free_all(0));
		pthread_mutex_unlock(&vars->someone_died);
		ft_usleep(5);
	}
	if (vars->flag_died == 0)
		ft_putstr("Every philosopher ate enough!\n");
	return (free_all(0));
}
