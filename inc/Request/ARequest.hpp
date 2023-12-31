/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ARequest.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: treeps <treeps@student.42wolfsburg.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 16:08:40 by treeps            #+#    #+#             */
/*   Updated: 2023/08/21 16:08:40 by treeps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_AREQUEST_HPP
# define WEBSERV_AREQUEST_HPP

# include <string>
# include <fstream>
# include <exception>
# include <cstdlib> // todo: replace strtol wit c++98 conform function
# include <sys/stat.h>
# include <sys/wait.h>
# include "Header/RequestHeader.hpp"
# include "Response/Response.hpp"
# include "utils/utils.hpp"

class ARequest {
public:
	// constructors
	ARequest(ClientSocket* clientSocket);
	ARequest(const ARequest &);

	// destructor
	virtual ~ARequest();

	// operator overload
	ARequest &operator=(const ARequest &);

	// member functions
	virtual Response *handle() = 0;
	static ARequest *newRequest(ClientSocket* clientSocket);

protected:
	// constructors
	ARequest();

	// member functions
	static bool _isDirectory(const std::string& path);

	// member variables
	ClientSocket *_clientSocket;
	std::string _rawRequest;
	RequestHeader _header;
	t_locationConfig _location;
	t_serverConfig _serverConfig;

private:
	// member functions
	static bool				_isCgiPath(const t_serverConfig& serverConfig, const std::string& path);
	static t_locationConfig	_findLocation(const t_serverConfig &serverConfig, std::string path);
	void					_expandPath();
	void					_unchunkBody();

public:
	// exceptions
	class ARequestException: public std::exception {
	public:
		ARequestException(int);
		virtual const char *what() const throw();
		std::string message() const;
		int code() const;
	private:
		int _code;
	};
};

#endif
