/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 14:14:34 by jhur              #+#    #+#             */
/*   Updated: 2020/08/10 21:04:27 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

unsigned long	get_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL))
		return (0);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void			ft_usleep(unsigned long time)
{
	unsigned long start;
	unsigned long time_elapsed;

	start = get_time();
	while (1)
	{
		time_elapsed = get_time() - start;
		if (time_elapsed >= time)
			break ;
		usleep(1);
	}
}
