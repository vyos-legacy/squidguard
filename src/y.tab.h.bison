typedef union {
  char *string;
  char *tval;
  char *dval;
  char *dvalcron;
  int  *integer;
} YYSTYPE;
#define	WORD	258
#define	END	259
#define	START_BRACKET	260
#define	STOP_BRACKET	261
#define	WEEKDAY	262
#define	DESTINATION	263
#define	REWRITE	264
#define	ACL	265
#define	TIME	266
#define	TVAL	267
#define	DVAL	268
#define	DVALCRON	269
#define	SOURCE	270
#define	CIDR	271
#define	IPCLASS	272
#define	IPADDR	273
#define	DBHOME	274
#define	DOMAINLIST	275
#define	URLLIST	276
#define	EXPRESSIONLIST	277
#define	IPLIST	278
#define	DOMAIN	279
#define	USER	280
#define	USERLIST	281
#define	IP	282
#define	NL	283
#define	PASS	284
#define	REDIRECT	285
#define	LOGDIR	286
#define	SUBST	287
#define	CHAR	288
#define	WEEKLY	289
#define	DATE	290
#define	WITHIN	291
#define	OUTSIDE	292
#define	ELSE	293
#define	LOGFILE	294
#define	ANONYMOUS	295


extern YYSTYPE yylval;
