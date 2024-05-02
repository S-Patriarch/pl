//
// (c) 2024 S-Patriarch
// Patriarch library : tcpip.hh
//
#ifndef PL_TCPIP_HH
#define PL_TCPIP_HH

#ifndef PL_EXCEPT_HH
#include "except.hh"
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <cerrno>
#include <cstring>

namespace pl {
   namespace mr {
      constexpr int LISTENQ  = 1024; // максимальное количество клиентских соединений
      constexpr int MAXLINE  = 4096; // максимальная длина текстовой строки
      constexpr int BUFFSIZE = 8192; // размер буфера для чтения и записи
   }
   class TCPip {
      // компонентные функции:
      // tcp_socket()  - создание сокета
      // tcp_bind()    - привязка сокета
      // tcp_listen()  - прослушивание подключений
      // tcp_accept()  - прием данных
      // tcp_connect() - установка соединения
      // tcp_close()   - закрытие созданного сокета
      // tcp_recv()    - чтение данных из сокета
      // tcp_send()    - запись данных в сокет      
      // tcp_read()    - чтение данных из потока
      // tcp_write()   - запись данных в поток
      // tcp_fork()    - порождение дочернего процесса
   public:
      int tcp_socket(int domain, int type, int protocol)
         // создание сокета
      {
         int   n {};
         if ((n = socket(domain,type,protocol))<0) {
            char  errmsg[mr::MAXLINE] = "E: Socket error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
         return n;
      }
     void tcp_bind(int fd, const struct sockaddr* addr, socklen_t len)
         // привязка сокета
      {
         if (bind(fd,addr,len)<0) {
            char  errmsg[mr::MAXLINE] = "E: Bind error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
      }
      void tcp_listen(int fd, int n)
         // прослушивание подключений
      {
         if (listen(fd,n)<0) {
            char  errmsg[mr::MAXLINE] = "E: Listen error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
      }
      int tcp_accept(int fd, struct sockaddr* addr, socklen_t* len)
         // прием данных
      {
         int   n {};
         if ((n = accept(fd,addr,len))<0) {
            char  errmsg[mr::MAXLINE] = "E: Accept error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
         return n;
      }
      void tcp_connect(int fd, const struct sockaddr *addr, socklen_t len)
         // установка соединения
      {
         if (connect(fd,addr,len)<0) {
            char  errmsg[mr::MAXLINE] = "E: Connect error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
      }
      void tcp_close(int fd)
         // закрытие созданного сокета
      {
         if (close(fd)<0) {
            char  errmsg[mr::MAXLINE] = "E: Close error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
      }
      ssize_t tcp_recv(int fd, void* ptr, size_t nbytes, int flags)
         // чтение данных из сокета
      {
         ssize_t n {};
         if ((n = recv(fd,ptr,nbytes,flags))<0) {
            char  errmsg[mr::MAXLINE] = "E: Recv error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
         return n;
      }
      void tcp_send(int fd, const void* ptr, size_t nbytes, int flags)
         // запись данных в сокет
      {
         if (send(fd,ptr,nbytes,flags)<0) {
            char  errmsg[mr::MAXLINE] = "E: Send error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
      }
      ssize_t tcp_read(int fd, void* ptr, size_t nbytes)
         // чтение данных из потока
      {
         ssize_t n {};
         if ((n = read(fd,ptr,nbytes))<0) {
            char  errmsg[mr::MAXLINE] = "E: Read error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
         return n;
      }
      void tcp_write(int fd, const void* ptr, size_t nbytes)
         // запись данных в поток
      {
         if (write(fd,ptr,nbytes)<0) {
            char  errmsg[mr::MAXLINE] = "E: Write error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
      }
     pid_t tcp_fork()
         // порождение дочернего процесса
      {
         pid_t pid {};
         if ((pid = fork())<0) {
            char  errmsg[mr::MAXLINE] = "E: Fork error - ";
            char* s = std::strerror(errno);
            throw Exception(strcat(errmsg,s));
         }
         return pid;
      }
   };
}
#endif // PL_TCPIP_HH
