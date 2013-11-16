#ifndef _JUDGE_VAR_H_
#define _JUDGE_VAR_H_

#include "..\..\thirdpart\mysql\include\mysql.h"


extern MYSQL *mysql;     //mysql����
extern char query[1024]; //��ѯ���


extern char GL_username[MAX_NAME];
extern int GL_solutionId;
extern int GL_problemId;
extern int GL_languageId;
extern int GL_verdictId;
extern int GL_contestId;
extern int GL_time;
extern int GL_memory;
extern int GL_reJudge;
extern int GL_testcase;
extern time_t GL_submitDate;
extern char GL_languageName[100];
extern char GL_languageExt[10];
extern char GL_languageExe[10];
extern int GL_time_limit;
extern int GL_memory_limit;
extern int GL_spj;


extern string GL_source;


extern int isTranscoding;
extern int limitIndex;
extern int nProcessLimit;



extern char sourcePath[1024];
extern char exePath[1024];


extern char inFileName[MAX_PATH];
extern char outFileName[MAX_PATH];
extern char DebugFile[MAX_PATH];
extern char ErrorFile[MAX_PATH];


extern int g_oj_debug_switch;

#endif