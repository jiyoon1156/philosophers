/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_status.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 20:32:46 by jhur              #+#    #+#             */
/*   Updated: 2020/08/10 21:03:20 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

void			print_status_body(t_vars *vars, t_philo *philo,
t_status status, char *phrase)
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

int				taken_fork_and_eat(t_vars *vars, t_philo *philo)
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
