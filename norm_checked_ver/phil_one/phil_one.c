/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phil_one.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:16:41 by jhur              #+#    #+#             */
/*   Updated: 2020/08/08 14:16:48 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "phil_one.h"

t_vars	*get_vars(void)
{
	static t_vars vars;

	return (&vars);
}

int		main(int argc, char **argv)
{
	t_vars *vars;

	if (argc != 5 && argc != 6)
		return (-1);
	vars = get_vars();
	if (!init(argc, argv) || !create_philo(vars))
		return (-1);
	while (1)
	{
		pthread_mutex_lock(&vars->alive);
		if (vars->n_alive == 0)
			break ;
		pthread_mutex_unlock(&vars->alive);
		pthread_mutex_lock(&vars->someone_died);
		if (vars->flag_died == 1)
			return (free_all(0));
		pthread_mutex_unlock(&vars->someone_died);
		ft_usleep(5);
	}
	if (vars->flag_died == 0)
		ft_putstr("Every philosopher ate enough!\n");
	return (free_all(0));
}
