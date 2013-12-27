/*

����GUETOJ virtual-judge
http://onlinejudge.guet.edu.cn/guetoj/index.html

*/

#if(JUDGE_VIRTUAL == VOS_YES)
#include <windows.h>
#include <process.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <queue>
#include <string>
#include <sstream>

#include "tlhelp32.h"


#include "product\judge\include\judge_inc.h"


using namespace std;

char g_GUETtmps[VJUDGE_MAX_SIZE_BUF];

/* guet language list */
UCHAR g_aucGUETLanguageName[][VJUDGE_MAX_LANG_SIZE] = {
	"GCC",
	"G++",
	"C",
};

char guet_username[1000]="weizengke";
char guet_password[1000]="********";

ULONG GUET_getLanguageNameByID(ULONG id, UCHAR *ucLanguageName)
{
	if (id < 0 || id >= sizeof(g_aucGUETLanguageName)/VJUDGE_MAX_LANG_SIZE)
	{
		return OS_FALSE;
	}

	strcpy((char *)ucLanguageName, (char *)g_aucGUETLanguageName[id]);
	return OS_TRUE;
}

ULONG GUET_getLanguageIDByName(UCHAR *ucLanguageName, ULONG *id)
{
	USHORT usLoop = 0;

	for (usLoop = 0; usLoop <= sizeof(g_aucGUETLanguageName)/VJUDGE_MAX_LANG_SIZE; ++usLoop)
	{
		if (strcmp((CHAR*)ucLanguageName, (CHAR*)g_aucGUETLanguageName[usLoop]) == 0)
		{
			*id = usLoop;
			return OS_TRUE;
		}
	}

	return OS_FALSE;
}


ULONG GUET_getResult(string s, string &res)
{
    int pos=s.find("<font color=");
	if (-1 == pos)
	{
		return OS_FALSE;
	}

    while (s[pos]!='>') pos++;
    pos++;

    int st=pos;
    while (s[pos]!='<') pos++;
    res = s.substr(st,pos-st);

	return OS_TRUE;
}

ULONG GUET_getRunid(string s, string &res) {
    int pos=s.find("<td height=22px>");
	if (-1 == pos)
	{
		return OS_FALSE;
	}

    while (s[pos]!='>') pos++;
    pos++;

    int st=pos;
    while (s[pos]!='<') pos++;

    res = s.substr(st,pos-st);

	return OS_TRUE;
}


string GUET_getCEinfo_brief(char *filename)
{
	string res="",ts;
    FILE * fp=fopen(filename,"r");

    while (fgets(g_GUETtmps,1000000,fp))
    {
        ts=g_GUETtmps;
        if (ts.find("View Compilation Error")!=string::npos)
        {
            while (fgets(g_GUETtmps,1000000,fp))
			{
                ts=g_GUETtmps;
				int pos = ts.find("<pre>");
                if (pos !=string::npos)
				{
					res = ts.substr(pos + 5, ts.length() - pos - 5);

					while (fgets(g_GUETtmps,1000000,fp))
					{
						ts=g_GUETtmps;
						if (ts.find("</pre>")!=string::npos)
						{
							MSG_OUPUT_DBG("FOUND CE_INFO");
							break;
						}
						else
						{
							res=res+ts;
						}
					}
					break;
				}
			}
            break;
        }
    }
    fclose(fp);
    return res;
}



ULONG GUET_getUsedTime(string s, string &timeuse)
{
    int pos=s.find("MS</td>");
	if (-1 == pos)
	{
		return OS_FALSE;
	}

    int st=pos;
    while (s[pos]!='>') pos--;
    pos++;

    timeuse =  s.substr(pos,st-pos);
	return OS_TRUE;
}

ULONG GUET_getUsedMem(string s, string &memuse)
{
	int pos=s.find("K</td>");
	if (-1 == pos)
	{
		return OS_FALSE;
	}

	int st=pos;
	while (s[pos]!='>') pos--;
	pos++;
	memuse = s.substr(pos,st-pos);
	return OS_TRUE;
}

string GUET_getLineFromFile(char *filename,int line)
{
    string res="";
    FILE * fp=fopen(filename,"r");
    int cnt=0;

	if (NULL == fp)
	{
		pdt_debug_print("open filename:%s error.",filename);
		return res;
	}

	pdt_debug_print("filename:%s",filename);

    while (fgets(g_GUETtmps,10000000,fp))
	{
        cnt++;
        res=g_GUETtmps;
        if (res.find("Submit Date</a></th>")!=string::npos)
		{
			pdt_debug_print("Found status.");
            fgets(g_GUETtmps,10000000,fp);
            res=res+g_GUETtmps;
            fgets(g_GUETtmps,10000000,fp);
            res=res+g_GUETtmps;
            break;
        }
    }

	pdt_debug_print("cnt:%d",cnt);

    fclose(fp);
    return res;
}



int GUET_getGUETLangID(int GDOJlangID)
{
	int alang[25] = {0,2,3,0,1,5,0,0,4,0,0,0};

	return alang[GDOJlangID];
}


ULONG GUET_getStatus(string username, int pid,int lang, string &runid, string &result,string& ce_info,string &tu,string &mu)
{
	ULONG ulRet = OS_TRUE;
	tu=mu="0";
	string ts;

	MSG_OUPUT_DBG("Do get status...");

	ts = GUET_getLineFromFile(g_Vjudgetfilename,77);

	puts(ts.c_str());

	if(OS_FALSE == GUET_getUsedTime(ts, tu))
	{
		++ulRet;
		MSG_OUPUT_DBG("GUET_getUsedTime failed.");
	}

	if(OS_FALSE == GUET_getUsedMem(ts, mu))
	{
		++ulRet;
		MSG_OUPUT_DBG("GUET_getUsedMem failed.");
	}

	if(OS_FALSE == GUET_getRunid(ts, runid))
	{
		++ulRet;
		MSG_OUPUT_DBG("GUET_getRunid failed.");
	}

	if(OS_FALSE == GUET_getResult(ts, result))
	{
		++ulRet;
		MSG_OUPUT_DBG("GUET_getResult failed.");
	}

	judge_outstring("Info: problem[%d] language[%d]  verdict[%s] submissionID[%s] time[%s ms] memory[%s kb].\r\n", pid, lang, result.c_str(), runid.c_str(), tu.c_str(), mu.c_str());

	if (OS_TRUE != ulRet)
	{
		MSG_OUPUT_DBG("get record failed.");
		return OS_FALSE;
	}

	MSG_OUPUT_DBG("Get status success...");

	return OS_TRUE;
}

ULONG GUET_getStatusEx(char *username)
{

	char cmd_string[MAX_PATH];
	char current_path[MAX_PATH] = {0};
	char tmp_source_path[MAX_PATH] = {0};
	char tmp_return_path[MAX_PATH] = {0};

	int ret = OS_OK;

	string pid = "1000";
	string lang = "C++";
	string runid;
	string result;
	string ce_info;
	string tu;
	string mu;

	GetCurrentDirectory(sizeof(current_path),current_path);
	sprintf(tmp_return_path, "%s//OJ_TMP//guet-judge.tmp",current_path);
	strcpy(g_Vjudgetfilename,tmp_return_path);

	puts(tmp_return_path);

	sprintf(cmd_string,"python -O pytesser_v0.0.1\\guet-vjudge.py status %s %s",username,tmp_return_path);
	system(cmd_string) ;

	/* idת�� */
	int lang_id = GUET_getGUETLangID(GL_languageId);

	ret =GUET_getStatus(username, GL_vpid, lang_id, runid, result,ce_info,tu,mu);

	if (result.find("Queuing")!=string::npos
		|| result.find("Compiling")!=string::npos
		|| result.find("Running")!=string::npos)
	{
		pdt_debug_print("Get Status, Queuing or Compiling or Running , try again...");
		ret = OS_ERR;
	}

	if (result.find("Compilation Error")!=string::npos)
	{
		//��ȡ���������Ϣ
		sprintf(cmd_string,"python -O guet-vjudge.py ce %s %s",runid.c_str(), tmp_return_path);
		system(cmd_string) ;

		string CE_Info = GUET_getCEinfo_brief(tmp_return_path);
		ce_info = CE_Info;
	}

	return ret;

}

int GUET_Judge_python()
{
	char current_path[MAX_PATH] = {0};
	char tmp_source_path[MAX_PATH] = {0};
	char tmp_return_path[MAX_PATH] = {0};
	GetCurrentDirectory(sizeof(current_path),current_path);

	sprintf(tmp_source_path, "%s//%s",current_path,sourcePath);
	sprintf(tmp_return_path, "%s//OJ_TMP//guetjudge-%d.tmp",current_path,GL_solutionId);

	strcpy(g_Vjudgetfilename,tmp_return_path);

	do
	{
		int lang_id = GUET_getGUETLangID(GL_languageId);

		char cmd_string[MAX_PATH];
		sprintf(cmd_string,"python -O guet-vjudge.py submit %d %d %s %s %s %s",
				GL_vpid, lang_id, guet_username, guet_password, tmp_source_path,tmp_return_path);
		system(cmd_string) ;

		int tryTime = 6;
		int ret = OS_FALSE;
		string runid, result,ce_info,tu,mu;

		//hdu ��status��1
		lang_id += 1;

		while (ret != OS_TRUE)
		{
			result = "";
			MSG_OUPUT_DBG("Get Status...");

			Sleep(10000);

			sprintf(cmd_string,"python -O guet-vjudge.py status %s %s",
				guet_username,tmp_return_path);

			system(cmd_string) ;

			ret =GUET_getStatus(guet_username, GL_vpid, lang_id, runid, result,ce_info,tu,mu);

			if (result.find("Queuing")!=string::npos
				|| result.find("Compiling")!=string::npos
				|| result.find("Running")!=string::npos)
			{
				pdt_debug_print("Get Status, Queuing or Compiling or Running , try again...");
				ret = OS_FALSE;
			}

			if (result.find("Compilation Error")!=string::npos)
			{
				//��ȡ���������Ϣ
				sprintf(cmd_string,"python -O guet-vjudge.py ce %s %s",runid.c_str(), tmp_return_path);
				system(cmd_string) ;

				string CE_Info = GUET_getCEinfo_brief(g_Vjudgetfilename);
				ce_info = CE_Info;
			}

			tryTime --;
			/* ѭ���ȴ�60s */
			if (0 == tryTime)
			{
				break;
			}
		}

		if (OS_FALSE == ret)
		{
			MSG_OUPUT_DBG("Get Status Error...");
			GL_verdictId = V_SE;
		}
		else
		{
			MSG_OUPUT_DBG("Get Status success...");
			if (result.find("Accepted")!=string::npos)
			{
				GL_verdictId = V_AC;
			}
			else if (result.find("Presentation Error")!=string::npos)
			{
				GL_verdictId = V_PE;
			}
			else if (result.find("Runtime Error")!=string::npos)
			{
				GL_verdictId = V_RE;
			}
			else if (result.find("Time Limit Exceeded")!=string::npos)
			{
				GL_verdictId = V_TLE;
			}
			else if (result.find("Memory Limit Exceeded")!=string::npos)
			{
				GL_verdictId = V_TLE;
			}
			else if (result.find("Output Limit Exceeded")!=string::npos)
			{
				GL_verdictId = V_OLE;
			}
			else if (result.find("Wrong Answer")!=string::npos)
			{
				GL_verdictId = V_WA;
			}
			else if (result.find("Compilation Error")!=string::npos)
			{
				GL_verdictId = V_CE;
				FILE *fp;
				char buffer[4096]={0};
				if ((fp = fopen (DebugFile, "w")) == NULL){
					write_log(JUDGE_ERROR,"DebugFile open error");
					break;
				}
				fputs(ce_info.c_str(),fp);
				fclose(fp);
				SQL_updateCompileInfo(GL_solutionId);
			}
			else
			{
				GL_verdictId = V_SE;
			}
		}

		GL_time = atoi(tu.c_str());
		GL_memory = atoi(mu.c_str());

	} while (0);

	DeleteFile(tmp_return_path);

	return OS_TRUE;

}

int GUET_VJudge()
{
	return GUET_Judge_python();
}



#endif

