/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_create_phils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 20:32:17 by jhur              #+#    #+#             */
/*   Updated: 2020/08/08 13:51:09 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "phil_one.h"

void	*philosophing(void *v_philo)
{
	t_vars	*vars;
	t_philo	*philo;

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

void	*monitoring(void *v_philo)
{
	t_vars	*vars;
	t_philo	*philo;

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

int		create_philo_even(t_vars *vars, unsigned long start_time)
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
		if (pthread_create(&vars->philo[j].thread, 0,
		&philosophing, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_create(&vars->philo[j].m_thread, 0,
		&monitoring, &vars->philo[j]))
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
	while ((j = 2 * i + 1) < vars->n_philo)
	{
		vars->philo[j].p_idx = j;
		vars->philo[j].n_eat = 0;
		vars->philo[j].start_time = start_time;
		vars->philo[j].last_eat_time = vars->philo[j].start_time;
		if (pthread_create(&vars->philo[j].thread, 0,
		&philosophing, &vars->philo[j]))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_detach((vars->philo[j].thread)))
			return (ft_error("Error: cannot create pthread"));
		if (pthread_create(&vars->philo[j].m_thread, 0,
		&monitoring, &vars->philo[j]))
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
