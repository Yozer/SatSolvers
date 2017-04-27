# Microsoft Developer Studio Project File - Name="C4" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=C4 - Win32 BoundChecker
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "C4.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "C4.mak" CFG="C4 - Win32 BoundChecker"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "C4 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "C4 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "C4 - Win32 BoundChecker" (based on "Win32 (x86) Console Application")
!MESSAGE "C4 - Win32 Release_2003Comp_Def1" (based on "Win32 (x86) Console Application")
!MESSAGE "C4 - Win32 Release_2003Comp_Def2" (based on "Win32 (x86) Console Application")
!MESSAGE "C4 - Win32 Release_2003Comp_Def3" (based on "Win32 (x86) Console Application")
!MESSAGE "C4 - Win32 Release_2004Comp" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "C4 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "D:\myfiles\Old_data\E\Download\sat\SparseMatrixImpl\infiles"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "NO_TCL" /D "PRINT_MODEL" /D "PRINT_IVS" /D "EXT2_PRINT" /D "PRINT_AIS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"D:\myfiles\Old_data\E\Download\sat\SparseMatrixImpl\infiles\Jerusat2_.exe"

!ELSEIF  "$(CFG)" == "C4 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "NO_TCL" /D "PRINT_MODEL" /D "PRINT_IVS" /D "EXT2_PRINT" /D "PRINT_AIS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Jerusat1.3-debug.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "C4 - Win32 BoundChecker"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "C4___Win32_BoundChecker"
# PROP BASE Intermediate_Dir "C4___Win32_BoundChecker"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C4___Win32_BoundChecker"
# PROP Intermediate_Dir "C4___Win32_BoundChecker"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "PRINT_IVS" /D "PRINT_AIS" /D "EXT2_PRINT" /D "DEL_PRINT" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl83.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl83.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def1"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "C4___Win32_Release_2003Comp_Def1"
# PROP BASE Intermediate_Dir "C4___Win32_Release_2003Comp_Def1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C4___Win32_Release_2003Comp_Def1"
# PROP Intermediate_Dir "C4___Win32_Release_2003Comp_Def1"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_1" /FR /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_1" /D "NO_TCL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl83.lib /nologo /subsystem:console /machine:I386 /out:"D:\myfiles\Old_data\E\Download\sat\SparseMatrixImpl\infiles\Jerusat_comp_1.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Jerusat1.2.a_win.exe"

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "C4___Win32_Release_2003Comp_Def2"
# PROP BASE Intermediate_Dir "C4___Win32_Release_2003Comp_Def2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C4___Win32_Release_2003Comp_Def2"
# PROP Intermediate_Dir "C4___Win32_Release_2003Comp_Def2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_1" /FR /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_2" /D "NO_TCL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl83.lib /nologo /subsystem:console /machine:I386 /out:"D:\myfiles\Old_data\E\Download\sat\SparseMatrixImpl\infiles\Jerusat_comp_1.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Jerusat1.2.b_win.exe"

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def3"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "C4___Win32_Release_2003Comp_Def3"
# PROP BASE Intermediate_Dir "C4___Win32_Release_2003Comp_Def3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C4___Win32_Release_2003Comp_Def3"
# PROP Intermediate_Dir "C4___Win32_Release_2003Comp_Def3"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_2" /FR /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_3" /D "NO_TCL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl83.lib /nologo /subsystem:console /machine:I386 /out:"D:\myfiles\Old_data\E\Download\sat\SparseMatrixImpl\infiles\Jerusat_comp_2.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Jerusat1.2.c_win.exe"
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2004Comp"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "C4___Win32_Release_2004Comp"
# PROP BASE Intermediate_Dir "C4___Win32_Release_2004Comp"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C4___Win32_Release_2004Comp"
# PROP Intermediate_Dir "C4___Win32_Release_2004Comp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_1" /D "NO_TCL" /FR /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEFAULT_4" /D "NO_TCL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Jerusat1.2.a_win.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Jerusat1.3.exe"

!ENDIF 

# Begin Target

# Name "C4 - Win32 Release"
# Name "C4 - Win32 Debug"
# Name "C4 - Win32 BoundChecker"
# Name "C4 - Win32 Release_2003Comp_Def1"
# Name "C4 - Win32 Release_2003Comp_Def2"
# Name "C4 - Win32 Release_2003Comp_Def3"
# Name "C4 - Win32 Release_2004Comp"
# Begin Group "inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\inc\C4dataStruct.h
# End Source File
# Begin Source File

SOURCE=.\inc\C4FBG.h
# End Source File
# Begin Source File

SOURCE=.\inc\C4FndBg.h
# End Source File
# Begin Source File

SOURCE=.\inc\C4Globals.h
# End Source File
# Begin Source File

SOURCE=.\inc\C4IVSStacks.h
# End Source File
# Begin Source File

SOURCE=.\inc\C4PQ.h
# End Source File
# Begin Source File

SOURCE=.\inc\C4Queue.h
# End Source File
# Begin Source File

SOURCE=.\inc\C4Stack.h
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\C4AisOps.c
# End Source File
# Begin Source File

SOURCE=.\src\C4AlgImpl.c
# End Source File
# Begin Source File

SOURCE=.\src\C4Aux.c
# End Source File
# Begin Source File

SOURCE=.\src\C4BoxOp.c
# End Source File
# Begin Source File

SOURCE=.\src\C4BuildInitCnf.c
# End Source File
# Begin Source File

SOURCE=.\src\C4ConfActions.c
# End Source File
# Begin Source File

SOURCE=.\src\C4ConfClsDel.c
# End Source File
# Begin Source File

SOURCE=.\src\C4FreeMemory.c
# End Source File
# Begin Source File

SOURCE=.\src\C4Imply.c
# End Source File
# Begin Source File

SOURCE=.\src\C4IVStacks.c
# End Source File
# Begin Source File

SOURCE=.\src\C4Layers.c
# End Source File
# Begin Source File

SOURCE=.\src\C4Main.c

!IF  "$(CFG)" == "C4 - Win32 Release"

!ELSEIF  "$(CFG)" == "C4 - Win32 Debug"

!ELSEIF  "$(CFG)" == "C4 - Win32 BoundChecker"

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def1"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def3"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2004Comp"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\C4MainSat2003.c

!IF  "$(CFG)" == "C4 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "C4 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "C4 - Win32 BoundChecker"

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def1"

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def2"

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2003Comp_Def3"

!ELSEIF  "$(CFG)" == "C4 - Win32 Release_2004Comp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\C4NewCC.c
# End Source File
# Begin Source File

SOURCE=.\src\C4PQNew.c
# End Source File
# Begin Source File

SOURCE=.\src\C4PrePushD1Block.c
# End Source File
# Begin Source File

SOURCE=.\src\C4Queue.c
# End Source File
# Begin Source File

SOURCE=.\src\C4Stack.c
# End Source File
# End Group
# End Target
# End Project
