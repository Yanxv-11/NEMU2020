#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{int x=0;
if(args == NULL)
x=1;
else sscanf(args,"%d",&x);
cpu_exec(x);
return 0;
}

static int cmd_info(char *args)
{
	if(args[0]=='r')
	{
	printf("eax,%x",cpu.eax);
	printf("edx,%x",cpu.edx);
	printf("ecx,%x",cpu.ecx);
	printf("ebx,%x",cpu.ebx);
	printf("ebp,%x",cpu.ebp);
	printf("esi,%x",cpu.esi);
	printf("edi,%x",cpu.edi);
	printf("esp,%x",cpu.esp);
	}
	else
	{
	printf("order not yet in/n");
	}
	return 0;
}
	

static int cmd_x(char *args)
{
	int age;
	int n;
	char *cmd = strtok(args," ");
	sscanf (cmd,"%d",&n);
	ages = cmd+strlen(cmd)+1;
	sscanf (ages,"%x",&age);
	swadder_t a;
	for(int i=0;i<n;i++)
	{
	a=swadder_read(age+4*i,4);
	printf("0x%x/n",a);
	}
	return 0;
}



static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si","dan bu zhi xing",cmd_si},
	{ "info","print all registers",cmd_info},
	{ "x","scanning memory",cmd_x},

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
