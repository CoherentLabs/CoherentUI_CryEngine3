#pragma once
#include <Cry_Vector3.h>

namespace CoherentUIPlugin
{

    class CCoherentGeometry
    {
        public:
            bool LoadFromObj( const char* objFile );
            bool IntersectWithRay( const Vec3& rayOriginModelSpace, const Vec3& rayDirectionModelSpace, float& outDist, float& outU, float& outV );

        private:
            static bool IntersectRayTriangle(
                const Vec3& rayOrigin, const Vec3& rayDirection,
                const Vec3& v0, const Vec3& v1, const Vec3& v2,
                float& t, float& u, float& v );

        private:
            struct Vertex
            {
                float X;
                float Y;
                float Z;

                float U;
                float V;
            };

            std::vector<Vec3> m_Vertices;
            std::vector<Vec2> m_TexCoords;
    };

}

