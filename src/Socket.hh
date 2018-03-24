#ifndef __SOCKET__
#define __SOCKET__
#include <cstring>
#include <string>

using namespace std;

namespace Socket {
  int ConnectTo(int port, string ip);
  int ConnectTo(int port, uint32_t ip);
  uint32_t GetIPaddr(int fd);
}
#endif
