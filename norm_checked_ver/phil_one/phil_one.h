/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phil_one.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:16:55 by jhur              #+#    #+#             */
/*   Updated: 2020/08/08 14:22:39 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHIL_ONE_H
# define PHIL_ONE_H

# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <stdio.h>

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
	pthread_mutex_t	*forks;
	pthread_mutex_t	*eats;
	pthread_mutex_t	pickup;
	pthread_mutex_t	putdown;
	pthread_mutex_t	alive;
	pthread_mutex_t	print;
	pthread_mutex_t	someone_died;
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
int					ft_error(char *str);
void				ft_putnbr(long n);
void				ft_putstr(char *str);
char				*ft_strdup(const char *s1);
int					ft_atoi(const char *str);
unsigned long		get_time(void);
t_vars				*get_vars(void);
void				print_status_body(t_vars *vars, t_philo *philo,
t_status status, char *phrase);
int					print_status(t_vars *vars, t_philo *philo, t_status status);
void				ft_usleep(unsigned long time);
int					taken_fork_and_eat(t_vars *vars, t_philo *philo);
void				*philosophing(void *v_philo);
void				*monitoring(void *v_philo);
int					free_struct(void *s);
int					free_all(int ret);
int					init_mutexes(t_vars *vars);
int					init(int argc, char **argv);
int					create_philo_even(t_vars *vars, unsigned long start_time);
int					create_philo_odd(t_vars *vars, unsigned long start_time);
int					create_philo(t_vars *vars);

#endif
