Plugin_D3D 1.8.1 (23.02.2013)
==============
Stable Release for CryEngine 3.4.4 (32/64 bit), DX9/DX11

New:
* DirectX 11 SwapChain Access

Changes:
* DirectX 11 Present Listeners now working
* DirectX 11 BeginScene Listener now same as PostPresent (compatibility)

Fixes:
* Fixed a sporadic assert/stack overflow in DX11 mode

Plugin_D3D 1.8.0 (08.02.2013)
==============
Stable Release for CryEngine 3.4.4 (32/64 bit), DX9/DX11

Plugin_D3D 1.7.0 (19.11.2012)
==============
Stable Release for CryEngine 3.4.3 (32/64 bit), DX9/DX11

Plugin_D3D 1.6.2 (02.11.2012)
==============
Stable Release for CryEngine 3.4.0 (32/64 bit), DX9/DX11

New:
* Sourcecode Release
* Now using the Plugin SDK

Changes:
* Cleanup

Plugin_D3D 1.6 (12.08.2012)
==============
Changes:
* Overall thread safety improved

Fixes:
* Fixed a sporadic stack overflow in DX11 mode

Plugin_D3D 1.5 (21.05.2012)
==============
New:
* FreeSDK 3.4 (DX11) Support
* Added GetType() function to public interface

Changes:
* Changed parameters of InjectTexture

Plugin_D3D 1.1.1 (19.04.2012)
==============
New:
* FreeSDK 3.4 (DX9) Support

Plugin_D3D 1.1 (06.04.2012)
==============
New:
* Added texture replacement API to public interface
* Removed DX version from filename (dynamically loads the correct version)

Fixes:
* Fixed Shutdown (in some cases you could run into memory access errors while closing CE3)
* Internal improvements (preparation for DX11)
* Renamed directory from "D3DPlugins" to "D3DPlugin"
* Renamed sample class from "Sample_impl.h" to "D3DPlugin_Sample_impl.h"
* Fixed UnregisterListener crash

Plugin_D3D 1.0.1 (05.03.2012)
==============-
Initial Public Release for FreeSDK 3.3.9 (DX9)
