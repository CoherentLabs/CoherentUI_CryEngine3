#include "StdAfx.h"
#include "CoherentGeometry.h"

namespace CoherentUIPlugin
{

    bool CCoherentGeometry::LoadFromObj( const char* objFile )
    {
        // Very, very primitive .obj parser
        // 1. Parse the positions ('v' lines)
        // 2. Parse the texcoords ('vt' lines)
        // 3. Parse faces ('f' lines); these must be declared after the positions and texcoords;
        //    For each face construct a vertex

        std::vector<float> positions;
        std::vector<float> texCoords;

        FILE* fin = fopen( objFile, "r" );
		if (!fin)
		{
			return false;
		}

        const int MAX_LINE_LEN = 1024;
        char line[MAX_LINE_LEN];

        while ( fgets( line, MAX_LINE_LEN, fin ) )
        {
            float x, y, z;

            if ( line[0] == '#' )
            {
                continue;
            }

            else if ( line[0] == 'v' && line[1] == ' ' )
            {
                // Position
                sscanf( line, "v %f %f %f", &x, &y, &z );
                positions.push_back( x );
                positions.push_back( y );
                positions.push_back( z );
            }

            else if ( line[0] == 'v' && line[1] == 't' )
            {
                // Texcoord
                sscanf( line, "vt %f %f", &x, &y );
                texCoords.push_back( x );
                texCoords.push_back( y );
            }

            else if ( line[0] == 'f' )
            {
                // Face
                int p[3];
                int t[3];
                sscanf( line, "f %d/%d %d/%d %d/%d", &p[0], &t[0], &p[1], &t[1], &p[2], &t[2] );

                for ( int i = 0; i < 3; ++i )
                {
                    float* posstart = &positions[( p[i] - 1 ) * 3];
                    float* texstart = &texCoords[( t[i] - 1 ) * 2];

                    m_Vertices.push_back( Vec3( posstart[0], posstart[1], posstart[2] ) );
                    m_TexCoords.push_back( Vec2( texstart[0], texstart[1] ) );
                }
            }
        }

        fclose( fin );

		return true;
    }

    bool CCoherentGeometry::IntersectWithRay( const Vec3& rayOriginModelSpace, const Vec3& rayDirectionModelSpace, float& outDist, float& outU, float& outV )
    {
        static const int NO_HIT = -1;
        int hitFaceIndex = NO_HIT; // Actually index of the hit face's first vertex index in the indices array, but never mind
        float minDist = std::numeric_limits<float>::max();
        float texU = 0;
        float texV = 0;

        for ( size_t i = 0; i < m_Vertices.size(); i += 3 )
        {
            float t, u, v;
            const Vec3& v0 = m_Vertices[i];
            const Vec3& v1 = m_Vertices[i + 1];
            const Vec3& v2 = m_Vertices[i + 2];

            if ( IntersectRayTriangle( rayOriginModelSpace, rayDirectionModelSpace, v0, v1, v2, t, u, v ) )
            {
                if ( t < minDist )
                {
                    minDist = t;
                    // Temporarily save barycentric coordinates here
                    texU = u;
                    texV = v;

                    hitFaceIndex = i;
                }
            }
        }

        if ( hitFaceIndex != NO_HIT )
        {
            // Compute texture coordinates from barycentric coordinates
            Vec2 texCoords =
                ( 1 - texU - texV ) * m_TexCoords[hitFaceIndex] +
                texU * m_TexCoords[hitFaceIndex + 1] +
                texV * m_TexCoords[hitFaceIndex + 2];

            outU = texCoords.x;
            outV = 1.0f - texCoords.y;
			outDist = minDist;

            return true;
        }

        return false;
    }

    bool CCoherentGeometry::IntersectRayTriangle(
        const Vec3& rayOrigin, const Vec3& rayDirection,
        const Vec3& v0, const Vec3& v1, const Vec3& v2,
        float& t, float& u, float& v )
    {
        // Moller-Trumbore ray-triangle intersection
        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;

        Vec3 pvec = rayDirection.cross( e2 );

        float det = e1.dot( pvec );

        static const float DET_EPSILON = 0.001f;

        // With culling
        if ( det < DET_EPSILON )
        {
            return false;
        }

        Vec3 tvec = rayOrigin - v0;
        u = tvec.dot( pvec );

        if ( u < 0 || u > det )
        {
            return false;
        }

        Vec3 qvec = tvec.cross( e1 );

        v = rayDirection.dot( qvec );

        if ( v < 0 || u + v > det )
        {
            return false;
        }

        float detRcp = 1.0f / det;
        t = e2.dot( qvec ) * detRcp;
        u *= detRcp;
        v *= detRcp;

        return true;
    }

}