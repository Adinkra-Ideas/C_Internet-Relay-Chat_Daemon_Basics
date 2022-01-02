/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rush00.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: euyi <euyi@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/03 15:56:06 by euyi              #+#    #+#             */
/*   Updated: 2022/01/02 05:06:06 by euyi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void	x_top_bottom_loop_sideways(int x); /* required for X top */
void	x_middle_loop_sideways(int x); /* required for X middle */
void	rush(int x, int y); /* multiply all X value along Y axis */
void	ft_putchar(char c); /* print single characters */

int	main(void)
{
	rush(1, 7);
	return (0);
}

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

void	rush(int x, int y)
{
	int	xw;
	int	yl;

	xw = 0;
	yl = 0;
	if (y == 1)
		x_top_bottom_loop_sideways(x);
	else if (y == 2)
	{
		x_top_bottom_loop_sideways(x);
		x_top_bottom_loop_sideways(x);
	}
	else if (yl <= (y >= 3))
	{
		x_top_bottom_loop_sideways(x);
		x_middle_loop_sideways(x);
		while (yl <= (y - 4))
		{
			x_middle_loop_sideways(x);
			yl++;
		}
		x_top_bottom_loop_sideways(x);
	}
}

void	x_top_bottom_loop_sideways(int x)
{
	int	xw;

	xw = 0;
	if (x == 1)
	{
		ft_putchar('o');
	}
	else if (x == 2)
	{
		ft_putchar('o');
		ft_putchar('o');
	}
	else if (xw <= (x >= 3))
	{
		ft_putchar('o');
		while (xw <= (x - 3))
		{
			ft_putchar('-');
			xw++;
		}
		ft_putchar('o');
		xw = 0;
	}
	ft_putchar('\n');
}

void	x_middle_loop_sideways(int x)
{
	int	xw;

	xw = 0;
	if (x == 1)
	{
		ft_putchar('|');
	}
	else if (x == 2)
	{
		ft_putchar('|');
		ft_putchar('|');
	}
	else if (xw <= (x >= 3))
	{
		ft_putchar('|');
		while (xw <= (x - 3))
		{
			ft_putchar(' ');
			xw++;
		}
		ft_putchar('|');
		xw = 0;
	}
	ft_putchar('\n');
}
