/* Plugin SDK - for licensing and copyright see license.txt */

#pragma once

/* SDK Version detection */
#include <IRenderer.h>
#include <I3DEngine.h>

/* for reuse of compiled Cryengine SDK version (its not defined in CDK directly so we have to do it ourself) */
#if defined(DEFAULT_SID)
#define CDK_VERSION 344
#define PLUGIN_COMPILED_CDK_VERSION "3.4.4"

#elif defined(FRT_CLEAR_RESET_VIEWPORT)
#define CDK_VERSION 343
#define PLUGIN_COMPILED_CDK_VERSION "3.4.3"

#elif defined(R_DX11_RENDERER)
#define CDK_VERSION 340
#define PLUGIN_COMPILED_CDK_VERSION "3.4.0"

#else
#define CDK_VERSION 339
#define PLUGIN_COMPILED_CDK_VERSION "3.3.9"

#endif

/* make some defines for backward compatibility */
#if CDK_VERSION < 344
#define InputPortConfig_Null() {0} //!< End of Input Ports
#define OutputPortConfig_Null() {0} //!< End of Output Ports
#endif

#if CDK_VERSION < 340
#define VIRTUAL_SCREEN_WIDTH 800.0f //!< Width of the virtual screen, inside the plugin relative sizes are used.
#define VIRTUAL_SCREEN_HEIGHT 600.0f //!< Height of the virtual screen, inside the plugin relative sizes are used.
#endif
