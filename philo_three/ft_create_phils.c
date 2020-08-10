/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_create_phils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:28:30 by jhur              #+#    #+#             */
/*   Updated: 2020/08/10 21:05:05 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

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
			print_status(vars, philo, DIED, 1);
			exit(1);
		}
		if ((sem_post(vars->eats) == -1))
			ft_error("error: sem_wait\n", 1);
		ft_usleep(5);
	}
	return (philo);
}

void	*philosophing(void *v_philo)
{
	t_vars	*vars;
	t_philo	*philo;

	vars = get_vars();
	philo = (t_philo *)v_philo;
	if (pthread_create(&philo->m_thread, 0, &monitoring, philo))
		exit(ft_error("Error: cannot create pthread", 1));
	if (pthread_detach((philo->m_thread)))
		exit(ft_error("Error: cannot create pthread", 1));
	while (1)
	{
		print_status(vars, philo, THINKING, 0);
		if (!taken_fork_and_eat(vars, philo))
			exit(0);
		print_status(vars, philo, SLEEPING, 0);
		ft_usleep(vars->t_sleep);
	}
	exit(0);
}

int		create_philo(t_vars *vars)
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
		if ((vars->philo[i].pid = fork()) == 0)
			philosophing(&vars->philo[i]);
		i++;
	}
	return (1);
}
