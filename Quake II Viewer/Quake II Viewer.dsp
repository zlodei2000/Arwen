# Microsoft Developer Studio Project File - Name="Quake II Viewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Quake II Viewer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Quake II Viewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Quake II Viewer.mak" CFG="Quake II Viewer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Quake II Viewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Quake II Viewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Quake II Viewer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Quake II Viewer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "..\3D" /I "..\Foundation" /I "..\Resource" /I "..\Interface" /I "..\jpeg" /I "..\libpng" /I "..\zlib" /I "." /I "..\Arwen" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib zlib.lib jpeg.lib libpng.lib winmm.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\jpeg" /libpath:"..\libpng" /libpath:"..\zlib"

!ENDIF 

# Begin Target

# Name "Quake II Viewer - Win32 Release"
# Name "Quake II Viewer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Application.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Array.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\AutoreleasePool.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\BasicCommands.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\BmpDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\BoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\BspDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\BspFile.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Camera.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\CelDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\ColDet.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\ConfigParser.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\Console.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\ConsoleCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\Controller.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\crc32.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Data.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Dictionary.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\DirectXInputReader.cpp
# End Source File
# Begin Source File

SOURCE=.\Entities.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\Events.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\FileSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\FileUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\Arwen\Fog.cpp
# End Source File
# Begin Source File

SOURCE=..\Arwen\Fog.h
# End Source File
# Begin Source File

SOURCE=..\3D\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Frustrum.cpp
# End Source File
# Begin Source File

SOURCE=.\GameController.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\GifDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\JpegDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\3d\Lightmap.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Math3D.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\matrix3D.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\Md2Decoder.cpp
# End Source File
# Begin Source File

SOURCE=..\3d\Mesh3D.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\Model.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\MutableData.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\ObjStr.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\OpenGLView.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\PakFileSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\PcxDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\PixelFormat.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Plane.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\PngDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Poly3D.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Pool.cpp
# End Source File
# Begin Source File

SOURCE=.\Quake2Level.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Queue.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Ray.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\ResourceDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\ResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\ResourceSource.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\RgbDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Set.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Sky.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Texture.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\TgaDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\TgaEncoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Timer.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Transform3D.cpp
# End Source File
# Begin Source File

SOURCE=..\3d\View.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\WalDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\ZipFileSystem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\3D\3DDefs.h
# End Source File
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Array.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\AutoreleasePool.h
# End Source File
# Begin Source File

SOURCE=..\Interface\BasicCommands.h
# End Source File
# Begin Source File

SOURCE=..\Resource\BmpDecoder.h
# End Source File
# Begin Source File

SOURCE=..\3D\BoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\BspFile.h
# End Source File
# Begin Source File

SOURCE=..\3D\Camera.h
# End Source File
# Begin Source File

SOURCE=..\Resource\CelDecoder.h
# End Source File
# Begin Source File

SOURCE=.\ColDet.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\ConfigParser.h
# End Source File
# Begin Source File

SOURCE=..\Interface\Console.h
# End Source File
# Begin Source File

SOURCE=..\Interface\ConsoleCommand.h
# End Source File
# Begin Source File

SOURCE=..\Interface\Controller.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Data.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Dictionary.h
# End Source File
# Begin Source File

SOURCE=..\Interface\DirectXInputReader.h
# End Source File
# Begin Source File

SOURCE=.\Entities.h
# End Source File
# Begin Source File

SOURCE=..\Interface\Events.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\FileUtils.h
# End Source File
# Begin Source File

SOURCE=..\3D\Font.h
# End Source File
# Begin Source File

SOURCE=..\3D\Frustrum.h
# End Source File
# Begin Source File

SOURCE=.\GameController.h
# End Source File
# Begin Source File

SOURCE=..\Resource\GifDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Resource\JpegDecoder.h
# End Source File
# Begin Source File

SOURCE=..\3d\LightMap.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Log.h
# End Source File
# Begin Source File

SOURCE=..\3d\Mapping.h
# End Source File
# Begin Source File

SOURCE=..\3D\math3D.h
# End Source File
# Begin Source File

SOURCE=..\3D\matrix3D.h
# End Source File
# Begin Source File

SOURCE=..\Resource\Md2Decoder.h
# End Source File
# Begin Source File

SOURCE=..\3D\Mesh3D.h
# End Source File
# Begin Source File

SOURCE=..\Interface\Model.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\MutableData.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Object.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\ObjStr.h
# End Source File
# Begin Source File

SOURCE=..\3D\OpenGLView.h
# End Source File
# Begin Source File

SOURCE=..\Resource\PakFileSystem.h
# End Source File
# Begin Source File

SOURCE=..\Resource\PcxDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Resource\PixelFormat.h
# End Source File
# Begin Source File

SOURCE=..\3D\Plane.h
# End Source File
# Begin Source File

SOURCE=..\Resource\PngDecoder.h
# End Source File
# Begin Source File

SOURCE=..\3D\Poly3D.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Pool.h
# End Source File
# Begin Source File

SOURCE=..\3D\Portal.h
# End Source File
# Begin Source File

SOURCE=.\Quake2Level.h
# End Source File
# Begin Source File

SOURCE=..\Resource\QuakeII.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Queue.h
# End Source File
# Begin Source File

SOURCE=..\3D\Ray.h
# End Source File
# Begin Source File

SOURCE=..\Resource\ResourceDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Resource\ResourceManager.h
# End Source File
# Begin Source File

SOURCE=..\Resource\RgbDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Set.h
# End Source File
# Begin Source File

SOURCE=..\3D\Sky.h
# End Source File
# Begin Source File

SOURCE=..\3D\Texture.h
# End Source File
# Begin Source File

SOURCE=..\Resource\TgaDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Resource\TgaEncoder.h
# End Source File
# Begin Source File

SOURCE=..\Resource\TgaStructs.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Timer.h
# End Source File
# Begin Source File

SOURCE=..\3D\Transform3D.h
# End Source File
# Begin Source File

SOURCE=..\Resource\Typedefs.h
# End Source File
# Begin Source File

SOURCE=..\3D\Vector2D.h
# End Source File
# Begin Source File

SOURCE=..\3D\vector3D.h
# End Source File
# Begin Source File

SOURCE=..\3D\Vector4D.h
# End Source File
# Begin Source File

SOURCE=..\3D\View.h
# End Source File
# Begin Source File

SOURCE=..\Resource\WalDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Resource\ZipFileSystem.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\sdk\dx7\lib\dxguid.lib
# End Source File
# Begin Source File

SOURCE=..\..\sdk\dx7\lib\dinput.lib
# End Source File
# End Target
# End Project
