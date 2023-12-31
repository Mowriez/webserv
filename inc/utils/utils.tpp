/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.tpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: treeps <treeps@student.42wolfsburg.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 18:21:59 by treeps            #+#    #+#             */
/*   Updated: 2023/08/21 18:21:59 by treeps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_UTILS_TPP
# define WEBSERV_UTILS_TPP

#include <sstream>

template<typename T> std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif
