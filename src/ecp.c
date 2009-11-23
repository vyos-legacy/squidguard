/*
 * Author: Stig Thormodsrud <stig@vyatta.com>
 * Date: October 2009  
 * Description: client code to issue a ECP request over a
 *              socket interface.  
 *
 * Note: This is essentially the same code that will be linked 
 *       into squidguard, but copied here for the stand-alone 
 *       vyattaguard client.
 * 
 * This code was originally developed by Vyatta, Inc.
 * Copyright (C) 2009 Vyatta, Inc.
 * All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>

#include "ecp.h"
#include "sock_util.h"

#define MAX_MSG_BUF 256
char classify_msg[MAX_MSG_BUF];    

int sockfd = -1;

int ecp_init(char *socket_path)
{
   struct sockaddr_un serv_addr;

   if (sockfd > 0)  // already been called
      return 0; 

   sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
   if (sockfd < 0) {
      return ECP_ESOCKET;
   }

   bzero((char *)&serv_addr, sizeof(serv_addr));
   serv_addr.sun_family = AF_UNIX;
   strncpy(serv_addr.sun_path, socket_path, strlen(socket_path));
   if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
      return ECP_ECONNECT;
   }

   return 0;
}

int ecp_url_classify(char *buffer, ecp_results *result)
{
   int n;
   int url_length;
   ecp_classify_cmd_t cmd;
   ecp_classify_rsp_t rsp;

   url_length = strlen(buffer);
   if (url_length >= ECP_MAX_URL) {
      return ECP_EMAXURL;
   }

   /*
    * send command
    */
   bzero(&cmd, sizeof(cmd));
   cmd.magic      = ECP_MAGIC;
   cmd.url_length = url_length;
   n = writen(sockfd, &cmd, sizeof(cmd));
   n = writen(sockfd, buffer, url_length);

   /*
    * block waiting for response
    */
   bzero(&rsp, sizeof(rsp));
   n = readn(sockfd, &rsp, sizeof (rsp));
   if (n != sizeof (rsp)) {
      return ECP_ERSPSZ;
   }       
   if (rsp.magic != ECP_MAGIC) {
      return ECP_EBADMAGIC;
   }

   int status = rsp.status;
   if (status >= 0) {
      int i;
      bzero(result, sizeof(ecp_results));
      for (i = 0; i < ECP_MAX_RESULTS && rsp.result[i] > 0; i++) {
         (*result)[i] = rsp.result[i];
      } 
   } else {
      int len      = rsp.length;
      int msg_size = len - sizeof (rsp);
      bzero(classify_msg, MAX_MSG_BUF);
      n = readn(sockfd, classify_msg, msg_size);
      status = ECP_ECLASSIFY;
   }
   return status;
}

int ecp_shutdown(void) 
{
   shutdown(sockfd, SHUT_RDWR);
   sockfd = -1;
   return 0;
}

char *ecp_errstring(int ret)
{
   char *msg = NULL;

   switch (ret) {
      case ECP_ESOCKET:
         msg = "Error failed to open socket";
         break;
      case ECP_ECONNECT:
         msg = "Error failed to connect to server";
         break;
      case ECP_ERSPSZ:
         msg = "Error bad response size";
         break;
      case ECP_EBADMAGIC:
         msg = "Error bad response magic";
         break;
      case ECP_ECLASSIFY:
         return classify_msg;
      case ECP_EMAXURL:
         msg = "Error max url length exceeded";
         break;
      default:
         msg = "Error undefined code";
         break;
   }
   return msg;
}

/* end of file */
