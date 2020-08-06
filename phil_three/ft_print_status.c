#include "phil_three.h"

int ft_error(char *str, int ret)
{
	t_vars *vars;

	vars = get_vars();
	sem_wait(vars->print_error);
	write(2, str, ft_strlen(str)); //fd 2는 표준 오류
	sem_post(vars->print_error);
	return (ret);
}

void    print_status_body(t_vars *vars, t_philo *philo, char *phrase, int exit_status)
{
	unsigned long	time;
	int				philo_no;

	philo_no = philo->p_idx + 1;
	time = get_time() - philo->start_time;
	if ((sem_wait(vars->print) == -1))
		ft_error("error: sem_wait\n", 1);
	// time = get_time() - philo->start_time; // 여기가 문제였네; 시간 계산하는 타이밍 문제라고? 아님 상관없음

	// if ((sem_wait(vars->someone_died) == -1))
	// 	ft_error("error: sem_wait\n", 1);
	// if (vars->flag_died) //죽었다면
	// {
	// 	if ((sem_post(vars->someone_died) == -1))
	// 		ft_error("error: sem_post\n", 1);
	// 	return ;
	// }
	// if ((sem_post(vars->someone_died) == -1))
	// 	ft_error("error: sem_post\n", 1);

	ft_putnbr(time);
	ft_putstr_fd(" ", 1);
	ft_putnbr(philo_no);
	ft_putstr_fd(phrase, 1);
	if (exit_status == 0)
	{
		if ((sem_post(vars->print) == -1))
			ft_error("error: sem_post\n", 1);
	}
}
int    print_status(t_vars *vars, t_philo *philo, t_status status, int exit_status)
{
	char	*phrase;

	phrase = 0;
	if (status == THINKING)
		phrase = ft_strdup(" is thinking\n");
	else if (status == EATING)
		phrase = ft_strdup(" is eating\n");
	else if (status == SLEEPING)
		phrase = ft_strdup(" is sleeping\n");
	else if (status == FORK_TAKEN)
		phrase = ft_strdup(" has taken a fork\n");
	else if (status == DIED)
		phrase = ft_strdup(" died\n");
	if (phrase == 0)
		return (0);
	print_status_body(vars, philo, phrase, exit_status);
	free(phrase);
	return (1);
}

int     taken_fork_and_eat(t_vars *vars, t_philo *philo)
{
	if ((sem_wait(vars->pickup) == -1))//포크를 드는거 lock
		ft_error("error: sem_wait\n", 1);

	if ((sem_wait(vars->forks) == -1))//포크 1번째꺼 lock
		ft_error("error: sem_wait\n", 1);
	print_status(vars, philo, FORK_TAKEN, 0);//포크 1개 들었음!!!!!
	if ((sem_wait(vars->forks) == -1))//포크 2번째꺼 lock
		ft_error("error: sem_wait\n", 1);
	print_status(vars, philo, FORK_TAKEN, 0);//포크 2개 들었음!!!!

	if ((sem_post(vars->pickup) == -1))//포크 다들었으니까 드는거 unlock
		ft_error("error: sem_post\n", 1);

	if ((sem_wait(vars->eats) == -1)) //이제 먹어야되니까 공유자원인 시간(last_eat_time) 계산해야돼서 먹는 행위 lock
		ft_error("error: sem_wait\n", 1);
	philo->last_eat_time = get_time();//마지막으로 먹는 시간 저장해두기
	print_status(vars, philo, EATING, 0);//먹는다!!!!!
	if ((sem_post(vars->eats) == -1))//먹었으니까 unlock
		ft_error("error: sem_post\n", 1);
	ft_usleep(vars->t_eat);//먹는 시간이 흘러

	if ((sem_wait(vars->putdown) == -1))//먹었으니까 포크를 내려놔야지. 내려놓은 행위 lock
		ft_error("error: sem_wait\n", 1);
	if ((sem_post(vars->forks) == -1))//포크 1번째꺼 내려놓음
		ft_error("error: sem_wait\n", 1);
	if ((sem_post(vars->forks) == -1))//포크 2번째꺼 내려놓음
		ft_error("error: sem_wait\n", 1);
	if ((sem_post(vars->putdown) == -1))//다 내려놨으니까 내려놓는 행위 unlock
		ft_error("error: sem_post\n", 1);
	if ((++(philo->n_eat) == vars->n_must_eat))//철학자가 먹은 횟수가 최소 먹는 횟수랑 같아진다면 시뮬레이션 종료가 되어야 하니까
		return (0);
	return (1);
}
