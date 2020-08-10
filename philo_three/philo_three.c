/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phil_three.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:59:58 by jhur              #+#    #+#             */
/*   Updated: 2020/08/10 21:04:57 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

t_vars	*get_vars(void)
{
	static t_vars vars;

	return (&vars);
}

void	wait_and_kill(t_vars *vars)
{
	int i;
	int status;

	status = 0;
	while (waitpid(-1, &status, 0) > 0)
	{
		if (WIFEXITED(status) && (WEXITSTATUS(status) == 1))
		{
			i = 0;
			while (i < vars->n_philo)
				kill(vars->philo[i++].pid, SIGINT);
		}
		else if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
		{
			if (--vars->n_philo == 0)
			{
				ft_putstr_fd("Every philosopher ate enough\n", 1);
				return ;
			}
		}
	}
}

int		main(int argc, char **argv)
{
	t_vars *vars;

	if (argc != 5 && argc != 6)
		return (-1);
	vars = get_vars();
	if (!init(argc, argv) || !create_philo(vars))
		return (-1);
	wait_and_kill(vars);
	return (free_all(0, 0));
}
