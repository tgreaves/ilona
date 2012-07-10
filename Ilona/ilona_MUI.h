#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>
#include <clib/alib_protos.h>

struct ObjApp
{
	APTR	App;
	APTR	MN_label_0;
	APTR	M_LB_O;
	APTR	TX_label_0;
	APTR	BT_label_0;
	APTR	WI_label_1;
	APTR	TX_label_1;
	APTR	BT_label_1;
	char *	STR_TX_label_0;
	char *	STR_TX_label_1;
};


extern struct ObjApp * CreateApp(void);
extern void DisposeApp(struct ObjApp *);
