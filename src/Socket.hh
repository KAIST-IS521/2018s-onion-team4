#ifndef __SOCKET__
#define __SOCKET__
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>

using namespace std;

namespace Socket{
  int ConnectServer(int port,string ip);
  struct sockaddr_in *Getipaddres(int fd);
}
#endif
