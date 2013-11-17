#ifndef _COMMAND_VAR_H_
#define _COMMAND_VAR_H_

extern char *szDebugName[];

extern unsigned long g_aulDebugMask[];
extern int g_debug_switch;

extern char g_sysname[];
extern const char *key_name[];

extern struct cmd_vty *vty;
extern cmd_vector_t *cmd_vec;
extern struct para_desc g_cmd_elem[];


extern int g_InputMachine_prev;
extern int g_InputMachine_now ;;
extern char g_tabbingString[CMD_MAX_CMD_ELEM_SIZE];  /* ���ʼ������ȫ���ҵ��ִ�*/
extern char g_tabString[CMD_MAX_CMD_ELEM_SIZE];      /* ���һ�β�ȫ������ */
extern int g_tabStringLenth;

extern key_handler_t key_resolver[];


#endif
