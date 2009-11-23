/*
 *
 * Module: ec.c
 *
 * **** License ****
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * This code was originally developed by Vyatta, Inc.
 * Portions created by Vyatta are Copyright (C) 2009 Vyatta, Inc.
 * All Rights Reserved.
 *
 * Author: Stig Thormodsrud
 * Date: August 2009
 * Description: interface to external classifier
 *
 * **** End License ****
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#include "sg.h"
#include "ecp.h"

/*
 * globals
 */
char tmp_lookup[ECP_MAX_URL];
char last_lookup[ECP_MAX_URL];
int  last_result_count = -1;
ecp_results last_result;      

int ec_debug = 0;

static
void sig_pipe(int signo)
{
   sgLogError("ec caught sigpipe, exiting...");
   ecp_shutdown();
   exit(1);
}

int ec_init(void)
{
    int ret = ecp_init(ECP_SOCKET_PATH);
    if (ret < 0) {
       sgLogFatalError("ec_init %s\n", strerror(errno));
       exit(1);
    }
    signal(SIGPIPE, sig_pipe);
    return 0;
}

#define MIN(A,B)	((A) < (B) ? (A):(B)) 

int is_category_in_result(int category, int result_count, ecp_results *result)
{
   int i;
   
   for (i = 0; i < result_count; i++) {
      if (ec_debug)
         sgLogError("result[%d] = %d", i, (*result)[i]);
      if ((*result)[i] == category) {
         return 1;
      }
   } 
   return 0;
}

static
void strip_url(char *url)
{
   char *p = NULL;

   p = strchr(url, '?');
   if (p) 
      p[0] = '\0';

   p = strchr(url, '#');
   if (p) 
      p[0] = '\0';
}

#if __STDC__
int ec_url_category(int category, char *lookup_url) 
#else
int ec_url_category(category, lookup_url)
     int   category;
     char *lookup_url;
#endif
{	
   ecp_results result;       
   int result_count;
   int len1, len2;

   if (strlen(lookup_url) > ECP_MAX_URL-1) {
      sgLogError("truncating long url %d", strlen(lookup_url));   
   }
   /*
    * Don't modify the original url.
    */
   strncpy(tmp_lookup, lookup_url, ECP_MAX_URL-1);                
   len1 = strlen(tmp_lookup);
   if (len1 == 0) {
      sgLogError("skipping 0 len url");
      return -1;
   }
   strip_url(tmp_lookup);
   len2 = strlen(tmp_lookup);

   if (strncmp(last_lookup, tmp_lookup, ECP_MAX_URL-1) == 0) {
      if (ec_debug) 
         sgLogError("cache match - result [%d]", last_result[0]);
      return is_category_in_result(category, last_result_count, &last_result);
   }

   /*
    * Do a new classification
    */
   if (ec_debug) {
      sgLogError("%d [%s]", len1, lookup_url);
      if (len1 > len2)
         sgLogError("%d [%s]", len2, tmp_lookup);
   }

   strncpy(last_lookup, tmp_lookup, ECP_MAX_URL-1);      
   result_count = ecp_url_classify(last_lookup, &result);
   if (ec_debug)
      sgLogError("ec result_count %d", result_count);
   if (result_count >= 0) {
      last_result_count = result_count;
      memcpy(last_result, result, sizeof(ecp_results));
      return  is_category_in_result(category, result_count, &result);
   } else {
      char *msg = ecp_errstring(result_count);
      sgLogError("ec classify error [%s]", msg);
      last_lookup[0] = 0;
      return -1;
   }
   return 0;
}

/* end of file */
