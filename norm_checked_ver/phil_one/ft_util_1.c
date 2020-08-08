/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_util_1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhur <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/06 20:33:02 by jhur              #+#    #+#             */
/*   Updated: 2020/08/08 13:56:44 by jhur             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "phil_one.h"

int		ft_strlen(char *s)
{
	int i;

	i = 0;
	while (s[i] != '\0')
		++i;
	return (i);
}

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

int		ft_error(char *str)
{
	write(2, str, ft_strlen(str));
	return (0);
}

void	ft_putnbr(long n)
{
	long	nb;

	nb = n;
	if (nb < 0)
	{
		ft_putchar('-');
		nb *= -1;
	}
	if (nb >= 10)
	{
		ft_putnbr(nb / 10);
		ft_putnbr(nb % 10);
	}
	else
		ft_putchar(nb + '0');
}

void	ft_putstr(char *str)
{
	write(1, str, ft_strlen(str));
}
