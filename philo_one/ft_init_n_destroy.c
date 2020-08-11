/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init_n_destroy.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 20:32:37 by jhur              #+#    #+#             */
/*   Updated: 2020/08/11 11:05:26 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

int	free_struct(void *s)
{
	if (s)
		free(s);
	s = 0;
	return (1);
}

int	free_all(int ret)
{
	t_vars	*vars;
	int		i;

	vars = get_vars();
	i = 0;
	while (i < vars->n_philo)
		pthread_mutex_destroy(&vars->forks[i++]);
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

int	init_mutexes(t_vars *vars)
{
	int i;

	if (!(vars->forks = malloc(sizeof(pthread_mutex_t) * vars->n_philo)))
		return (0);
	if (!(vars->eats = malloc(sizeof(pthread_mutex_t) * vars->n_philo)))
		return (0);
	i = 0;
	while (i < vars->n_philo)
		if (pthread_mutex_init(&vars->forks[i++], 0))
			return (0);
	i = 0;
	while (i < vars->n_philo)
		if (pthread_mutex_init(&vars->eats[i++], 0))
			return (0);
	if (pthread_mutex_init(&vars->pickup, 0))
		return (0);
	if (pthread_mutex_init(&vars->putdown, 0))
		return (0);
	if (pthread_mutex_init(&vars->alive, 0))
		return (0);
	if (pthread_mutex_init(&vars->print, 0))
		return (0);
	if (pthread_mutex_init(&vars->someone_died, 0))
		return (0);
	return (1);
}

int	init(int argc, char **argv)
{
	t_vars *vars;

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
	if (vars->n_philo < 1)
		return (0);
	if (vars->t_die < 0 || vars->t_eat < 0 || vars->t_sleep < 0)
		return (0);
	vars->n_alive = vars->n_philo;
	if (!(init_mutexes(vars)))
		return (0);
	return (1);
}
