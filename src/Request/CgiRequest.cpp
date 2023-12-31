/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: treeps <treeps@student.42wolfsburg.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 11:27:17 by treeps            #+#    #+#             */
/*   Updated: 2023/08/30 11:27:17 by treeps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request/CgiRequest.hpp"

// constructors
CgiRequest::CgiRequest() {}
CgiRequest::CgiRequest(ClientSocket* clientSocket) : ARequest(clientSocket), _envp(), _env(), _scriptPath(), _queryString(), _pathInfo(), _cgiInput(), _cgiOutput() {
	int i = 0;
	while (environ[i])
		_env.push_back(environ[i++]);
}
CgiRequest::CgiRequest(const CgiRequest &other): ARequest(other), _envp(other._envp), _env(other._env), _scriptPath(other._scriptPath), _queryString(other._queryString), _pathInfo(other._pathInfo) {
	*this = other;
}

// destructor
CgiRequest::~CgiRequest() {}

// operator overload
CgiRequest &CgiRequest::operator=(const CgiRequest &other) {
	if (this == &other)
		return (*this);
	_env = other._env;
	_header = other._header;
	_clientSocket = other._clientSocket;
	_rawRequest = other._rawRequest;
	return (*this);
}

// member functions
Response *CgiRequest::handle() {
	Response *response = new Response(_clientSocket);
	std::cout << "CgiRequest::handle()" << std::endl;

	_getScriptPath();
	_getQueryString();
	_getPathInfo();
	_setEnv();
	_execCgi(response);
	return (response);
}

void CgiRequest::_getScriptPath() {
	std::string path = _header["Path"];
	size_t pos = path.find('.');
	size_t pathInfo = path.find('/', pos);
	size_t queryString = path.find('?', pos);
	if (queryString < pathInfo)
		_scriptPath = path.substr(0, queryString);
	else
		_scriptPath = path.substr(0, pathInfo);
	if (access(_scriptPath.c_str(), F_OK) == -1)
		throw ARequest::ARequestException(NOT_FOUND);
	if (access(_scriptPath.c_str(), X_OK) == -1)
		throw ARequest::ARequestException(FORBIDDEN);
}

void CgiRequest::_getQueryString() {
	std::string path = _header["Path"];
	size_t pos = path.find('?');
	if (pos != std::string::npos)
		_queryString = path.substr(pos + 1);
}

void CgiRequest::_getPathInfo() {
	std::string path = _header["Path"];
	size_t pos = path.find('.');
	size_t addPath = path.find('/', pos);
	size_t queryString = path.find('?', pos);
	if (queryString != std::string::npos && addPath != std::string::npos)
		_pathInfo = path.substr(addPath, queryString - addPath);
	else if (addPath != std::string::npos)
		_pathInfo = path.substr(addPath);
	else
		_pathInfo = "";
}

void CgiRequest::_setEnv() {
	_env.push_back("AUTH_TYPE=" + _header["Authorization"]);
	_env.push_back("CONTENT_LENGTH=" + _header["Content-Length"]);
	_env.push_back("CONTENT_TYPE=" + _header["Content-Type"]);
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_env.push_back("PATH_INFO=" + _pathInfo);
	_env.push_back("PATH_TRANSLATED=" + _scriptPath);
	_env.push_back("QUERY_STRING=" + _queryString);
	// _env.push_back("REMOTE_ADDR=" + _clientSocket.getIP());
	_env.push_back("REMOTE_IDENT=");
	_env.push_back("REMOTE_USER=");
	_env.push_back("REQUEST_METHOD=" + _header["Method"]);
	_env.push_back("REQUEST_URI=" + _header["Path"]);
	_env.push_back("SCRIPT_NAME=" + _scriptPath);
	_env.push_back("SERVER_NAME=" + _serverConfig.serverName);
	_env.push_back("SERVER_PORT=" + toString(_serverConfig.port));
	_env.push_back("SERVER_PROTOCOL=" + _header["HTTP-Version"]);
	_env.push_back("SERVER_SOFTWARE=webserv/1.0");
}

void CgiRequest::_exportEnv() {
	int i = 0;
	_envp = new char*[_env.size() + 1];
	while (i < (int)_env.size()) {
		_envp[i] = new char[_env[i].length() + 1];
		_envp[i] = (char *)_env[i].c_str();
		i++;
	}
	_envp[i] = NULL;
}

void CgiRequest::_writeCgiInput() {
	write(_cgiInput[1], _rawRequest.substr(_rawRequest.find("\r\n\r\n"), _rawRequest.length() - _rawRequest.find("\r\n\r\n")).c_str(), strtol(_header["Content-Length"].c_str(), NULL, 10));
}

void CgiRequest::_readCgiOutput() {
	static char buffer[BUFFER_SIZE];
	ssize_t bytesRead;

	bzero(buffer, BUFFER_SIZE);
	while (true) {
		bytesRead = read(_cgiOutput[0], buffer, BUFFER_SIZE);
		if (bytesRead <= 0)
			break;
		_cgiOutputString.append(buffer, bytesRead);
		if (bytesRead < BUFFER_SIZE)
			break;
	}
}

void CgiRequest::_execCgi(Response *response) {
	pid_t pid;
	int status;

	if (pipe(_cgiInput) == -1 || pipe(_cgiOutput) == -1)
		throw ARequest::ARequestException(INTERNAL_SERVER_ERROR);
	pid = fork();
	if (pid == -1)
		throw ARequest::ARequestException(INTERNAL_SERVER_ERROR);
	else if (pid == 0) {
		dup2(_cgiInput[0], 0);
		dup2(_cgiOutput[1], 1);
		close(_cgiInput[1]);
		close(_cgiOutput[0]);
		close(_cgiInput[0]);
		close(_cgiOutput[1]);
		_exportEnv();
		std::string scriptPath = _scriptPath;
		std::string scriptName = _scriptPath.substr(_scriptPath.find_last_of('/') + 1);
		while (scriptPath.back() != '/')
			scriptPath.pop_back();
		scriptPath = "./" + scriptPath;
		if (chdir(scriptPath.c_str()) == 1)
			exit(1);
		if (execve(scriptName.c_str(), NULL, _envp) == -1)
			exit(1);
	} else {
		if (_header["Method"] == "POST")
			_writeCgiInput();
		close(_cgiInput[1]);
		close(_cgiInput[0]);
		close(_cgiOutput[1]);
		time_t start;
		std::time(&start);
		while (std::difftime(std::time(NULL), start) < g_timeout) {
			waitpid(pid, &status, WNOHANG);
			if (WIFEXITED(status))
				break;
		}
		if (!WIFEXITED(status))
		{
			kill(pid, SIGKILL);
			close(_cgiOutput[0]);
			throw ARequest::ARequestException(GATEWAY_TIMEOUT);
		}
		if (WEXITSTATUS(status) == 1) {
			close(_cgiOutput[0]);
			throw ARequest::ARequestException(INTERNAL_SERVER_ERROR);
		}
		_readCgiOutput();
		close(_cgiOutput[0]);
		size_t pos = _cgiOutputString.find(CRLF CRLF);
		if (pos != std::string::npos) {
			std::string header = _cgiOutputString.substr(0, pos);
			response->setHeader(ResponseHeader(header));
			response->setBody(_cgiOutputString.substr(pos + 4));
		} else {
			response->setBody(_cgiOutputString);
		}
	}
}

// http://example.com/cgi-bin/printenv.pl/with/additional/path?and=a&query=string
// http://example.com/cgi-bin/printenv.pl?and=a&query=string
