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
#include <signal.h>
#include <pthread.h>
#include <netdb.h>

#include <cerrno>
#include <cstring>
#include <cstdlib>
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
      // компонентные функции:
      //
      // tcp_socket()     - создание сокета
      // tcp_bind()       - привязка сокета
      // tcp_listen()     - прослушивание подключений
      // tcp_listen_ser() - 
      // tcp_accept()     - прием данных
      // tcp_connect()    - установка соединения
      // tcp_close()      - закрытие созданного сокета
      // tcp_recv()       - чтение данных из сокета
      // tcp_send()       - запись данных в сокет      
      // tcp_read()       - чтение данных из потока
      // tcp_readline()   - чтение данных из потока
      // tcp_write()      - запись данных в поток
      // tcp_writen()     - запись данных в поток
      // tcp_fork()       - порождение дочернего процесса
      // tcp_signal()     - обработчик сигналов 
      // tcp_shutdown()   - отправка сегмента FIN
      // tcp_setsockopt() - 
      // tcp_malloc()     - выделение памяти
      // tcp_calloc()     - выделение памяти
      // tcp_fgets()      - ввод из потока
      // tcp_fputs()      - вывод из потока
      //
      // tcp_pthread_create()      - создание потока
      // tcp_pthread_join()        - завершение потока
      // tcp_pthread_detach()      - отсоединение потока
      // tcp_pthread_kill()        - уничтожение потока
      // tcp_pthread_key_create()  - 
      // tcp_pthread_setspecific() - 
      // tcp_pthread_once()        - 
   private:
      int              _read_cnt;
      char*            _read_ptr;
      char             _read_buf[mr::MAXLINE];
      pthread_key_t    _rl_key;
      pthread_once_t   _rl_once {PTHREAD_ONCE_INIT};

      struct Rline {
         int     s_rl_cnt;
         char*   s_rl_bufptr;
         char    s_rl_buf[mr::MAXLINE];
      };
      //------------------------------------------------------------------------
      void error_ex(const char* str)
      {
         char errmsg[mr::MAXLINE];
         strcpy(errmsg,str);
         char* s = std::strerror(errno);
         throw Exception(strcat(errmsg,s));
      }
      //------------------------------------------------------------------------
      Sigfunc* _signal(int signo, Sigfunc* func)
      {
         struct sigaction act;
         struct sigaction oact;
         
         act.sa_handler = func;
         sigemptyset(&act.sa_mask);
         act.sa_flags = 0;

         if (signo==SIGALRM) {
            #ifdef SA_INTERRUPT
            act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
            #endif
         }
         else {
            #ifdef SA_RESTART
            act.sa_flags |= SA_RESTART; // SVR4, 44BSD
            #endif
         }
         if (sigaction(signo,&act,&oact)<0) return SIG_ERR;
         return oact.sa_handler;
      }
      //------------------------------------------------------------------------
      void _readline_destructor(void* ptr) {free(ptr);}
      //------------------------------------------------------------------------
      void _readline_once()
      {tcp_pthread_key_create(&_rl_key,_readline_destructor);}
      //------------------------------------------------------------------------
      ssize_t _my_read(Rline* tsd, int fd, char* ptr)
      {
         if (tsd->s_rl_cnt<=0) {
            again:
            if ((tsd->s_rl_cnt = read(fd,tsd->s_rl_buf,mr::MAXLINE))<0) {
               if (errno==EINTR) goto again;
               return -1;
            }
            else if (tsd->s_rl_cnt==0) return 0;
            tsd->s_rl_bufptr = tsd->s_rl_buf;
         }
         tsd->s_rl_cnt--;
         *ptr = *tsd->s_rl_bufptr++;
         return 1;
      }
      //------------------------------------------------------------------------
      ssize_t _readline(int fd, void* vptr, size_t maxlen) 
      {
         char      c {};
         char*     ptr {static_cast<char*>(vptr)};
         ssize_t   n {};
         ssize_t   rc {};
         Rline*    tsd;

         tcp_pthread_once(&_rl_once,_readline_once);
         if ((tsd = pthread_getspecific(_rl_key))==NULL) {
            tsd = tcp_calloc(1,sizeof(Rline));
            tcp_pthread_setspecific(_rl_key,tsd);
         }
         for (n=1; n<maxlen; n++) {
            if ((rc = _my_read(tsd,fd,&c))==1) {
               *ptr++ = c;
               if (c=='\n') break;
            }
            else if (rc==0) {
               *ptr = 0;
               return n-1;
            }
            else return -1;
         }
         *ptr = 0;
         return n;
      }
      //------------------------------------------------------------------------
      ssize_t _readlinebuf(void** vptrptr)
      {
         if (_read_cnt) *vptrptr = _read_ptr;
         return _read_cnt;
      }
      //------------------------------------------------------------------------
      ssize_t _writen(int fd, const void* vptr, size_t n)
      {
         const char*   ptr {static_cast<const char*>(vptr)};
         size_t        nleft {n};
         ssize_t       nwritten {};

         while (nleft>0) {
            if ((nwritten = write(fd,ptr,nleft))<=0) {
               if (nwritten<0 && errno==EINTR) nwritten = 0;
               else return -1;
            }
            nleft-=nwritten;
            ptr+=nwritten;
         }
         return n;
      }
      //------------------------------------------------------------------------
      int _tcp_listen_ser(const char* host, 
                          const char* serv, 
                          socklen_t* addrlenp)
      {
         int         listenfd {};
         int         n {};
         const int   on {1};

         struct addrinfo  hints;
         struct addrinfo* res;
         struct addrinfo* ressave;

         std::memset(&hints,0,sizeof(struct addrinfo));
         hints.ai_flags = AI_PASSIVE;
         hints.ai_family = AF_UNSPEC;
         hints.ai_socktype = SOCK_STREAM;

         if ((n = getaddrinfo(host,serv,&hints,&res))!=0)
            error_ex("E: tcp_listen_ser error - ");

         ressave = res;
         do {
            listenfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
            if (listenfd<0) continue;
            tcp_setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
            if (bind(listenfd,res->ai_addr,res->ai_addrlen)==0) break;
            tcp_close(listenfd);
         } while ((res = res->ai_next)!=NULL);

         if (res==NULL) error_ex("E: tcp_listen_ser error - ");
         tcp_listen(listenfd,mr::LISTENQ);
         if (addrlenp) *addrlenp = res->ai_addrlen;
         freeaddrinfo(ressave);
         return listenfd;
      }
   public:
      //------------------------------------------------------------------------
      int tcp_socket(int domain, int type, int protocol)
         // создание сокета
      {
         int   n {};
         if ((n = socket(domain,type,protocol))<0) 
            error_ex("E: socket error - ");
         return n;
      }
      //------------------------------------------------------------------------
      void tcp_bind(int fd, const struct sockaddr* addr, socklen_t len)
         // привязка сокета
      {
         if (bind(fd,addr,len)<0) 
            error_ex("E: bind error - ");
      }
      //------------------------------------------------------------------------
      void tcp_listen(int fd, int n)
         // прослушивание подключений
      {
         if (listen(fd,n)<0) 
            error_ex("E: listen error - ");
      }
      //------------------------------------------------------------------------
      int tcp_accept(int fd, struct sockaddr* addr, socklen_t* len)
         // прием данных
      {
         int   n {};
         again:
         if ((n = accept(fd,addr,len))<0) {
            #ifdef EPROTO
               if (errno==EPROTO || errno==ECONNABORTED)
            #else
               if (errno==ECONNABORTED)
            #endif
                  goto again;
               else      
                  error_ex("E: accept error - ");
         }
         return n;
      }
      //------------------------------------------------------------------------
      void tcp_connect(int fd, const struct sockaddr* addr, socklen_t len)
         // установка соединения
      {
         if (connect(fd,addr,len)<0) 
            error_ex("E: connect error - ");
      }
      //------------------------------------------------------------------------
      void tcp_close(int fd)
         // закрытие созданного сокета
      {
         if (close(fd)<0) 
            error_ex("E: close error - ");
      }
      //------------------------------------------------------------------------
      ssize_t tcp_recv(int fd, void* ptr, size_t nbytes, int flags)
         // чтение данных из сокета
      {
         ssize_t   n {};
         if ((n = recv(fd,ptr,nbytes,flags))<0) 
            error_ex("E: recv error - ");
         return n;
      }
      //------------------------------------------------------------------------
      void tcp_send(int fd, const void* ptr, size_t nbytes, int flags)
         // запись данных в сокет
      {
         if (send(fd,ptr,nbytes,flags)<0) 
            error_ex("E: send error - ");
      }
      //------------------------------------------------------------------------
      ssize_t tcp_read(int fd, void* ptr, size_t nbytes)
         // чтение данных из потока
      {
         ssize_t   n {};
         if ((n = read(fd,ptr,nbytes))<0) 
            error_ex("E: read error - ");
         return n;
      }
      //------------------------------------------------------------------------
      ssize_t tcp_readline(int fd, void* ptr, size_t maxlen)
         // чтение данных из потока
      {
         ssize_t   n {};
         if ((n = _readline(fd,ptr,maxlen))<0) 
            error_ex("E: readline error - ");
         return n;
      }
      //------------------------------------------------------------------------
      void tcp_write(int fd, const void* ptr, size_t nbytes)
         // запись данных в поток
      {
         if (write(fd,ptr,nbytes)<0) 
            error_ex("E: write error - ");
      }
      //------------------------------------------------------------------------
      void tcp_writen(int fd, const void* ptr, size_t nbytes)
         // запись данных в поток
      {
         if (_writen(fd,ptr,nbytes)!=nbytes) 
            error_ex("E: writen error - ");
      }
      //------------------------------------------------------------------------
      pid_t tcp_fork()
         // порождение дочернего процесса
      {
         pid_t   pid {};
         if ((pid = fork())<0) 
            error_ex("E: fork error - ");
         return pid;
      }
      //------------------------------------------------------------------------
      Sigfunc* tcp_signal(int signo, Sigfunc* func)
         // обработчик сигналов
      {
         Sigfunc* sigfunc;
         if ((sigfunc = _signal(signo,func))==SIG_ERR)
            error_ex("E: signal error - ");
         return sigfunc;
      }
      //------------------------------------------------------------------------
      void tcp_shutdown(int fd, int how)
         // установка признака конца файла в стандартном потоке ввода
         // отправка сегмента FIN
      {
         if (shutdown(fd,how)<0)
            error_ex("E: shutdown error - ");
      }
      //------------------------------------------------------------------------
      void tcp_pthread_create(pthread_t* tid, 
                              const pthread_attr_t* attr, 
                              void* (*func)(void*), 
                              void* arg)
         // создание потока
      {
         int   n {};
         if ((n = pthread_create(tid,attr,func,arg))==0) return;
         errno = n;
         error_ex("E: pthread_create error - ");
      }
      //------------------------------------------------------------------------
      void tcp_pthread_join(pthread_t tid, void** status)
         // завершение потока
      {
         int   n {};
         if ((n = pthread_join(tid,status))==0) return;
         errno = n;
         error_ex("E: pthread_join error - ");
      }
      //------------------------------------------------------------------------
      void tcp_pthread_detach(pthread_t tid)
         // отсоединение потока
      {
         int   n {};
         if ((n = pthread_detach(tid))==0) return;
         errno = n;
         error_ex("E: pthread_detach error - ");
      }
      //------------------------------------------------------------------------
      void tcp_pthread_kill(pthread_t tid, int signo)
         // уничтожение потока
      {
         int   n {};
         if ((n = pthread_kill(tid,signo))==0) return;
         errno = n;
         error_ex("E: pthread_kill error - ");
      }
      //------------------------------------------------------------------------
      void tcp_pthread_key_create(pthread_key_t* key, void (*func)(void *)) 
      {
         int   n {};
         if ((n = pthread_key_create(key,func))==0) return;
         errno = n;
         error_ex("E: pthread_key_create error - ");
      }
      //------------------------------------------------------------------------
      void tcp_pthread_setspecific(pthread_key_t key, const void* value) 
      {
         int   n {};
         if ((n = pthread_setspecific(key,value))==0) return;
         errno = n;
         error_ex("E: pthread_setspecific error - ");
      }
      //------------------------------------------------------------------------
      void tcp_pthread_once(pthread_once_t* ptr, void (*func)(void)) 
      {
         int   n {};
         if ((n = pthread_once(ptr,func))==0) return;
         errno = n;
         error_ex("E: pthread_once error - ");
      }
      //------------------------------------------------------------------------
      void tcp_setsockopt(int fd, 
                          int level, 
                          int optname, 
                          const void* optval, 
                          socklen_t optlen)
      {
         if (setsockopt(fd,level,optname,optval,optlen)<0)
            error_ex("E: setsockopt error - ");
      }
      //------------------------------------------------------------------------
      int tcp_listen_ser(const char* host, 
                         const char* serv, 
                         socklen_t* addrlenp)
      {return _tcp_listen_ser(host,serv,addrlenp);}
      //------------------------------------------------------------------------
      void* tcp_malloc(size_t size)
         // выделение памяти
      {
         void*   ptr;
         if ((ptr = malloc(size))==NULL)
            error_ex("E: malloc error - ");
         return ptr;
      }
      //------------------------------------------------------------------------
      void* tcp_calloc(size_t n, size_t size) 
         // выделение памяти
      {
         void*   ptr;
         if ((ptr = calloc(n,size))==NULL)
            error_ex("E: calloc error - ");
         return ptr;
      }
      //------------------------------------------------------------------------
      char* tcp_fgets(char* ptr, int n, FILE* stream)
         // ввод из потока
      {
         char*   rptr {};
         if ((rptr = fgets(ptr,n,stream))==NULL && ferror(stream))
            error_ex("E: fgets error - ");
         return rptr;
      }
      //------------------------------------------------------------------------
      void tcp_fputs(const char* ptr, FILE* stream)
         // вывод в поток
      {
         if (fputs(ptr,stream)==EOF)
            error_ex("E: fputs error - ");
      }
   };
}

#endif // PL_TCPIP_HH
