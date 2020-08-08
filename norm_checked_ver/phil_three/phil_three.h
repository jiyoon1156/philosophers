/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phil_three.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 15:07:34 by jhur              #+#    #+#             */
/*   Updated: 2020/08/08 15:08:24 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHIL_THREE_H
# define PHIL_THREE_H

# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>
# include <pthread.h>
# include <semaphore.h>
# include <stdio.h>
# include <signal.h>

typedef struct		s_philo
{
	int				p_idx;
	pthread_t		m_thread;
	int				n_eat;
	unsigned long	start_time;
	unsigned long	last_eat_time;
	pid_t			pid;
}					t_philo;

typedef struct		s_vars
{
	int				n_philo;
	int				t_die;
	int				t_eat;
	int				t_sleep;
	int				n_must_eat;
	int				n_alive;
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
}					t_status;

int					ft_strlen(char *s);
void				ft_putchar(char c);
void				ft_putstr_fd(char *str, int fd);
void				ft_putnbr(long n);
char				*ft_strdup(const char *s);
int					ft_atoi(const char *str);
t_vars				*get_vars(void);
unsigned long		get_time(void);
int					ft_error(char *str, int ret);
void				ft_usleep(unsigned long time);
int					ft_unlink(int ret);
void				print_status_body(t_vars *vars, t_philo *philo,
char *phrase, int exit_status);
int					print_status(t_vars *vars, t_philo *philo,
t_status status, int exit_status);
int					taken_fork_and_eat(t_vars *vars, t_philo *philo);
void				*monitoring(void *v_philo);
void				*philosophing(void *v_philo);
int					create_philo(t_vars *vars);
void				clean_shm(void);
int					init_semaphore(t_vars *vars);
int					init(int argc, char **argv);
void				wait_and_kill(t_vars *vars);
int					free_all(char *str, int ret);

#endif
