#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

//replace mutex with semaphore! mutex lock to mutex unlock are sem wait to sem post;

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
	sem_t			*forks;
	sem_t			*eats;
	sem_t			*pickup;
	sem_t			*putdown;
	sem_t			*alive;
	sem_t			*print;
	sem_t			*print_error;
	sem_t			*someone_died;
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

void ft_putstr_fd(char *str, int fd)
{
	write(fd, str, ft_strlen(str));
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
	return (&vars);
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

void				clean_shm(void)
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

int     init_semaphore(t_vars *vars)
{
	vars->n_alive = vars->n_philo;
	clean_shm();
	if ((vars->forks = sem_open("/forks", O_CREAT, 0660, vars->n_philo))\
	== SEM_FAILED)
		return (0);
	if ((vars->eats = sem_open("/eats", O_CREAT, 0660, vars->n_philo)) == SEM_FAILED)
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
	if ((vars->print_error = sem_open("/print_error", O_CREAT, 0660, 1))\
	== SEM_FAILED)
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
	if (!(init_semaphore(vars)))
		return (0);
	return (1);
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

void    print_status_body(t_vars *vars, t_philo *philo, t_status status, char *phrase)
{
	unsigned long	time;
	int				philo_no;

	philo_no = philo->p_idx + 1;
	time = get_time() - philo->start_time;
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
	ft_putnbr(philo_no);
	ft_putstr_fd(phrase, 1);
	if (!(status == DIED))
	{
		if ((sem_post(vars->print) == -1))
			ft_error("error: sem_post\n", 1);
	}
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
	free(phrase);
	return (1);
}

int     taken_fork_and_eat(t_vars *vars, t_philo *philo)
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
	if ((sem_wait(vars->alive) == -1))
		ft_error("error: sem_wait\n", 1);
	vars->n_alive--;
	if ((sem_post(vars->alive) == -1))
		ft_error("error: sem_post\n", 1);
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
		if ((sem_wait(vars->eats) == -1))
			ft_error("error: sem_wait\n", 1);
		if (get_time() - philo->last_eat_time > (unsigned)vars->t_die)
		{
			print_status(vars, philo, DIED);
			if ((sem_wait(vars->someone_died) == -1))
				ft_error("error: sem_wait\n", 1);
			vars->flag_died = 1;
			if ((sem_post(vars->someone_died) == -1))
				ft_error("error: sem_post\n", 1);
		}
		if ((sem_post(vars->eats) == -1))
			ft_error("error: sem_wait\n", 1);
		ft_usleep(5);
	}
	return (philo);
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
			return (ft_error("Error: can not create pthread", 0));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: can not deatch pthread", 0));
		if (pthread_create(&vars->philo[j].m_thread, NULL, &monitoring, &vars->philo[j]))
			return (ft_error("Error: can not create pthread", 0));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: can not deatch pthread", 0));
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
			return (ft_error("Error: can not create pthread", 0));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: can not deatch pthread", 0));
		if (pthread_create(&vars->philo[j].m_thread, NULL, &monitoring, &vars->philo[j]))
			return (ft_error("Error: can not create pthread", 0));
		if (pthread_detach((vars->philo[j].m_thread)))
			return (ft_error("Error: can not deatch pthread", 0));
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
int     free_struct(void *s)
{
	if (s)
		free(s);
	s = 0;
	return (1);
}

int     free_all(char *str, int ret)
{
	t_vars *vars;

	if (str != 0)
		ft_putstr_fd(str, 2);
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

int     main(int argc, char **argv)
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
		if (vars->n_alive == 0)
			break ;
		if ((sem_post(vars->alive) == -1))
			return (free_all("error: sem_post\n", -1));
		if ((sem_wait(vars->someone_died) == -1))
			return (free_all("error: sem_wait\n", -1));
		if (vars->flag_died == 1)
			return (free_all(0, 0));
		if ((sem_post(vars->someone_died) == -1))
			return (free_all("error: sem_post\n", -1));
		ft_usleep(5);
	}
	if (vars->flag_died == 0)
		ft_putstr_fd("Every philosopher ate enough!\n", 1);
	return (free_all(0, 0));
}