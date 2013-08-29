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

Using the prebuilt versions
---------
NOTE: The prebuilt versions are targeted for **Coherent UI 1.2.1.1 ProTrial**

There are prebuilt binaries in the `<Code>/CoherentUI_CryEngine3/Prebuilt` folder. To use them, just copy them over to the appropriate directory of `<CE3>` (i.e. Bin32 or Bin64).

You should also copy the contents of the `<Code>/CoherentUI_CryEngine3/samples` folder into `<CE3>`, overwriting files when asked.

Sample flow graph
---------
There is a sample flow graph in `<Code>/CoherentUI_CryEngine3/samples/Game/Levels/Singleplayer/ForestCOUI/CoUIFlowNodes.xml`.
The easiest way to use it is to open the sandbox (editor), load the provided *ForestCOUI* level and import the flow graph into an entity.

As an example, we'll import the sample into *CoherentBreakableEntity*'s flow. Select the *CoherentBreakableEntity* entity and click on "Create" in the *Flow Graph* section of the rollup bar.
From the Flow Graph window, select `File -> Import...` and import *CoUIFlowNodes.xml*. Exit the Flow Graph window, save the level and export it to the engine (Ctrl+E). Now the flow graph will be used in the Launcher, too.

CVars / Commands
================
* ```prefix_samplecvar```
  TODO: Describe the cvar

Flownodes
=========

CoherentUI_Plugin:OutputEntity
------------------------------

Renders a Coherent UI View on an entity
  
**Inputs**
  
  - `Activate` Activate the node
  - `Url` Initial url to display on the view
  - `Width` Width of the view in pixels
  - `Height` Height of the view in pixels
  - `Transparent` Defines if the view should be transparent or not
  - `Clickable` Defines if the view should support click-through queries
  - `Mesh` Geometry object for calculating correct click coordinates on the entity
  - `SharedMemory` Defines if the view uses shared memory for the rendering data transport or shared textures

**Outputs**
  
  - `ViewID` Id of the view for further use (e.g. for the TriggerEvent node)

CoherentUI_Plugin:OutputHUD
---------------------------

Renders a Coherent UI View on the HUD
  
**Inputs**
  
  - `Activate` Activate the node
  - `Path` Path to the html files defining the HUD (e.g. *coui://Libs/UI/CoherentUI/hud/hud.html*)

**Outputs**

  - `ViewID` Id of the view for further use (e.g. for the TriggerEvent node)

CoherentUI_Plugin:HandleEvent
-----------------------------

Handles an event from a Coherent UI View with an optional string and boolean parameter
  
**Inputs**
  
  - `Activate` Activate the node
  - `ViewID` Id of the view as obtained from one of the output nodes
  - `Event` The event name

**Outputs**

  - `Arg1` String argument received from the view (optional)
  - `Arg2` Boolean argument received from the view (optional)

CoherentUI_Plugin:TriggerEvent
------------------------------

Triggers an event on a Coherent UI View with an optional boolean parameter
  
**Inputs**
  
  - `Activate` Activate the node
  - `ViewID` Id of the view as obtained from one of the output nodes
  - `Event` The event name
  - `Arg1` Boolean argument to be send to the view (optional)

CoherentUI_Plugin:TriggerEventFloat
-----------------------------------

Triggers an event on a Coherent UI View with up to four float parameter
  
**Inputs**
  
  - `Activate` Activate the node
  - `ViewID` Id of the view as obtained from one of the output nodes
  - `Event` The event name
  - `Arg1` First float argument to be send to the view (optional)
  - `Arg2` Second float argument to be send to the view (optional)
  - `Arg3` Third float argument to be send to the view (optional)
  - `Arg4` Fourth float argument to be send to the view (optional)

CoherentUI_Plugin:SendTokens
----------------------------

Triggers an event on a Coherent UI View that receives the JSON encoded game tokens
  
**Inputs**
  
  - `Activate` Activate the node
  - `ViewID` Id of the view as obtained from one of the output nodes
  - `Event` The event name (default: GameTokens)

CoherentUI_Plugin:SetInput
--------------------------

Switch input between player and Coherent UI. When enabled, all input is sent
to Coherent UI and the mouse cursor is displayed.
  
**Inputs**
  
  - `Activate` Activate the node
  - `Enabled` Boolean argument. True enables input to Coherent UI, false disables it

Objects
=======
TODO: Describe custom object classes inside your plugin

...