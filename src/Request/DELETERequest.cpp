/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DELETERequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: treeps <treeps@student.42wolfsburg.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 16:11:53 by treeps            #+#    #+#             */
/*   Updated: 2023/08/21 16:11:53 by treeps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request/DELETERequest.hpp"

// constructors
DELETERequest::DELETERequest() {}
DELETERequest::DELETERequest(const ClientSocket& clientSocket, const std::string &request) : ARequest(clientSocket, request) {
	_method = "DELETE";
}
DELETERequest::DELETERequest(const DELETERequest &other): ARequest(other) {}

// destructor
DELETERequest::~DELETERequest() {}

// operator overload
DELETERequest &DELETERequest::operator=(const DELETERequest &other) {
	if (this == &other)
		return (*this);
	return (*this);
}

Response DELETERequest::handle() {
	Response response(_clientSocket);
	std::cout << "DELETERequest::handle()" << std::endl;
	std::string path = _extractPath(7); // 7 = length of "DELETE "
	if (path == "/")
		path += _clientSocket.getIndexFile();
	path = _clientSocket.getIndexFolder() + path;
	if (std::remove(path.c_str()) != 0) {
		response.buildErrorPage(404);
		return (response);
	}
	response.setStatusCode(200);
	response.setContentType("text/html");
	response.setBody("<html><body><h1>File deleted</h1></body></html>");
	return (response);
}
