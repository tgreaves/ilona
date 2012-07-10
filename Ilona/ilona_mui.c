#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifdef _DCC
#define __inline
#endif

#include "ilona_MUI.h"

struct ObjApp * CreateApp(void)
{
   struct ObjApp * Object;

   APTR  MNlabel0Project, MNlabel0AboutIlona, MNlabel0Quit, GROUP_ROOT_0, Main_Window_O;
   APTR  GROUP_ROOT_1, GR_grp_2, LA_label_0;

#ifndef DISABLESTUFF
   static const struct Hook rexx_gc_hook = { { NULL, NULL}, (VOID *)rexx_getchannel, NULL, NULL } ;

   static struct MUI_Command IlonaRexxStruct[] =
   {
     { "get_channel", NULL,  NULL, &rexx_gc_hook, NULL },
     { NULL,  NULL,  NULL, NULL,           NULL }
   } ;
#endif

   if (!(Object = AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
      return(NULL);

   Object->STR_TX_label_0 = "\033cWelcome to Ilona";
   Object->STR_TX_label_1 = "\033c(c) Copyright 1997 Tristan Greaves\n\nEarly Test GUI Stages";

   Object->TX_label_0 = TextObject,
      MUIA_Background, MUII_FILLBACK2,
      MUIA_Frame, MUIV_Frame_Text,
      MUIA_Text_Contents, Object->STR_TX_label_0,
      MUIA_Text_SetMin, TRUE,
   End;

   Object->BT_label_0 = SimpleButton("Quit");

   Main_Window_O = GroupObject,
      MUIA_HelpNode, "Main_Window_O",
      Child, Object->TX_label_0,
      Child, Object->BT_label_0,
   End;

   GROUP_ROOT_0 = GroupObject,
      Child, Main_Window_O,
   End;

   Object->M_LB_O = WindowObject,
      MUIA_Window_Title, "Ilona",
      MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
      WindowContents, GROUP_ROOT_0,
   End;

   LA_label_0 = Label("\033cIlona v1.4 (AmigaOS)");

   Object->TX_label_1 = TextObject,
      MUIA_Background, MUII_TextBack,
      MUIA_Frame, MUIV_Frame_Text,
      MUIA_Text_Contents, Object->STR_TX_label_1,
      MUIA_Text_SetMin, TRUE,
   End;

   Object->BT_label_1 = SimpleButton("Sounds Great!");

   GR_grp_2 = GroupObject,
      MUIA_HelpNode, "GR_grp_2",
      Child, LA_label_0,
      Child, Object->TX_label_1,
      Child, Object->BT_label_1,
   End;

   GROUP_ROOT_1 = GroupObject,
      Child, GR_grp_2,
   End;

   Object->WI_label_1 = WindowObject,
      MUIA_Window_Title, "About Ilona...",
      MUIA_Window_ID, MAKE_ID('1', 'W', 'I', 'N'),
      MUIA_Window_CloseGadget, FALSE,
      MUIA_Window_DepthGadget, FALSE,
      MUIA_Window_SizeGadget, FALSE,
      WindowContents, GROUP_ROOT_1,
   End;

   MNlabel0AboutIlona = MenuitemObject,
      MUIA_Menuitem_Title, "About Ilona...",
      MUIA_Menuitem_Shortcut, "a",
   End;

   MNlabel0Quit = MenuitemObject,
      MUIA_Menuitem_Title, "Quit",
      MUIA_Menuitem_Shortcut, "q",
   End;

   MNlabel0Project = MenuitemObject,
      MUIA_Menuitem_Title, "Project",
      MUIA_Family_Child, MNlabel0AboutIlona,
      MUIA_Family_Child, MNlabel0Quit,
   End;

   Object->MN_label_0 = MenustripObject,
      MUIA_Family_Child, MNlabel0Project,
   End;

   Object->App = ApplicationObject,
      MUIA_Application_Author, "Tristan Greaves",
      MUIA_Application_Menustrip, Object->MN_label_0,
      MUIA_Application_Base, "ILONA",
      MUIA_Application_Title, "Ilona",
      MUIA_Application_Version, "$VER: Ilona  v1.4 (24.04.97)",
      MUIA_Application_Copyright, "(c) Copyright 1997 Tristan Greaves",
      MUIA_Application_Description, "Advanced IRC Channel Management System",
      MUIA_Application_HelpFile, "Ilona.Guide",
/*      MUIA_Application_UseRexx, TRUE, */
/*      MUIA_Application_Commands, IlonaRexxStruct, */
      SubWindow, Object->M_LB_O,
      SubWindow, Object->WI_label_1,
   End;


   if (!Object->App)
   {
      FreeVec(Object);
      return(NULL);
   }

   DoMethod(Object->App,
      MUIM_Notify, MUIA_Application_Active, TRUE,
      Object->App,
      3,
      MUIM_Set, MUIA_Application_Sleep, FALSE
      );

   DoMethod(Object->App,
      MUIM_Notify, MUIA_Application_Active, FALSE,
      Object->App,
      3,
      MUIM_Set, MUIA_Application_Sleep, TRUE
      );

   DoMethod(MNlabel0AboutIlona,
      MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
      Object->WI_label_1,
      3,
      MUIM_Set, MUIA_Window_Open, TRUE
      );

   DoMethod(MNlabel0Quit,
      MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
      Object->App,
      2,
      MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
      );

   DoMethod(Object->M_LB_O,
      MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
      Object->App,
      2,
      MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
      );

   DoMethod(Object->BT_label_0,
      MUIM_Notify, MUIA_Pressed, FALSE,
      Object->App,
      2,
      MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
      );

   DoMethod(Object->M_LB_O,
      MUIM_Window_SetCycleChain, Object->BT_label_0,
      0
      );

   DoMethod(Object->WI_label_1,
      MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
      Object->WI_label_1,
      3,
      MUIM_Set, MUIA_Window_Open, FALSE
      );

   DoMethod(Object->BT_label_1,
      MUIM_Notify, MUIA_Pressed, FALSE,
      Object->WI_label_1,
      3,
      MUIM_Set, MUIA_Window_Open, FALSE
      );

   DoMethod(Object->WI_label_1,
      MUIM_Window_SetCycleChain, Object->BT_label_1,
      0
      );

   set(Object->M_LB_O,
      MUIA_Window_Open, TRUE
      );


   return(Object);
}

void DisposeApp(struct ObjApp * Object)
{
   MUI_DisposeObject(Object->App);
   FreeVec(Object);
}
