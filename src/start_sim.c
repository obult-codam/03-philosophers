/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   start_sim.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: obult <obult@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/02 16:41:10 by obult         #+#    #+#                 */
/*   Updated: 2022/02/09 15:09:35 by obult         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	someonedied_function(t_general *data, int philo)
{
	int	i;

	i = 0;
	while (i < (data->philocount + 9) / 10)
	{
		pthread_mutex_lock(&data->dead[i].mut);
		data->dead[i].check = 1;
		i++;
	}
	printf("time: %i, philo: %i died\n", (int)elapsed_time(&data->ph_info[philo - 1]), philo);
	i = 0;
	while (i < (data->philocount + 9) / 10)
	{
		pthread_mutex_unlock(&data->dead[i].mut);
		i++;
	}
}

int		whale_loop(t_general *data)
{
	int	i;

	i = 0;
	
	while (i < data->philocount)
	{
		// pthread_mutex_lock();
		if (data->eats != data->ph_info[i].times_eaten || data->eats == 0) // controleren
			return (0);
		// pthread_mutex_unlock();
		i++;
	}
	return (1);
}

void	ph_holy_thread(t_general *data)
{
	int	i;
	int c;

	c = 0;
	while (1)
	{
		i = 0;
		while (i < data->philocount)
		{
			// printf("last eaten %lld\n", data->ph_info[i].last_eaten);
			if (time_in_millis() > data->ph_info[i].last_eaten + data->time_to_die && data->ph_info[i].last_eaten != -1)
			{
				// printf("dead philo alert! (%i)(%lld)\n", i + 1, elapsed_time(data->ph_info));
				someonedied_function(data, i + 1);
				c = 1;
				break ;
			}
			i++;
		}
		if (c || whale_loop(data))
			break ;
		usleep(200);
	}
}

void	*start_sim(t_general *data)
{
	int	i;

	i = 0;
	
	data->start_time = time_in_millis();
	int k = 0;
	while (k < (data->philocount + 9) / 10)
	{
		data->dead[k].check = 0;
		pthread_mutex_init(&data->dead[k].mut, NULL);
		k++;
	}
	int j = data->philocount;
	while (j)
	{
		j--;
		pthread_mutex_init(&data->forks[j], NULL);
	}
	while (i < data->philocount)
	{
		data->ph_info[i].last_eaten = data->start_time;
		pthread_create(&data->philos[i], NULL, iam_philo, (void *)&data->ph_info[i]);
		i++;
	}
	ph_holy_thread(data);
	while (i)
	{
		i--;
		pthread_join(data->philos[i], NULL);
	}
	while (i < data->philocount)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	// pthread_mutex_destroy(&data->dead.mut); needs to be whole array
	return (NULL);
}