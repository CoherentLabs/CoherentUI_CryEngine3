D3D Plugin for CryEngine
========================
Purpose of this plugin is to provide access to the Direct3D device and to the renderthread, so that custom Direct3D rendering can be achieved.

Installation / Integration
==========================
Use the installer or extract the files to your CryEngine SDK Folder so that the Code and BinXX/Plugins directory match up.

The plugin manager will automatically load up the plugin when the game/editor is restarted or if you directly load it.

Its recommended that you install the [DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)

Contributing
============
* Feature requests/bug reports on github:
 [Wishes / Issues](https://github.com/hendrikp/Plugin_D3D/issues)
* For patches and source contributions see [Plugin SDK Wiki: Contribution Guideline](https://github.com/hendrikp/Plugin_SDK/wiki/Contribution-Guideline)

C++ Integration
====================================================================
Follow these steps to integrate the D3D Plugin into your Plugin or GameDLL.

Repository Setup
----------------
* Clone the repository to your local machine to your Code directory e.g. ```C:\cryengine3_3.4.0\Code```.
* Use a github UI version like [Github for Windows](http://windows.github.com/)
* or if you already have git installed

```
    git clone https://github.com/hendrikp/Plugin_SDK.git
```

Compiler Settings
-----------------
* Open the solution CryEngine_GameCodeOnly.sln
  * or Open the solution of your plugin
* Open CryGame properties
  * or Open the properties of your plugin
* Set the Dropdowns to All Configurations, All Platforms
* Add to C/C++ -> General -> Additional Include Directories

```
$(SolutionDir)..\Plugin_D3D\inc
```

* Apply those properties and close the dialog

Building / Compiling
--------------------
* Use the supplied seperate solution to compile this project.
* Or use the supplied batch ```tools\build.bat``` to build this project only.
* Or use the supplied batch ```Plugin_SDK\build_all.bat``` to build all plugins in the Code directory and the GameDll.
* Or add the project file to your CryGame solution and the plugin manager as dependency for each plugin.

Using a Plugin from a Plugin (Dependency)
-----------------------------------------
* See [Plugin SDK Wiki: Plugin Dependencies](https://github.com/hendrikp/Plugin_SDK/wiki/Plugin-Dependencies)

Samples
=======

Getting the Direct3D Device
---------------------------
The same method applies to DirectX 11 (exchange interface and header names)

* Open the file where you want to access DirectX
* Add behind the existing includes the following:

```C++
	#include <IPluginManager.h>
    #include <IPluginD3D.h>
    #include <d3d9.h>
```

* To get the device use the following in the Plugin:

```C++
    IDirect3DDevice9* pDevice = gD3DSystem ? static_cast<IDirect3DDevice9*>(gD3DSystem->GetDevice()) : NULL;
```

* or in the GameDLL the following  :

```C++
	IPluginD3D* pPlugin = PluginManager::safeGetPluginConcreteInterface<D3DPlugin::IPluginD3D*>( "D3D" );
    IDirect3DDevice9* pDevice = pPlugin ? static_cast<IDirect3DDevice9*>(pPlugin->GetDevice()) : NULL;
```

* I recommend saving the device pointer somewhere for reuse.
(it won't change anymore during this run)

Realize custom rendering
------------------------
To use the device at the **correct** moment you have to create a Listener class. Use this sample as your
base by simply adding it to your project and creating an instance. This event listener runs in the renderthread.

This sample will make your rendering window green, but you are free to do something more meaningfull ;)

* [Sample File](sample/D3DPlugin_Sample_impl.h) (include into a cpp file of your choice or paste it there)
