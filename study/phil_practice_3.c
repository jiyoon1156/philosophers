#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <signal.h>

//replace mutex with semaphore! mutex lock to mutex unlock are sem wait to sem post;

typedef struct  	s_philo
{
	int             p_idx;
	pthread_t       m_thread;
	int             n_eat;
	unsigned long   start_time;
	unsigned long   last_eat_time;
	pid_t			pid;
}               	t_philo;

typedef struct  	s_vars
{
	int     		n_philo;
	int     		t_die;
	int     		t_eat;
	int     		t_sleep;
	int     		n_must_eat;
	int     		n_alive;
	// int     		flag_died;
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

int		ft_strlen(char *s)
{
	int	count;

	count = 0;
	while (s[count] != '\0')
		++count;
	return (count);
}

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

void	ft_putstr_fd(char *str, int fd)
{
	write(fd, str, ft_strlen(str));
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

int		ft_atoi(const char *str)
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

void    *philosophing(void *v_philo)
{
	t_vars  *vars;
	t_philo *philo;

	vars = get_vars();
	philo = (t_philo *)v_philo;
	while (1)
	{
		print_status(vars, philo, THINKING);
		if (!taken_fork_and_eat(vars, philo))//(taken_fork_and_eat(vars,philo) == 0) 즉, 최소먹는 횟수를 다 채웠다면
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
			print_status(vars, philo, DIED, 1);
			exit(1);
		}
		if ((sem_post(vars->eats) == -1))
			ft_error("error: sem_wait\n", 1);
		ft_usleep(2000);
	}
	return (philo);
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
	// sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value); value는 초기화하는 값. 실패시 SEM_FAILED 리턴.
	if ((vars->forks = sem_open("/forks", O_CREAT, 0660, vars->n_philo)) == SEM_FAILED)
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
	if ((vars->print_error = sem_open("/print_error", O_CREAT, 0660, 1)) == SEM_FAILED)
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
	vars->n_must_eat = -1;
	if (argc == 6)
	{
		if ((vars->n_must_eat = ft_atoi(argv[5])) <= 0)
			return (0);
	}
	if (vars->n_philo < 2)
		return (0);
	if (vars->t_die < 0 || vars->t_eat < 0 || vars->t_sleep < 0)
		return (0);
	if (!(init_semaphore(vars)))
		return (0);
	return (1);
}

void	wait_and_kill(t_vars *vars)
{
	int i;
	int status;

	status = 0;
	//waitpid(pid_t pid, int *stat_loc, int options);
	while (waitpid(-1, &status, 0) > 0)
	{

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
