/*
 * Copyright(C) 2024, S-Patriarch
 * Patriarch library : tcpip.hh
 */

#ifndef PL_TCPIP_HH
#define PL_TCPIP_HH 1

#ifndef PL_EXCEPT_HH
#include "except.hh"
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include <cerrno>
#include <cstring>
#include <cstdio>

typedef void Sigfunc(int); // для обработчиков сигналов

namespace pl {
  namespace mr {
    constexpr int LISTENQ     = 1024; // максимальное количество клиентских соединений
    constexpr int MAXLINE     = 4096; // максимальная длина текстовой строки
    constexpr int BUFFSIZE    = 8192; // размер буфера для чтения и записи
    constexpr int MAXSOCKADDR = 128;  // максимальный размер структуры адреса сокета
    constexpr int SERV_PORT   = 9877; // клиент-серверы TCP и UDP
  }

  class TCPip {
    /*
     * компонентные функции:
     *
     * tcp_socket()   - создание сокета
     * tcp_bind()     - привязка сокета
     * tcp_listen()   - прослушивание подключений
     * tcp_accept()   - прием данных
     * tcp_connect()  - установка соединения
     * tcp_close()    - закрытие созданного сокета
     * tcp_recv()     - чтение данных из сокета
     * tcp_send()     - запись данных в сокет      
     * tcp_read()     - чтение данных из потока
     * tcp_readline() - чтение данных из потока
     * tcp_write()    - запись данных в поток
     * tcp_writen()   - запись данных в поток
     * tcp_fork()     - порождение дочернего процесса
     * tcp_signal()   - обработчик сигналов 
     * tcp_shutdown() - отправка сегмента FIN
     * tcp_fgets()    - ввод из потока
     * tcp_fputs()    - вывод в поток
     *
     * tcp_pthread_create() - создание потока
     * tcp_pthread_join()   - завершение потока
     * tcp_pthread_detach() - отсоединение потока
     * tcp_pthread_kill()   - уничтожение потока
     */
  private:
    int    _read_cnt;
    char*  _read_ptr;
    char   _read_buf[mr::MAXLINE];

    void error_ex(const char* str)
    {
      char errmsg[mr::MAXLINE];
      strcpy(errmsg, str);
      char* s = std::strerror(errno);
      throw Exception(strcat(errmsg, s));
    }

    Sigfunc* _signal(int signo, Sigfunc* func)
    {
      struct sigaction act;
      struct sigaction oact;
         
      act.sa_handler = func;
      sigemptyset(&act.sa_mask);
      act.sa_flags = 0;

      if (signo == SIGALRM) {
        #ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
        #endif
      }
      else {
        #ifdef SA_RESTART
        act.sa_flags |= SA_RESTART; // SVR4, 44BSD
        #endif
      }
      if (sigaction(signo, &act, &oact) < 0) return SIG_ERR;
        return oact.sa_handler;
    }

    ssize_t _my_read(int fd, char* ptr)
    {
      if (_read_cnt <= 0) {
        again:
        if ((_read_cnt = read(fd,_read_buf,sizeof(_read_buf))) < 0) {
          if (errno == EINTR) goto again;
          return -1;
        }
        else if (_read_cnt == 0) return 0;
          _read_ptr = _read_buf;
      }
      _read_cnt--;
      *ptr = *_read_ptr++;
      return 1;
    }

    ssize_t _readline(int fd, void* vptr, size_t maxlen) 
    {
      char    c {};
      char*   ptr {static_cast<char*>(vptr)};
      ssize_t n {};
      ssize_t rc {};

      for (n=1; n<maxlen; n++) {
        if ((rc = _my_read(fd, &c)) == 1) {
          *ptr++ = c;
          if (c == '\n') break;
        }
        else if (rc == 0) {
          *ptr = 0;
          return n - 1;
        }
        else return -1;
      }
      *ptr = 0;
      return n;
    }

    ssize_t _readlinebuf(void** vptrptr)
    {
      if (_read_cnt) *vptrptr = _read_ptr;
      return _read_cnt;
    }

    ssize_t _writen(int fd, const void* vptr, size_t n)
    {
      const char* ptr {static_cast<const char*>(vptr)};
      size_t      nleft {n};
      ssize_t     nwritten {};

      while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
          if (nwritten < 0 && errno == EINTR) nwritten = 0;
          else return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
      }
      return n;
    }
  public:
    int tcp_socket(int domain, int type, int protocol)
      // создание сокета
    {
      int n {};
      if ((n = socket(domain, type, protocol)) < 0) 
        error_ex("E: socket error - ");
      return n;
    }

    void tcp_bind(int fd, const struct sockaddr* addr, socklen_t len)
      // привязка сокета
    {
      if (bind(fd, addr, len) < 0) 
        error_ex("E: bind error - ");
    }

    void tcp_listen(int fd, int n)
      // прослушивание подключений
    {
      if (listen(fd, n) < 0) 
        error_ex("E: listen error - ");
    }

    int tcp_accept(int fd, struct sockaddr* addr, socklen_t* len)
      // прием данных
    {
      int n {};
      if ((n = accept(fd, addr, len)) < 0) 
        error_ex("E: accept error - ");
      return n;
    }

    void tcp_connect(int fd, const struct sockaddr* addr, socklen_t len)
      // установка соединения
    {
      if (connect(fd, addr, len) < 0) 
        error_ex("E: connect error - ");
    }

    void tcp_close(int fd)
      // закрытие созданного сокета
    {
      if (close(fd) < 0) 
        error_ex("E: close error - ");
    }

    ssize_t tcp_recv(int fd, void* ptr, size_t nbytes, int flags)
      // чтение данных из сокета
    {
      ssize_t n {};
      if ((n = recv(fd, ptr, nbytes, flags)) < 0) 
        error_ex("E: recv error - ");
      return n;
    }

    void tcp_send(int fd, const void* ptr, size_t nbytes, int flags)
      // запись данных в сокет
    {
      if (send(fd, ptr, nbytes, flags) < 0) 
        error_ex("E: send error - ");
    }

    ssize_t tcp_read(int fd, void* ptr, size_t nbytes)
      // чтение данных из потока
    {
      ssize_t n {};
      if ((n = read(fd, ptr, nbytes)) < 0) 
        error_ex("E: read error - ");
      return n;
    }

    ssize_t tcp_readline(int fd, void* ptr, size_t maxlen)
      // чтение данных из потока
    {
      ssize_t n {};
      if ((n = _readline(fd, ptr, maxlen)) < 0) 
        error_ex("E: readline error - ");
      return n;
    }

    void tcp_write(int fd, const void* ptr, size_t nbytes)
      // запись данных в поток
    {
      if (write(fd, ptr, nbytes) < 0) 
        error_ex("E: write error - ");
    }

    void tcp_writen(int fd, const void* ptr, size_t nbytes)
      // запись данных в поток
    {
      if (_writen(fd, ptr, nbytes) != nbytes) 
        error_ex("E: writen error - ");
    }

    pid_t tcp_fork()
      // порождение дочернего процесса
    {
      pid_t pid {};
      if ((pid = fork()) < 0) 
        error_ex("E: fork error - ");
      return pid;
    }

    Sigfunc* tcp_signal(int signo, Sigfunc* func)
      // обработчик сигналов
    {
      Sigfunc* sigfunc;
      if ((sigfunc = _signal(signo, func)) == SIG_ERR)
        error_ex("E: signal error - ");
      return sigfunc;
    }

    void tcp_shutdown(int fd, int how)
      // установка признака конца файла в стандартном потоке ввода
      // отправка сегмента FIN
    {
      if (shutdown(fd, how) < 0)
        error_ex("E: shutdown error - ");
    }

    char* tcp_fgets(char* str, int count, std::FILE* stream)
      // ввод из потока
    {
      char* rstr {};
      if ((rstr = fgets(str, count, stream)) == NULL && std::ferror(stream))
        error_ex("E: fgets error - ");
      return rstr;
    }

    void tcp_fputs(const char* str, std::FILE* stream)
      // вывод в поток
    {
      if (fputs(str, stream) == std::feof())
        error_ex("E: fputs error - ");
    }

    void tcp_pthread_create(pthread_t* tid, 
                            const pthread_attr_t* attr, 
                            void* (*func)(void*), 
                            void* arg)
      // создание потока
    {
      int n {};
      if ((n = pthread_create(tid, attr, func, arg)) == 0) return;
      errno = n;
      error_ex("E: pthread_create error - ");
    }

    void tcp_pthread_join(pthread_t tid, void** status)
      // завершение потока
    {
      int n {};
      if ((n = pthread_join(tid, status)) == 0) return;
      errno = n;
      error_ex("E: pthread_join error - ");
    }

    void tcp_pthread_detach(pthread_t tid)
      // отсоединение потока
    {
      int n {};
      if ((n = pthread_detach(tid)) == 0) return;
      errno = n;
      error_ex("E: pthread_detach error - ");
    }

    void tcp_pthread_kill(pthread_t tid, int signo)
      // уничтожение потока
    {
      int n {};
      if ((n = pthread_kill(tid, signo)) == 0) return;
      errno = n;
      error_ex("E: pthread_kill error - ");
    }
  };
}

#endif
