#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Socket.hh"

namespace Socket {
  int ConnectServerInternal(int port, uint32_t ip) {
      int client_fd;
      struct sockaddr_in client_addr;

      client_fd = socket(AF_INET, SOCK_STREAM, 0);

      client_addr.sin_addr.s_addr = ip;
      client_addr.sin_family = AF_INET;
      client_addr.sin_port = htons(port);

      if (connect(client_fd, (struct sockaddr *)&client_addr,
                  sizeof(client_addr)) == -1)
      {
          close(client_fd);
          return -1;
      }

      return client_fd;
  }

  int ConnectTo(int port, string ip) {
      return ConnectServerInternal(port, inet_addr(ip.c_str()));
  }

  int ConnectTo(int port, uint32_t ip) {
      return ConnectServerInternal(port, htonl(ip));
  }

  uint32_t GetIPaddr(int fd) {
    struct sockaddr_in temp_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    getpeername(fd, (struct sockaddr *)&temp_addr, &len);

    return ntohl(temp_addr.sin_addr.s_addr);
  }

  uint16_t GetPort(int fd) {
    struct sockaddr_in temp_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    getpeername(fd, (struct sockaddr *)&temp_addr, &len);

    return ntohs(temp_addr.sin_port);
  }
}


