/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_two.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:23:59 by jhur              #+#    #+#             */
/*   Updated: 2020/08/10 21:04:35 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

t_vars	*get_vars(void)
{
	static t_vars vars;

	return (&vars);
}

int		main(int argc, char **argv)
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
		if (vars->flag_died == 1)
			return (ft_unlink(1));
		ft_usleep(10);
	}
	if (vars->flag_died == 0)
		ft_putstr_fd("Every philosopher ate enough!\n", 1);
	return (free_all(0, 0));
}
