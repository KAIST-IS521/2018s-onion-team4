#ifndef __SOCKET__
#define __SOCKET__
#include <string>

using namespace std;

namespace Socket {
  int ConnectTo(int port, string ip);
  int ConnectTo(int port, uint32_t ip);
  uint32_t GetIPaddr(int fd);
  uint16_t GetPort(int fd);
}
#endif
