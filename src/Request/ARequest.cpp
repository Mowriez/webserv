/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ARequest.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: treeps <treeps@student.42wolfsburg.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 16:08:40 by treeps            #+#    #+#             */
/*   Updated: 2023/08/21 16:08:40 by treeps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request/ARequest.hpp"
#include "Request/GETRequest.hpp"
#include "Request/POSTRequest.hpp"
#include "Request/DELETERequest.hpp"
#include "Request/InvalidRequest.hpp"

// constructor
ARequest::ARequest() : _clientSocket() {}
ARequest::ARequest(const ClientSocket& clientSocket, const std::string& request): _clientSocket(clientSocket), _request(request) {}
ARequest::ARequest(const ARequest &other): _clientSocket(other._clientSocket), _request(other._request) {}

// destructor
ARequest::~ARequest() {}

// operator overload
ARequest &ARequest::operator=(const ARequest &other) {
	if (this == &other)
		return (*this);
	return (*this);
}

std::string ARequest::_extractPath(int start) {
	std::string path = _request.substr(start, _request.find(HTTP_VERSION) - (start + 1));
	return path;
}

ARequest *ARequest::newRequest(const ClientSocket &clientSocket, const std::string &request) {
	std::string version = request.substr(request.find("HTTP/"), request.find("\r\n") - request.find("HTTP/"));
	if (clientSocket.getAllowedHTTPVersion() != version)
		return (new InvalidRequest(clientSocket, request, 505));
	else if (request.find("GET ") == 0)
		return (new GETRequest(clientSocket, request));
	else if (request.find("POST ")  == 0)
		return (new POSTRequest(clientSocket, request));
	else if (request.find("DELETE ") == 0)
		return (new DELETERequest(clientSocket, request));
	else
		return (new InvalidRequest(clientSocket, request, 405));
}
