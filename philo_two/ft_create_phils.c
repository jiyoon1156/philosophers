/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_create_phils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:00:11 by jhur              #+#    #+#             */
/*   Updated: 2020/08/10 21:03:35 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

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
	if ((sem_wait(vars->alive) == -1))
		ft_error("error: sem_wait\n", 1);
	vars->n_alive--;
	if ((sem_post(vars->alive) == -1))
		ft_error("error: sem_post\n", 1);
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
		ft_usleep(2000);
	}
	return (philo);
}

int		create_philo(t_vars *vars)
{
	unsigned long	start_time;
	int				i;

	if (!(vars->philo = (t_philo *)malloc(sizeof(t_philo) * vars->n_philo)))
		return (0);
	start_time = get_time();
	i = -1;
	while (++i < vars->n_philo)
	{
		vars->philo[i].p_idx = i;
		vars->philo[i].n_eat = 0;
		vars->philo[i].start_time = start_time;
		vars->philo[i].last_eat_time = vars->philo[i].start_time;
		if (pthread_create(&vars->philo[i].thread, 0,
		&philosophing, &vars->philo[i]))
			return (ft_error("Error: can not create pthread", 0));
		if (pthread_detach((vars->philo[i].thread)))
			return (ft_error("Error: can not deatch pthread", 0));
		if (pthread_create(&vars->philo[i].m_thread, 0,
		&monitoring, &vars->philo[i]))
			return (ft_error("Error: can not create pthread", 0));
		if (pthread_detach((vars->philo[i].m_thread)))
			return (ft_error("Error: can not deatch pthread", 0));
	}
	return (1);
}
