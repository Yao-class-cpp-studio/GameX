#include "p2pnode.h"
#ifdef _WIN32
p2pnode::p2pnode() : is_initialized(false), sockfd(INVALID_SOCKET) {
  // Init Winsock
  WSADATA wsadata;
  int iRes = WSAStartup(MAKEWORD(2, 2), &wsadata);
  if (iRes) {
    std::cerr << "WSAStartUp failed: " << iRes << std::endl;
    exit(1);
  }
}

p2pnode::~p2pnode() {
  closesocket(sockfd);
  WSACleanup();
}

void p2pnode::initialize(uint16_t port) {
  if (is_initialized) {
    std::cerr << "Node is already initialized." << std::endl;
    return;
  }

  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd == INVALID_SOCKET) {
    std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
    WSACleanup();
    exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
    std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
    closesocket(sockfd);
    WSACleanup();
    exit(1);
  }

  is_initialized = true;
}

void p2pnode::send(const std::string &mes,
                   const std::string &ip,
                   uint16_t port) const {
  if (!is_initialized) {
    std::cerr << "Node is not initialized." << std::endl;
    return;
  }
  sockaddr_in dest_addr{};
  ZeroMemory(&dest_addr, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  InetPtonA(AF_INET, ip.c_str(), &(dest_addr.sin_addr));
  dest_addr.sin_port = htons(port);

  sendto(sockfd, mes.c_str(), mes.length(), 0, (struct sockaddr *)&dest_addr,
         sizeof(dest_addr));
}

std::tuple<std::string, std::string, uint16_t> p2pnode::receive() const {
  if (!is_initialized) {
    std::cerr << "Node is not initialized." << std::endl;
    return std::make_tuple("", "", 0);
  }

  char buffer[1024];
  sockaddr_in sender_addr{};
  int sender_addr_size = sizeof(sender_addr);

  int len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr *)&sender_addr, &sender_addr_size);
  if (len == SOCKET_ERROR) {
    std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
    closesocket(sockfd);
    WSACleanup();
    exit(1);
  }
  buffer[len] = '\0';

  char sender_ip[INET_ADDRSTRLEN];
  InetNtopA(AF_INET, &(sender_addr.sin_addr), sender_ip, INET_ADDRSTRLEN);
  int sender_port = ntohs(sender_addr.sin_port);

  return std::make_tuple(std::string(buffer), std::string(sender_ip),
                         sender_port);
}

void p2pnode::closeConnection() {
  if (sockfd != INVALID_SOCKET) {
    closesocket(sockfd);
    sockfd = INVALID_SOCKET;
  }
  is_initialized = false;
}

bool p2pnode::isInit() const {
  return is_initialized;
}

#elif __APPLE__
p2pnode::p2pnode() : is_initialized(false), is_server(false), sockfd(-1) {}

p2pnode::~p2pnode(){
  closeConnection();
}

void p2pnode::initialize(int port) {
  if (is_initialized) {
    std::cerr << "Node is already initialized." << std::endl;
    return;
  }

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error creating socket" << std::endl;
    exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cerr << "Error binding socket" << std::endl;
    exit(1);
  }

  is_initialized = true;
}

void p2pnode::send(const std::string& mes, const std::string& ip, uint16_t port) const {
  if (!is_initialized) {
    std::cerr << "Node is not initialized." << std::endl;
    return;
  }

  struct sockaddr_in dest_addr;
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  dest_addr.sin_port = htons(port);

  sendto(sockfd, mes.c_str(), mes.length(), 0,
         (struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

std::tuple<std::string, std::string, uint16_t> p2pnode::receive() {
  if (!is_initialized) {
    std::cerr << "Node is not initialized." << std::endl;
    return std::make_tuple("", "", 0);
  }

  char buffer[1024];
  struct sockaddr_in sender_addr;
  socklen_t sender_addr_size = sizeof(sender_addr);

  int len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr *)&sender_addr, &sender_addr_size);
  buffer[len] = '\0';

  std::string sender_ip = inet_ntoa(sender_addr.sin_addr);
  uint16_t sender_port = ntohs(sender_addr.sin_port);

  return std::make_tuple(std::string(buffer), sender_ip, sender_port);
}

void p2pnode::closeConnection() {
  if (sockfd != -1) {
    close(sockfd);
    sockfd = -1;
    is_initialized = false;
  }
}

bool p2pnode::isInited() const {
  return is_initialized;
}
#endif

#ifdef _WIN32
std::vector<std::string> localIPs() {
  std::vector<std::string> ips;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  auto pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
  PIP_ADAPTER_INFO pAdapter = nullptr;

  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
    free(pAdapterInfo);
    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
  }

  if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
    pAdapter = pAdapterInfo;
    while (pAdapter) {
      ips.emplace_back(pAdapter->IpAddressList.IpAddress.String);
      pAdapter = pAdapter->Next;
    }
  }

  if (pAdapterInfo) {
    free(pAdapterInfo);
  }

  return ips;
}
#elif __APPLE__

#define NI_MAXHOST 1025
// macOS implementation
std::vector<std::string> localIPs() {
  struct ifaddrs *ifaddr, *ifa;
  char host[NI_MAXHOST];
  std::vector<std::string> ips;

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    exit(EXIT_FAILURE);
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL)
      continue;

    if (ifa->ifa_addr->sa_family == AF_INET) { // Check for IPv4
      if (strcmp(ifa->ifa_name, "lo") != 0) {  // Exclude loopback interface
        int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
          std::cerr << "getnameinfo() failed: " << gai_strerror(s) << std::endl;
          continue;
        }

        ips.emplace_back(host);
      }
    }
  }

  freeifaddrs(ifaddr);
  return ips;
}

#endif