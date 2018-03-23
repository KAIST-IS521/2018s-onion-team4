#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
using std::string;


#define BUF_SIZE 1024
namespace Socket{
  int ConnectServer(int port, string ip) //5555,"12.121.121.11"
  {
      int client_fd,len;
      struct sockaddr_in client_addr;
      char recv_data[BUF_SIZE];

      client_fd = socket(PF_INET,SOCK_STREAM,0);

      client_addr.sin_addr.s_addr = inet_addr(ip);
      client_addr.sin_family = AF_INET;
      client_addr.sin_port = htons(port);

      if(connect(client_fd,(struct sockaddr *)&client_addr,sizeof(client_addr))== -1)
      {
          //cant connect, error
          close(client_fd);
          return -1;
      }

      return client_fd;
  }
  struct sockaddr_in *Getipaddres(int fd){
    int len;
    struct sockaddr_in temp_addr;

    getpeername(fd, (struct sockaddr *)&temp_addr, (socklen_t*)&len);

    return &temp_addr;
    //struct sockaddr_in temp_addr=Getipaddres(fd);
  } //print ("%s",inet_ntoa(temp_addr));


}
