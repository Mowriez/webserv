/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: treeps <treeps@student.42wolfsburg.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 19:20:33 by treeps            #+#    #+#             */
/*   Updated: 2023/11/02 10:54:35 by mtrautne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/utils.hpp"

void replaceAll(std::string &str, const std::string &oldStr, const std::string &newStr) {
	size_t pos = 0;
	while ((pos = str.find(oldStr, pos)) != std::string::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

std::string readFile(std::ifstream &file) {
	std::string content;
	std::string line;
	getline(file, line);
	content += line;
	while (getline(file, line))
		content += "\r\n" + line;
	return (content);
}

std::string readFile(const std::string &path) {
	std::ifstream file(path.c_str());
	if (!file.is_open())
		return ("");
	std::string content = readFile(file);
	file.close();
	return (content);
}

std::string getContentType(const std::string& path) {
	if (path.find(".html") != std::string::npos) {
		return "text/html";
	} else if (path.find(".css") != std::string::npos) {
		return "text/css";
	} else if (path.find(".js") != std::string::npos) {
		return "text/javascript";
	} else if (path.find(".png") != std::string::npos) {
		return "image/png";
	} else if (path.find(".jpg") != std::string::npos) {
		return "image/jpeg";
	} else if (path.find(".gif") != std::string::npos) {
		return "image/gif";
	} else if (path.find(".ico") != std::string::npos) {
		return "image/x-icon";
	} else {
		return "text/plain";
	}
}

std::string getHTTPErrorMessages(int statusCode) {
	switch (statusCode) {
		case OK:								return MSG_200;
		case CREATED:							return MSG_201;
		case ACCEPTED:							return MSG_202;
		case NO_CONTENT:						return MSG_204;
		case PARTIAL_CONTENT:					return MSG_206;

		case MULTIPLE_CHOICES:					return MSG_300;
		case MOVED_PERMANENTLY:					return MSG_301;
		case FOUND:								return MSG_302;
		case SEE_OTHER:							return MSG_303;
		case NOT_MODIFIED:						return MSG_304;
		case USE_PROXY:							return MSG_305;
		case TEMPORARY_REDIRECT:				return MSG_307;
		case PERMANENT_REDIRECT:				return MSG_308;

		case BAD_REQUEST:						return MSG_400;
		case UNAUTHORIZED:						return MSG_401;
		case PAYMENT_REQUIRED:					return MSG_402;
		case FORBIDDEN:							return MSG_403;
		case NOT_FOUND:							return MSG_404;
		case METHOD_NOT_ALLOWED:				return MSG_405;
		case NOT_ACCEPTABLE:					return MSG_406;
		case PROXY_AUTHENTICATION_REQUIRED:		return MSG_407;
		case REQUEST_TIMEOUT:					return MSG_408;
		case CONFLICT:							return MSG_409;
		case GONE:								return MSG_410;
		case LENGTH_REQUIRED:					return MSG_411;
		case PRECONDITION_FAILED:				return MSG_412;
		case REQUEST_ENTITY_TOO_LARGE:			return MSG_413;
		case REQUEST_URI_TOO_LONG:				return MSG_414;
		case UNSUPPORTED_MEDIA_TYPE:			return MSG_415;
		case REQUESTED_RANGE_NOT_SATISFIABLE:	return MSG_416;
		case EXPECTATION_FAILED:				return MSG_417;
		case IM_A_TEAPOT:						return MSG_418;

		case INTERNAL_SERVER_ERROR:				return MSG_500;
		case NOT_IMPLEMENTED:					return MSG_501;
		case BAD_GATEWAY:						return MSG_502;
		case SERVICE_UNAVAILABLE:				return MSG_503;
		case GATEWAY_TIMEOUT:					return MSG_504;
		case HTTP_VERSION_NOT_SUPPORTED:		return MSG_505;

		default: return "Unknown HTTP Error";
	}
}

void	printServerConfigMap(std::map<int, std::map<std::string, t_serverConfig> > &configMap) {
	std::map<int, std::map<std::string, t_serverConfig> >::iterator		portIt; // Iterator for all elements on port level

	std::cout << "┌GLOBAL VARS" << std::endl;
	std::cout << "├── Timeout: " << g_timeout << std::endl;
	std::cout << "├── maxClients: " << g_maxClients << std::endl;
	std::cout << "└── maxFileSize: " << g_maxFileSize << std::endl;
	// iterates over ports
	int	portNum = 0;
	for (portIt = configMap.begin(); portIt != configMap.end(); portIt++) {
		int port = portIt->first;
		std::cout << std::endl;
		std::cout << "┌PORTBLOCK: " << portNum << ", portMapKey: |" << port << "|" << std::endl;

		// iterates over individual servers
		std::map<std::string, t_serverConfig>& serverConfigsMap = portIt->second;
		std::map<std::string, t_serverConfig>::iterator serverConfigsIt;
		int	serverNum = 0;
		for (serverConfigsIt = serverConfigsMap.begin(); serverConfigsIt != serverConfigsMap.end(); serverConfigsIt++) {
			std::string serverName = serverConfigsIt->first;
			t_serverConfig &ServerConfig = serverConfigsIt->second;
			std::cout <<"├── SERVERBLOCK: |" << serverNum << "| serverKey: |" << serverName
						<< "| port: |" << portNum << "(" << port << ")|" << std::endl;
			std::cout << "│   ├── serverName: |" << ServerConfig.serverName << "|" << std::endl;
			std::cout << "│   ├── port: |" << ServerConfig.port << "|" << std::endl;
			std::cout << "│   ├── errorDir: |" << ServerConfig.errorDir << "|" << std::endl;
			std::cout << "│   └── LocationMap" << std::endl;

			// iterates over locations
			std::map<std::string, t_locationConfig>& locationMap = ServerConfig.locationMap;
			std::map<std::string, t_locationConfig>::iterator	locationConfigsIt;
			int	locationNum = 0;
			for (locationConfigsIt = locationMap.begin(); locationConfigsIt != locationMap.end(); locationConfigsIt++) {
				std::string locationPath = locationConfigsIt->first;
				t_locationConfig &locationConfig = locationConfigsIt->second;
				std::cout << "│       ├── LOCATIONBLOCK: |" << locationNum << "| locationKey: |" << locationPath
							<< "| server: |" << serverName << "| port: |" << port << "|" << std::endl;
				std::cout << "│       │   ├── path: |" << locationConfig.path << "|" << std::endl;
				std::cout << "│       │   ├── root: |" << locationConfig.root << "|" << std::endl;
				std::cout << "│       │   ├── index: |" << locationConfig.index << "|" << std::endl;
				std::cout << "│       │   ├── cgiExtension: |" << locationConfig.cgiExtension << "|" << std::endl;
				std::cout << "│       │   ├── upload: |" << locationConfig.upload << "|" << std::endl;
				std::cout << "│       │   ├── redirect: |" << locationConfig.redirect << "|" << std::endl;
				std::cout << "│       │   ├── allowedMethods: |" << locationConfig.allowedMethods << "|" << std::endl;
				std::cout << "│       │   └── autoIndex: |" << locationConfig.autoIndex << "|" << std::endl;
				locationNum++;
			}
		serverNum++;
		}
	portNum++;
	}
	std::cout <<  std::endl << "   ├──────###----END OF CONFIG MAP----###──────┤" << std::endl << std::endl;
}

void	removeLeadingWhitespaces(std::string &string) {

	size_t	start = 0;
	while (start < string.size() && (string[start] == ' ' || string[start] == '\t')) {
		start++;
	}
	string = string.substr(start, string.size() - start);
}

std::string	getValStr(const int& key) {
	switch (key) {
		case SERVERNAME:
			return "serverName";
		case PORT:
			return "port";
		case ERRORDIR:
			return "error directory";
		case PATH:
			return "path";
		case ROOT:
			return "root directory";
		case INDEX:
			return "index file";
		case CGI:
			return "cgi extension";
		case UPLOAD:
			return "upload directory";
		case REDIRECT:
			return "http redirection";
		case METHODS:
			return "allowed methods";
		case AUTOINDEX:
			return "auto index";
		default:
			return "unspecificConfigValue";
	}
}

int		stringToInt(const std::string &string) {
	std::istringstream stream(string);
	int number;

	if (stream >> number)
		return number;
	else{
		std::string	invalidInteger = RED "Error: " + string + " is not a valid integer." RESET;
		throw std::invalid_argument(invalidInteger);
	}
}
