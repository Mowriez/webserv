/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ARequest.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: treeps <treeps@student.42wolfsburg.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 16:08:40 by treeps            #+#    #+#             */
/*   Updated: 2023/08/21 16:08:40 by treeps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request/ARequest.hpp"
#include "Request/CgiRequest.hpp"
#include "Request/DELETERequest.hpp"
#include "Request/GETRequest.hpp"
#include "Request/POSTRequest.hpp"
#include "Request/RedirectRequest.hpp"

// constructor
ARequest::ARequest() : _location() {}

ARequest::ARequest(ClientSocket *clientSocket): _clientSocket(clientSocket), _rawRequest(clientSocket->getRawRequest()), _location() {
	_header = RequestHeader(_rawRequest.substr(0, _rawRequest.find(CRLF CRLF)));
	_location = _findLocation(_header["Path"]);
	if (_header["Path"].find("..") != std::string::npos)
		throw ARequest::ARequestException(FORBIDDEN);
	_expandPath();
	if (_header["Transfer-Encoding"] == "chunked")
		_unchunkBody();
}

ARequest::ARequest(const ARequest &other): _clientSocket(other._clientSocket), _location() {
	*this = other;
}

// destructor
ARequest::~ARequest() {}

// operator overload
ARequest &ARequest::operator=(const ARequest &other) {
	if (this == &other)
		return (*this);
	_clientSocket = other._clientSocket;
	_rawRequest = other._rawRequest;
	_header = other._header;
	_location = other._location;
	return (*this);
}

// member functions
ARequest *ARequest::newRequest(ClientSocket *clientSocket) {
	std::string request = clientSocket->getRawRequest();
	RequestHeader header(request.substr(0, request.find(CRLF CRLF)));
	Location *location = _findLocation(clientSocket, header["Path"]);
	if (header["Method"].empty() || header["Path"].empty() || header["HTTP-Version"].empty())
		throw ARequest::ARequestException(BAD_REQUEST);
	else if (clientSocket->getAllowedHTTPVersion() != header["HTTP-Version"])
		throw ARequest::ARequestException(HTTP_VERSION_NOT_SUPPORTED);
	else if (location->getAllowedMethods().find(header["Method"]) == std::string::npos)
		throw ARequest::ARequestException(METHOD_NOT_ALLOWED);
	else if (!location->getRedirect().empty())
		return (new RedirectRequest(clientSocket));
	else if (_isCgiPath(clientSocket, header["Path"]))
		return (new CgiRequest(clientSocket));
	else if (header["Method"] == "GET")
		return (new GETRequest(clientSocket));
	else if (header["Method"] == "POST")
		return (new POSTRequest(clientSocket));
	else if (header["Method"] == "DELETE")
		return (new DELETERequest(clientSocket));
	else
		throw ARequest::ARequestException(NOT_IMPLEMENTED);
}

// private member functions
bool ARequest::_isCgiPath(const ClientSocket *clientSocket, const std::string &path) {
	Location *location = _findLocation(clientSocket, path);

	size_t pos = path.find_last_of('.');
	if (pos == std::string::npos)
		return (false);
	std::string extension = path.substr(pos);
	if (location->getCgiExtension().find(extension) != std::string::npos)
		return (true);
	return (false);
}

bool ARequest::_isDirectory(const std::string &path) {
	struct stat s = {};
	if (stat(path.c_str(), &s) == 0)
		if (s.st_mode & S_IFDIR)
			return (true);
	return (false);
}

Location *ARequest::_findLocation(const ClientSocket *clientSocket, std::string path) {
	if (path.find('/') == 0)
		path = path.substr(1);
	if (!path.empty())
		path = path.substr(0, path.find('/'));
	path = "/" + path;
	if (clientSocket->getLocationMap()->find(path) != clientSocket->getLocationMap()->end())
		return (&clientSocket->getLocationMap()->find(path)->second);
	return (&clientSocket->getLocationMap()->find("/")->second);
}

Location *ARequest::_findLocation(std::string path) {
	if (path.find('/') == 0)
		path = path.substr(1);
	if (!path.empty())
		path = path.substr(0, path.find('/'));
	path = "/" + path;
	if (_clientSocket->getLocationMap()->find(path) != _clientSocket->getLocationMap()->end())
		return (&_clientSocket->getLocationMap()->find(path)->second);
	return (&_clientSocket->getLocationMap()->find("/")->second);
}

void ARequest::_expandPath() {
	std::string path = _header["Path"];
	if (path.find(_location->getPath()) == 0)
		path.replace(path.find(_location->getPath()), _location->getPath().length(), _location->getRoot());
	if (path[path.length() - 1] == '/' && path.length() > 1)
		path = path.substr(0, path.length() - 1);
	if (_isDirectory(path) && !_location->getIndex().empty())
		path += "/" + _location->getIndex();
	_header["Path"] = path;
}

void ARequest::_unchunkBody() {
	std::string body = _rawRequest.substr(_rawRequest.find(CRLF CRLF) + 4);
	std::string unchunkedBody;
	size_t chunkSize;
	size_t newlinePos;
	while (body.length() > 0) {
		chunkSize = strtol(body.c_str(), NULL, 16);
		if (chunkSize == 0)
			break;
		newlinePos = body.find(CRLF);
		body = body.substr(newlinePos + 2);
		unchunkedBody += body.substr(0, chunkSize);
		body = body.substr(chunkSize + 2);
	}
	_rawRequest = _rawRequest.substr(0, _rawRequest.find(CRLF CRLF) + 4) + unchunkedBody;
}

// exceptions

ARequest::ARequestException::ARequestException(int code): _code(code) {}

const char *ARequest::ARequestException::what() const throw() {
	return ("ARequestException");
}

std::string ARequest::ARequestException::message() const {
	return (toString(_code) + ": " + getHTTPErrorMessages(_code));
}

int ARequest::ARequestException::code() const {
	return (_code);
}
