CoherentUI Plugin for CryEngine SDK
=====================================

Coherent UI for CryEngine 3.4.5 (build 6666) Free SDK

Visit [our blog](http://blog.coherent-labs.com/2013/01/coherent-ui-in-cryengine-3-redux.html) for a review of the plugin and code walkthrough.

NOTE: build instructions in the blog post may be outdated, we changed the directory structure; Use the build instructions in this file.

Installation / Integration
==========================
Use the installer or extract the files to your CryEngine SDK Folder so that the Code and BinXX/Plugins directories match up.

The plugin manager will automatically load up the plugin when the game/editor is restarted or if you directly load it.

Installation of Coherent UI Plugin
--------------------------

Building the plugin yourself
---------

NOTE: We'll refer to the root CryEngine3 folder as **&lt;CE3&gt;** and the &lt;CE3&gt;/Code folder as **&lt;Code&gt;**.

1. Checkout the CoherentUI Plugin (this repository) in the `<Code>` directory.
2. Checkout [Plugin_SDK](https://github.com/hendrikp/Plugin_SDK) in the `<Code>` directory.
3. Checkout [Plugin_D3D](https://github.com/hendrikp/Plugin_D3D) in the `<Code>` directory.  
You should have the following folder structure now:  
`<Code>/CoherentUI_CryEngine3`  
`<Code>/Plugin_D3D`  
`<Code>/Plugin_SDK`
4. Get Coherent UI from http://coherent-labs.com/UIDownload/.
 - Extract the archive in `<Code>/CoherentUI_CryEngine3/CoherentUI`
 - Run `<Code>/CoherentUI_CryEngine3/CoherentUI/bootstrap_win32.bat`
 - *(Optional)* Run `<Code>/CoherentUI_CryEngine3/CoherentUI/bootstrap_win64.bat` if you intend to build 64-bit binaries
5. Build the plugins
 - Build `<Code>/Plugin_SDK/project/Plugin SDK.sln` (it is important to build this first since it generates a include file used by the other projects)
 - Build `<Code>/Plugin_D3D/project/D3D.sln`
 - Build `<Code>/CoherentUI_CryEngine3/project/CoherentUI.sln`
6. Copy the `<Code>/CoherentUI_CryEngine3/CoherentUI/lib/Win32/host` directory into `<CE3>/Bin32/Plugins/CoherentUI` (after the copy there should be a directory named *host* in the CoherentUI folder).
7. Copy the files in `<Code>/CoherentUI_CryEngine3/CoherentUI/lib/Win32` (without the host) into `<CE3>/Bin32`. You should have *CoherentUI.dll* in the `<CE3>/Bin32/` folder now.
 - *(Optional)* If you're building 64-bit binaries:
     - copy `<Code>/CoherentUI_CryEngine3/CoherentUI/lib/Win64/host` directory into `<CE3>/Bin64/Plugins/CoherentUI`
     - copy the files in `<Code>/CoherentUI_CryEngine3/CoherentUI/lib/Win64` (without the host) into `<CE3>/Bin64`. 
8. Copy the contents of the `<Code>/CoherentUI_CryEngine3/samples` folder into `<CE3>`, overwriting files when asked
9. Build the Game DLL using `<Code>/Solutions/CryEngine_GameCodeOnly.sln`.

NOTE: There is a sample flow graph in `<Code>/CoherentUI_CryEngine3/samples/Game/Levels/Singleplayer/ForestCOUI/CoUIFlowNodes.xml`.

Using the prebuilt versions
---------
NOTE: The prebuilt versions are targeted for **Coherent UI 1.2.1.1 ProTrial**

TODO: Build & commit binaries; Describe how to use them.

CVars / Commands
================
* ```prefix_samplecvar```
  TODO: Describe the cvar

Flownodes
=========
* ```CoherentUI_Plugin:OutputEntity``` TODO: Describe the flownodes inside your plugin
* ```CoherentUI_Plugin:OutputHUD``` TODO: Describe the flownodes inside your plugin

Objects
=======
TODO: Describe custom object classes inside your plugin

...