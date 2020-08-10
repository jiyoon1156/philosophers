/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init_n_destroy.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:30:12 by jhur              #+#    #+#             */
/*   Updated: 2020/08/10 21:05:08 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

int		ft_unlink(int ret)
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

void	clean_shm(void)
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

int		init_semaphore(t_vars *vars)
{
	vars->n_alive = vars->n_philo;
	clean_shm();
	if ((vars->forks = sem_open("/forks", O_CREAT, 0660, vars->n_philo))
	== SEM_FAILED)
		return (0);
	if ((vars->eats = sem_open("/eats", O_CREAT, 0660, vars->n_philo))
	== SEM_FAILED)
		return (0);
	if ((vars->pickup = sem_open("/pickup", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->putdown = sem_open("/putdown", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->print = sem_open("/print", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->alive = sem_open("/alive", O_CREAT, 0660, 1)) == SEM_FAILED)
		return (0);
	if ((vars->someone_died = sem_open("/someone_died", O_CREAT, 0660, 1))
	== SEM_FAILED)
		return (0);
	if ((vars->print_error = sem_open("/print_error", O_CREAT, 0660, 1))
	== SEM_FAILED)
		return (0);
	return (1);
}

int		init(int argc, char **argv)
{
	t_vars	*vars;

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

int		free_all(char *str, int ret)
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
