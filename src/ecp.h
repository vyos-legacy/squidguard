/*
 * ECP - external classification protocol
 */
#ifndef ECP_H
#define ECP_H

#define ECP_MAGIC 0x1215

/*
 * Max number of classifitions per url
 */
#define ECP_MAX_RESULTS     5

#define ECP_SOCKET_PATH "/var/run/ecp"

typedef int ecp_results[ECP_MAX_RESULTS];

/*
 * ECP_URL_CLASSIFY command
 *
 */
typedef struct ecp_classify_cmd_t_ {
      uint16_t   magic;
      uint16_t   url_length;  
      uint8_t    url[0]; 
} ecp_classify_cmd_t;

/*
 * ECP_URL_CLASSIFY response 
 *
 * status - Number of classification results.
 *          Negative values represents an error code, 
 *          the error message can be read after the
 *          header for length - sizeof(ecp_classify_rsp_t).
 */
typedef struct ecp_classify_rsp_t_ {
      uint16_t    magic;
      uint16_t    length;
      int32_t     status;        
      ecp_results result;
} ecp_classify_rsp_t;


/*
 * Error codes
 */
#define  ECP_ESOCKET    -1
#define  ECP_ECONNECT   -2
#define  ECP_ERSPSZ     -3
#define  ECP_EBADMAGIC  -4
#define  ECP_ECLASSIFY  -5


/*
 * function prototypes
 */
extern int  ecp_init(char *socket_path);
extern int  ecp_url_classify(char *buffer, ecp_results *result);
extern int  ecp_shutdown(void);
extern char *ecp_errstring(int ret);

#endif // ECP_H
