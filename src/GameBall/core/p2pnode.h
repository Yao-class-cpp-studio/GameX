#ifndef GAMEX_P2PNODE_H
#define GAMEX_P2PNODE_H

const int MAX_PLAYER = 5;

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#ifdef _WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <iphlpapi.h>

#include <thread>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#elif __APPLE__
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#endif

class p2pnode {
 private:
#ifdef _WIN32
  SOCKET sockfd;
#elif __APPLE__
  int sockfd;
#endif
  bool is_initialized;
  struct sockaddr_in addr;

 public:
  p2pnode();
  ~p2pnode();
  void initialize(uint16_t port);
  bool isInit() const;
  void send(const std::string &mes, const std::string &ip, uint16_t port) const;
  [[nodiscard]] std::tuple<std::string, std::string, uint16_t> receive() const;
  void closeConnection();
  bool is_server;
};

std::vector<std::string> localIPs();

#endif  // GAMEX_P2PNODE_H
