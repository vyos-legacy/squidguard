/* 
 * Inspired by sample code from "Unix Network Programming" 
 * by W. Richard Stevens
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "sock_util.h"


int sock_debug = 0;

size_t
readn(int fd, void *vptr, size_t n)
{
   size_t  nleft;
   size_t  nread;
   char	   *ptr;
   
   if (sock_debug)
      printf("%d - readn(%d)\n", getpid(), n);
   ptr   = vptr;
   nleft = n;
   while (nleft > 0) {
      if ((nread = read(fd, ptr, nleft)) < 0) {
         if (sock_debug)
            printf("%d - read error [%s]\n", getpid(), strerror(errno));
         if (errno == EINTR)
            nread = 0;		/* and call read() again */
         else
            exit(1);
      } else if (nread == 0)    /* EOF */
         exit(1);		
      nleft -= nread;
      ptr   += nread;
   }
   return(n - nleft);		/* return >= 0 */
}       

size_t	
writen(int fd, const void *vptr, size_t n)
{
   size_t     nleft;
   size_t     nwritten;
   const char *ptr;
   
   if (sock_debug)
      printf("%d - writen(%d)\n", getpid(), n);
   ptr   = vptr;
   nleft = n;
   while (nleft > 0) {
      if ((nwritten = write(fd, ptr, nleft)) <= 0) {
         if (sock_debug)
            printf("%d - write error [%s]\n", getpid(), strerror(errno));
         if (nwritten < 0 && errno == EINTR)
            nwritten = 0;	/* and call write() again */
         else 
            exit(1);
      }
      nleft -= nwritten;
      ptr   += nwritten;
   }
   return(n);
}       

/* end of file */
