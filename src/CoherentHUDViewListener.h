#pragma once

#include "CoherentViewListener.h"

namespace CoherentUIPlugin
{
    class CCoherentHUDViewListener : public CCoherentViewListener
    {
        virtual bool RaycastGeometry( const Vec3& origin, const Vec3& dir, float& outDist, int& outViewX, int& outViewY ) COHERENT_OVERRIDE;
        virtual bool ShouldSwapRedAndBlueChannels() const COHERENT_OVERRIDE;
    };
}
