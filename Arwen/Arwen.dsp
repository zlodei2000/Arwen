# Microsoft Developer Studio Project File - Name="Arwen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Arwen - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Arwen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Arwen.mak" CFG="Arwen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Arwen - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Arwen - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Arwen - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /vmg /GR /GX /O2 /I "." /I "..\3D" /I "..\Foundation" /I "..\Interface" /I "..\Occlusion" /I "..\Resource" /I "..\jpeg" /I "..\zlib" /I "..\libpng" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib uuid.lib opengl32.lib glu32.lib jpeg.lib libpng.lib zlib.lib winmm.lib dxguid.lib dinput.lib dinput8.lib /nologo /subsystem:windows /profile /debug /debugtype:both /machine:I386 /libpath:"..\jpeg" /libpath:"..\libpng" /libpath:"..\zlib" /libpath:"\sdk\dx8\lib"

!ELSEIF  "$(CFG)" == "Arwen - Win32 Debug"

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
# ADD CPP /nologo /ML /W3 /Gm /vmg /GR /GX /ZI /Od /I "." /I "..\3D" /I "..\Foundation" /I "..\Interface" /I "..\Occlusion" /I "..\Resource" /I "..\jpeg" /I "..\zlib" /I "..\libpng" /I "\sdk\dx8\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib uuid.lib opengl32.lib glu32.lib jpeg.lib libpng.lib zlib.lib winmm.lib dinput.lib dxguid.lib dinput8.lib /nologo /subsystem:windows /profile /debug /machine:I386 /libpath:"..\jpeg" /libpath:"..\libpng" /libpath:"..\zlib" /libpath:"\sdk\dx8\lib"

!ENDIF 

# Begin Target

# Name "Arwen - Win32 Release"
# Name "Arwen - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AccSubScene.cpp
# End Source File
# Begin Source File

SOURCE=.\Animator.cpp
# End Source File
# Begin Source File

SOURCE=.\Application.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Array.cpp
# End Source File
# Begin Source File

SOURCE=.\Arwen.rc
# End Source File
# Begin Source File

SOURCE=..\Resource\AseDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\AutoreleasePool.cpp
# End Source File
# Begin Source File

SOURCE=..\Interface\BasicCommands.cpp
# End Source File
# Begin Source File

SOURCE=.\Billboard.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\BmpDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\BoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\BoundingSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\BspTree.cpp
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

SOURCE=.\CompositeObject.cpp
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

SOURCE=..\Interface\Events.cpp
# End Source File
# Begin Source File

SOURCE=.\Explosion.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\FileSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\FileUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\Fire.cpp
# End Source File
# Begin Source File

SOURCE=.\Fog.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Font.cpp
# End Source File
# Begin Source File

SOURCE=.\Fountain.cpp
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

SOURCE=.\Light.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Lightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\LinearFog.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkedObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\MagicSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Math3D.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Matrix2D.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Matrix3D.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\Md2Decoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\Md3Decoder.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Mesh3D.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshObject.cpp
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

SOURCE=.\ParticleSystem.cpp
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

SOURCE=..\3D\Quaternion.cpp
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

SOURCE=.\RotationAnimator.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Set.cpp
# End Source File
# Begin Source File

SOURCE=.\Shader.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderPass.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Sky.cpp
# End Source File
# Begin Source File

SOURCE=.\SmokeTrail.cpp
# End Source File
# Begin Source File

SOURCE=.\Snow.cpp
# End Source File
# Begin Source File

SOURCE=..\Foundation\Stack.cpp
# End Source File
# Begin Source File

SOURCE=.\StencilSubScene.cpp
# End Source File
# Begin Source File

SOURCE=.\SubScene.cpp
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

SOURCE=..\3D\Transform2D.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Transform3D.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\Vector3D.cpp
# End Source File
# Begin Source File

SOURCE=..\3D\View.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Resource\WalDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\World.cpp
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

SOURCE=.\AccSubScene.h
# End Source File
# Begin Source File

SOURCE=.\Animator.h
# End Source File
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Array.h
# End Source File
# Begin Source File

SOURCE=..\Resource\AseDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\AutoreleasePool.h
# End Source File
# Begin Source File

SOURCE=..\Interface\BasicCommands.h
# End Source File
# Begin Source File

SOURCE=.\Billboard.h
# End Source File
# Begin Source File

SOURCE=..\Resource\BmpDecoder.h
# End Source File
# Begin Source File

SOURCE=..\3D\BoundingBox.h
# End Source File
# Begin Source File

SOURCE=..\3D\BoundingSphere.h
# End Source File
# Begin Source File

SOURCE=..\3D\BspTree.h
# End Source File
# Begin Source File

SOURCE=..\3D\Camera.h
# End Source File
# Begin Source File

SOURCE=..\Resource\CelDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Coder.h
# End Source File
# Begin Source File

SOURCE=.\ColDet.h
# End Source File
# Begin Source File

SOURCE=.\CompositeObject.h
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

SOURCE=..\Interface\Events.h
# End Source File
# Begin Source File

SOURCE=.\Explosion.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\FileSystem.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\FileUtils.h
# End Source File
# Begin Source File

SOURCE=.\Fire.h
# End Source File
# Begin Source File

SOURCE=.\Fog.h
# End Source File
# Begin Source File

SOURCE=..\3D\Font.h
# End Source File
# Begin Source File

SOURCE=.\Fountain.h
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

SOURCE=..\Interface\Input.h
# End Source File
# Begin Source File

SOURCE=..\Interface\InputReader.h
# End Source File
# Begin Source File

SOURCE=..\Resource\JpegDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Interface\Keys.h
# End Source File
# Begin Source File

SOURCE=.\Light.h
# End Source File
# Begin Source File

SOURCE=..\3D\LightMap.h
# End Source File
# Begin Source File

SOURCE=.\LinearFog.h
# End Source File
# Begin Source File

SOURCE=.\LinkedObject.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Log.h
# End Source File
# Begin Source File

SOURCE=.\MagicSphere.h
# End Source File
# Begin Source File

SOURCE=..\3D\Mapping.h
# End Source File
# Begin Source File

SOURCE=.\Math1D.h
# End Source File
# Begin Source File

SOURCE=..\3D\Math3D.h
# End Source File
# Begin Source File

SOURCE=..\3D\Matrix2D.h
# End Source File
# Begin Source File

SOURCE=..\3D\Matrix3D.h
# End Source File
# Begin Source File

SOURCE=..\Resource\Md2Decoder.h
# End Source File
# Begin Source File

SOURCE=..\Resource\Md3Decoder.h
# End Source File
# Begin Source File

SOURCE=..\3D\Mesh3D.h
# End Source File
# Begin Source File

SOURCE=.\MeshObject.h
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

SOURCE=..\Occlusion\OcclusionBuffer.h
# End Source File
# Begin Source File

SOURCE=..\3D\OpenGLView.h
# End Source File
# Begin Source File

SOURCE=..\Resource\PakFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\ParticleSystem.h
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

SOURCE=..\Resource\QuakeII.h
# End Source File
# Begin Source File

SOURCE=..\3D\Quaternion.h
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

SOURCE=..\Foundation\ResourceSource.h
# End Source File
# Begin Source File

SOURCE=..\Resource\RgbDecoder.h
# End Source File
# Begin Source File

SOURCE=.\RotationAnimator.h
# End Source File
# Begin Source File

SOURCE=.\SceneDecoder.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Set.h
# End Source File
# Begin Source File

SOURCE=.\Shader.h
# End Source File
# Begin Source File

SOURCE=.\ShaderPass.h
# End Source File
# Begin Source File

SOURCE=..\3D\Sky.h
# End Source File
# Begin Source File

SOURCE=.\SmokeTrail.h
# End Source File
# Begin Source File

SOURCE=.\Snow.h
# End Source File
# Begin Source File

SOURCE=..\Foundation\Stack.h
# End Source File
# Begin Source File

SOURCE=.\StencilSubScene.h
# End Source File
# Begin Source File

SOURCE=.\SubScene.h
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

SOURCE=..\3D\Transform2D.h
# End Source File
# Begin Source File

SOURCE=..\3D\Transform3D.h
# End Source File
# Begin Source File

SOURCE=..\Resource\typedefs.h
# End Source File
# Begin Source File

SOURCE=..\3D\Vector2D.h
# End Source File
# Begin Source File

SOURCE=..\3D\Vector3D.h
# End Source File
# Begin Source File

SOURCE=..\3D\Vector4D.h
# End Source File
# Begin Source File

SOURCE=..\3D\View.h
# End Source File
# Begin Source File

SOURCE=.\VisualObject.h
# End Source File
# Begin Source File

SOURCE=..\Resource\WalDecoder.h
# End Source File
# Begin Source File

SOURCE=.\World.h
# End Source File
# Begin Source File

SOURCE=..\Resource\ZipFileSystem.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Arwen.ico
# End Source File
# End Group
# End Target
# End Project
