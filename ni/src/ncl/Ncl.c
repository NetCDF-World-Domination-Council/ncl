#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>
#include "defs.h"
#include "Symbol.h"
#include "NclData.h"
#include "Machine.h"
#include "DataSupport.h"
#include "NclType.h"
#include "TypeSupport.h"
#include <unistd.h>
#include <ncarg/hlu/ConvertP.h>
#include <ncarg/hlu/Error.h>
#include <ncarg/hlu/App.h>
#include <netcdf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#if defined(HPUX)
#include <dl.h>
#else
#include <dlfcn.h>
#endif
extern NhlClass NhlworkstationClass;



FILE *thefptr;
FILE *theoptr;
int cmd_line;
extern int cmd_line_is_set;
extern int cur_line_number;
extern char *cur_line_text;
extern int cur_line_maxsize;
extern char *cur_line_text_pos;

/*
#if     defined(SunOS) && (MAJOR == 4)
extern FILE *nclin;
extern int nclparse(int);
#else
*/
extern FILE *yyin;
extern int yyparse(int);
/*
#endif 
*/


#define BUFF_SIZE 512

extern FILE *error_fp;
extern FILE *stdout_fp ;
extern FILE *stdin_fp ;
extern int number_of_constants;


extern void nclprompt(
#if	NhlNeedProto
void * user_data,
int arg
#endif
);

extern void InitializeReadLine(
#if	NhlNeedProto
int opt
#endif
);

extern NhlErrorTypes _NclPreLoadScript(
#if     NhlNeedProto
char * /*path*/,
int /*status*/
#endif
);


main(int argc, char* argv[]) {

	int errid = -1;
	int appid;
	int k;
	int reset = 1;
	DIR *d;
	struct dirent *ent;
#if defined(HPUX)
	shl_t so_handle;
#else
	void *so_handle;
#endif
	char buffer[4*NCL_MAX_STRING];
	void (*init_function)(void);
	char *libpath;
	char *scriptpath;
	char *pt;
	char *tmp = NULL;

#ifdef YYDEBUG
	extern int yydebug;
	yydebug = 1;
#endif
error_fp = stderr;
stdout_fp = stdout;
stdin_fp = stdin;
	
	ncopts = NC_VERBOSE;

	cmd_line =isatty(fileno(stdin));

	error_fp = stderr;
	stdout_fp = stdout;
	fprintf(stdout," Copyright (C) 1995-2005 - All Rights Reserved   \n University Corporation for Atmospheric Research   \n NCAR Command Language Version %s   \n The use of this software is governed by a License Agreement.\n See http://www.ncl.ucar.edu for more details.\n",GetNCARGVersion());
/*
         k = (mode_t)umask(22);
	fprintf(stdout,"%d\n",k);

	stdout_fp = fopen("/dev/null","w");
*/

	stdin_fp = stdin;
	cur_line_text = NclMalloc((unsigned)512);
	cur_line_maxsize = 512;
	cur_line_text_pos = &(cur_line_text[0]);


#ifdef NCLDEBUG
	thefptr = fopen("ncl.tree","w");
	theoptr = fopen("ncl.seq","w");
#else
	thefptr = NULL;
	theoptr = NULL;
#endif
	NhlInitialize();
	NhlVACreate(&appid,"ncl",NhlappClass,NhlDEFAULT_APP,
		NhlNappDefaultParent,1,
		NhlNappUsrDir,"./",NULL);
	NhlPalLoadColormapFiles(NhlworkstationClass);
	errid = NhlErrGetID();
	NhlVAGetValues(errid,NhlNerrFileName,&tmp,NULL);
	
	if((tmp == NULL)||(!strcmp(tmp,"stderr"))){
		NhlVASetValues(errid,
			NhlNerrFilePtr,stdout,NULL);
	}
	_NclInitMachine();
	_NclInitSymbol();	
	_NclInitTypeClasses();
	_NclInitDataClasses();

/*
* Now handle default directories
*/
	if((libpath = getenv("NCL_DEF_LIB_DIR"))!=NULL) {
		d = opendir(_NGResolvePath(libpath));
		if(d != NULL) {
			while((ent = readdir(d)) != NULL) {
				if(*ent->d_name != '.') {
					sprintf(buffer,"%s/%s",_NGResolvePath(libpath),ent->d_name);
#if defined(HPUX)
					so_handle = shl_load(buffer,BIND_IMMEDIATE,0L);
#else
					so_handle = dlopen(buffer,RTLD_NOW);
					if(so_handle == NULL) {
						NhlPError(NhlFATAL,NhlEUNKNOWN," Could not open (%s): %s",buffer,dlerror());
					}
#endif
					if(so_handle != NULL) {
#if defined(HPUX)
						init_function = NULL;
						(void)shl_findsym(&so_handle, "Init",TYPE_UNDEFINED,(void*)&init_function);
#else
						init_function = dlsym(so_handle, "Init");
#endif
						if(init_function != NULL) {
							(*init_function)();
						} else {
#if defined(HPUX)
							shl_unload(so_handle);
#else
							dlclose(so_handle);
#endif
							NhlPError(NhlWARNING,NhlEUNKNOWN,"Could not find Init() in external file %s, file not loaded",buffer);
						}
					} 
				}
			}
		} else {
			closedir(d);
			NhlPError(NhlFATAL,NhlEUNKNOWN," Could not open (%s), no libraries loaded",libpath);
		}
		_NclResetNewSymStack();
	}
/*
	if(cmd_line)	
		fprintf(stdout_fp,"ncl %d> ",0);
*/
	if(cmd_line == 1) {
		InitializeReadLine(1);
		NclSetPromptFunc(nclprompt,NULL);
		cmd_line = 1;
		cmd_line_is_set = 1;
	} else {
		InitializeReadLine(0);
	}
	if((scriptpath = getenv("NCL_DEF_SCRIPTS_DIR"))!=NULL) {
		d = opendir(_NGResolvePath(scriptpath));
		if(d!= NULL) {
			while((ent = readdir(d)) != NULL) {
				if(*ent->d_name != '.') {
					sprintf(buffer,"%s/%s",_NGResolvePath(scriptpath),ent->d_name);
					pt = strrchr(buffer,'.');
					if(pt != NULL) {
						pt++;
						if(strncmp(pt,"ncl",3)==0) {
							if(_NclPreLoadScript(buffer,1) == NhlFATAL) {
								NhlPError(NhlFATAL,NhlEUNKNOWN,"Error loading default script");
							} else {
								yyparse(reset);
/*
								if(reset)
									reset = 0;
*/
							}
						} else {
							NhlPError(NhlFATAL,NhlEUNKNOWN,"Scripts must have the \".ncl\" file extension");
						}
					} else {
							NhlPError(NhlFATAL,NhlEUNKNOWN,"Scripts must have the \".ncl\" file extension");
					}
				}
			}
		} else {
			closedir(d);
                        NhlPError(NhlFATAL,NhlEUNKNOWN," Could not open (%s), no scripts loaded",scriptpath);
		}

	}
/*
#if     defined(SunOS) && (MAJOR == 4)
	nclparse(1);
#else
*/
	if(argc==1) {
		yyparse(reset);
	} else if(argc ==2 ) {
		strcpy(buffer,_NGResolvePath(argv[1]));
                if(_NclPreLoadScript(buffer,0) == NhlFATAL) {
                        NhlPError(NhlFATAL,NhlEUNKNOWN,"Error loading default script");
                } else {
                        yyparse(reset);
                }
        } else {
                NhlPError(NhlFATAL,NhlEUNKNOWN,"To many arguments, NCL only accepts a single script for execution");
        }

/*
#endif
*/
#ifdef NCLDEBUG
	fclose(thefptr);
	fprintf(stdout,"Number of unfreed objects %d\n",_NclNumObjs());
	_NclObjsSize(stdout);
	_NclNumGetObjCals(stdout);
	_NclPrintUnfreedObjs(theoptr);
	fprintf(stdout,"Number of constants used %d\n",number_of_constants);
	fclose(theoptr);
#endif
	NhlClose();
	exit(0);
}





#ifdef __cplusplus
}
#endif
