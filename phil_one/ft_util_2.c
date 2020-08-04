#include "phil_one.h"

char	*ft_strdup(const char *s1)
{
	int		i;
	int		length;
	char	*result;

	length = 0;
	while (s1[length])
		length++;
	if (!(result = (char *)malloc(sizeof(char) * (length + 1))))
		return (0);
	i = 0;
	while (i < length)
	{
		result[i] = s1[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}

int		ft_atoi(const char *str)
{
	int sign;
	int result;
	int i;

	i = 0;
	while ((9 <= str[i] && str[i] <= 13) || str[i] == 32)
		i++;
	sign = 1;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign *= -1;
		i++;
	}
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}
