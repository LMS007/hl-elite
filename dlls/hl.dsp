# Microsoft Developer Studio Project File - Name="hl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=hl - Win32 SantaDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hl.mak" CFG="hl - Win32 SantaDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hl - Win32 Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hl - Win32 ServerDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hl - Win32 SantaDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/GoldSrc/dlls", ELEBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Releasehl"
# PROP Intermediate_Dir ".\Releasehl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ws2_32.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\hl.def" /out:".\Releasehl/hle.dll"
# SUBTRACT LINK32 /profile
# Begin Custom Build
TargetPath=.\Releasehl\hle.dll
InputPath=.\Releasehl\hle.dll
SOURCE="$(InputPath)"

"hle.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                                                          $(TargetPath)                                                                 C:\SIERRA\Half-Life\hle\dlls\  
	copy                                               $(TargetPath)                                                 C:\Steam\SteamApps\steams~1.com\half-life\hle\dlls\  
	
# End Custom Build

!ELSEIF  "$(CFG)" == "hl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\hl___Win"
# PROP BASE Intermediate_Dir ".\hl___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /Zi /I "..\dlls" /I "..\engine" /I "..\common" /I "..\game_shared" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o".\Debug/hle.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\hl.def" /out:".\Debug\hle.dll" /implib:".\Debug\hl.lib"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Custom Build - Copying to dlls folder
TargetPath=.\Debug\hle.dll
InputPath=.\Debug\hle.dll
SOURCE="$(InputPath)"

"hle.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                               $(TargetPath)                                                 C:\Steam\SteamApps\steams~1.com\half-life\hle\dlls\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "hl - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\hl___Win"
# PROP BASE Intermediate_Dir ".\hl___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Profilehl"
# PROP Intermediate_Dir ".\Profilehl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\engine" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /D "ASEQuerySDK.lib" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\hl.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib ASEQuerySDK.LIB ws2_32.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /def:".\hl.def" /out:".\Profilehl/hle.dll"
# Begin Custom Build - Copying to dlls folder
TargetPath=.\Profilehl\hle.dll
InputPath=.\Profilehl\hle.dll
SOURCE="$(InputPath)"

"hle.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                                              $(TargetPath)                                                              C:\hl\hle\dlls\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "hl - Win32 ServerDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "hl___Win32_ServerDebug"
# PROP BASE Intermediate_Dir "hl___Win32_ServerDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "hl___Win32_ServerDebug"
# PROP Intermediate_Dir "hl___Win32_ServerDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\game_shared" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\game_shared" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\engine" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\Debug/hle.bsc"
# ADD BSC32 /nologo /o".\Debug/hle.bsc"
LINK32=link.exe
# ADD BASE LINK32 odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\hl.def" /out:".\Debug\hle.dll" /implib:".\Debug\hl.lib"
# SUBTRACT BASE LINK32 /profile /nodefaultlib
# ADD LINK32 odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\hl.def" /out:".\Debug\hle.dll" /implib:".\Debug\hl.lib"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Custom Build - Copying to dlls folder
TargetPath=.\Debug\hle.dll
InputPath=.\Debug\hle.dll
SOURCE="$(InputPath)"

"hle.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                       $(TargetPath)         C:\HLServer\hle\dlls

# End Custom Build

!ELSEIF  "$(CFG)" == "hl - Win32 SantaDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "hl___Win32_SantaDebug"
# PROP BASE Intermediate_Dir "hl___Win32_SantaDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "hl___Win32_SantaDebug"
# PROP Intermediate_Dir "hl___Win32_SantaDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\game_shared" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\game_shared" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "QUIVER" /D "VOXEL" /D "QUAKE2" /D "VALVE_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\engine" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o".\Debug/hle.bsc"
# ADD BSC32 /nologo /o".\Debug/hle.bsc"
LINK32=link.exe
# ADD BASE LINK32 odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\hl.def" /out:".\Debug\hle.dll" /implib:".\Debug\hl.lib"
# SUBTRACT BASE LINK32 /profile /nodefaultlib
# ADD LINK32 odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\hl.def" /out:".\Debug\hle.dll" /implib:".\Debug\hl.lib"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Custom Build - Copying to dlls folder
TargetPath=.\Debug\hle.dll
InputPath=.\Debug\hle.dll
SOURCE="$(InputPath)"

"hle.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                               $(TargetPath)                                                C:\Steam\SteamApps\santag~1.com\half-life\hle\dlls\ 

# End Custom Build

!ENDIF 

# Begin Target

# Name "hl - Win32 Release"
# Name "hl - Win32 Debug"
# Name "hl - Win32 Profile"
# Name "hl - Win32 ServerDebug"
# Name "hl - Win32 SantaDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\aflock.cpp
# End Source File
# Begin Source File

SOURCE=.\agrunt.cpp
# End Source File
# Begin Source File

SOURCE=.\airtank.cpp
# End Source File
# Begin Source File

SOURCE=.\animating.cpp
# End Source File
# Begin Source File

SOURCE=.\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\apache.cpp
# End Source File
# Begin Source File

SOURCE=.\barnacle.cpp
# End Source File
# Begin Source File

SOURCE=.\barney.cpp
# End Source File
# Begin Source File

SOURCE=.\bigmomma.cpp
# End Source File
# Begin Source File

SOURCE=.\bloater.cpp
# End Source File
# Begin Source File

SOURCE=.\bmodels.cpp
# End Source File
# Begin Source File

SOURCE=.\bullsquid.cpp
# End Source File
# Begin Source File

SOURCE=.\buttons.cpp
# End Source File
# Begin Source File

SOURCE=.\cbase.cpp
# End Source File
# Begin Source File

SOURCE=.\client.cpp
# End Source File
# Begin Source File

SOURCE=.\combat.cpp
# End Source File
# Begin Source File

SOURCE=.\controller.cpp
# End Source File
# Begin Source File

SOURCE=.\crossbow.cpp
# End Source File
# Begin Source File

SOURCE=.\crowbar.cpp
# End Source File
# Begin Source File

SOURCE=.\defaultai.cpp
# End Source File
# Begin Source File

SOURCE=.\doors.cpp
# End Source File
# Begin Source File

SOURCE=.\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\egon.cpp
# End Source File
# Begin Source File

SOURCE=.\explode.cpp
# End Source File
# Begin Source File

SOURCE=.\flyingmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\func_break.cpp
# End Source File
# Begin Source File

SOURCE=.\func_tank.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gargantua.cpp
# End Source File
# Begin Source File

SOURCE=.\gauss.cpp
# End Source File
# Begin Source File

SOURCE=.\genericmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\ggrenade.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\gman.cpp
# End Source File
# Begin Source File

SOURCE=.\h_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\h_battery.cpp
# End Source File
# Begin Source File

SOURCE=.\h_cine.cpp
# End Source File
# Begin Source File

SOURCE=.\h_cycler.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\handgrenade.cpp
# End Source File
# Begin Source File

SOURCE=.\hassassin.cpp
# End Source File
# Begin Source File

SOURCE=.\headcrab.cpp
# End Source File
# Begin Source File

SOURCE=.\healthkit.cpp
# End Source File
# Begin Source File

SOURCE=.\hgrunt.cpp
# End Source File
# Begin Source File

SOURCE=.\wpn_shared\hl_wpn_glock.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_entities.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_flag.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_maploader.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_action.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_ctf.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_ctp.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_duel.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_ffa.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_lms.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_practice.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_taction.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_team.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_teamduel.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_tlms.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_mode_tourney.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_observer.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_utility.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_vadmin.cpp
# End Source File
# Begin Source File

SOURCE=.\hle_vote.cpp
# End Source File
# Begin Source File

SOURCE=.\hornet.cpp
# End Source File
# Begin Source File

SOURCE=.\hornetgun.cpp
# End Source File
# Begin Source File

SOURCE=.\houndeye.cpp
# End Source File
# Begin Source File

SOURCE=.\ichthyosaur.cpp
# End Source File
# Begin Source File

SOURCE=.\islave.cpp
# End Source File
# Begin Source File

SOURCE=.\items.cpp
# End Source File
# Begin Source File

SOURCE=.\leech.cpp
# End Source File
# Begin Source File

SOURCE=.\lights.cpp
# End Source File
# Begin Source File

SOURCE=.\m249.cpp
# End Source File
# Begin Source File

SOURCE=.\maprules.cpp
# End Source File
# Begin Source File

SOURCE=.\monstermaker.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters.cpp
# End Source File
# Begin Source File

SOURCE=.\monsterstate.cpp
# End Source File
# Begin Source File

SOURCE=.\mortar.cpp
# End Source File
# Begin Source File

SOURCE=.\mp5.cpp
# End Source File
# Begin Source File

SOURCE=.\mpstubb.cpp
# End Source File
# Begin Source File

SOURCE=.\nihilanth.cpp
# End Source File
# Begin Source File

SOURCE=.\osprey.cpp
# End Source File
# Begin Source File

SOURCE=.\pathcorner.cpp
# End Source File
# Begin Source File

SOURCE=.\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\plats.cpp
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_debug.c
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_math.c
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.c
# End Source File
# Begin Source File

SOURCE=.\python.cpp
# End Source File
# Begin Source File

SOURCE=.\rat.cpp
# End Source File
# Begin Source File

SOURCE=.\roach.cpp
# End Source File
# Begin Source File

SOURCE=.\rpg.cpp
# End Source File
# Begin Source File

SOURCE=.\satchel.cpp
# End Source File
# Begin Source File

SOURCE=.\schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\scientist.cpp
# End Source File
# Begin Source File

SOURCE=.\scripted.cpp
# End Source File
# Begin Source File

SOURCE=.\shockrifle.cpp
# End Source File
# Begin Source File

SOURCE=.\shotgun.cpp
# End Source File
# Begin Source File

SOURCE=.\singleplay_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\skill.cpp
# End Source File
# Begin Source File

SOURCE=.\sniper.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\soundent.cpp
# End Source File
# Begin Source File

SOURCE=.\spectator.cpp
# End Source File
# Begin Source File

SOURCE=.\sporelauncher.cpp
# End Source File
# Begin Source File

SOURCE=.\squadmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\squeakgrenade.cpp
# End Source File
# Begin Source File

SOURCE=.\subs.cpp
# End Source File
# Begin Source File

SOURCE=.\talkmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\tempmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\tentacle.cpp
# End Source File
# Begin Source File

SOURCE=.\triggers.cpp
# End Source File
# Begin Source File

SOURCE=.\tripmine.cpp
# End Source File
# Begin Source File

SOURCE=.\turret.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_gamemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\world.cpp
# End Source File
# Begin Source File

SOURCE=.\xen.cpp
# End Source File
# Begin Source File

SOURCE=.\zombie.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\activity.h
# End Source File
# Begin Source File

SOURCE=.\activitymap.h
# End Source File
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\basemonster.h
# End Source File
# Begin Source File

SOURCE=.\cbase.h
# End Source File
# Begin Source File

SOURCE=.\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=.\client.h
# End Source File
# Begin Source File

SOURCE=.\decals.h
# End Source File
# Begin Source File

SOURCE=.\defaultai.h
# End Source File
# Begin Source File

SOURCE=.\doors.h
# End Source File
# Begin Source File

SOURCE=.\effects.h
# End Source File
# Begin Source File

SOURCE=..\engine\eiface.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\explode.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\flyingmonster.h
# End Source File
# Begin Source File

SOURCE=.\func_break.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\gamerules.h
# End Source File
# Begin Source File

SOURCE=.\hle_cvars.h
# End Source File
# Begin Source File

SOURCE=.\hle_entities.h
# End Source File
# Begin Source File

SOURCE=.\hle_flag.h
# End Source File
# Begin Source File

SOURCE=.\hle_flagevents.h
# End Source File
# Begin Source File

SOURCE=.\hle_gamemodes.h
# End Source File
# Begin Source File

SOURCE=.\hle_maploader.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_action.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_ctf.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_ctp.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_duel.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_ffa.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_lms.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_practice.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_taction.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_team.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_teamduel.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_tlms.h
# End Source File
# Begin Source File

SOURCE=.\hle_mode_tourney.h
# End Source File
# Begin Source File

SOURCE=.\hle_utility.h
# End Source File
# Begin Source File

SOURCE=.\hle_vadmin.h
# End Source File
# Begin Source File

SOURCE=.\hle_vote.h
# End Source File
# Begin Source File

SOURCE=.\hornet.h
# End Source File
# Begin Source File

SOURCE=.\items.h
# End Source File
# Begin Source File

SOURCE=.\monsterevent.h
# End Source File
# Begin Source File

SOURCE=.\monsters.h
# End Source File
# Begin Source File

SOURCE=.\nodes.h
# End Source File
# Begin Source File

SOURCE=.\plane.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_debug.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_defs.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_info.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_materials.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_movevars.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.h
# End Source File
# Begin Source File

SOURCE=.\saverestore.h
# End Source File
# Begin Source File

SOURCE=.\schedule.h
# End Source File
# Begin Source File

SOURCE=.\scripted.h
# End Source File
# Begin Source File

SOURCE=.\scriptevent.h
# End Source File
# Begin Source File

SOURCE=.\skill.h
# End Source File
# Begin Source File

SOURCE=.\soundent.h
# End Source File
# Begin Source File

SOURCE=.\spectator.h
# End Source File
# Begin Source File

SOURCE=.\squadmonster.h
# End Source File
# Begin Source File

SOURCE=.\talkmonster.h
# End Source File
# Begin Source File

SOURCE=.\teamplay_gamerules.h
# End Source File
# Begin Source File

SOURCE=.\trains.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\weapons.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\common\nowin.h
# End Source File
# End Target
# End Project
