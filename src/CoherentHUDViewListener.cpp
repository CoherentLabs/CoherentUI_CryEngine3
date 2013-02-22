#include "StdAfx.h"
#include "CoherentHUDViewListener.h"

#include <Coherent/UI/View.h>

namespace CoherentUIPlugin
{

	bool CCoherentHUDViewListener::RaycastGeometry(const Vec3& origin, const Vec3& dir, float& outDist, int& outViewX, int& outViewY)
	{
		if (!m_pView)
		{
			return false;
		}

		// Get the mouse position and check if it's over a solid pixel,
		// ignoring the input ray.
		POINT cursorPos;
		::GetCursorPos( &cursorPos );
		::ScreenToClient( HWND( gEnv->pRenderer->GetHWND() ), &cursorPos );

		// When using shared textures you should issue the query at the beginning
		// of the frame and fetch as late as possible for optimal performance.
		// Since the current state of the plugin supports only shared memory transport,
		// issuing a query and immediately fetching causes no performance penalty.
		m_pView->IssueMouseOnUIQuery(
			cursorPos.x / (float)m_pView->GetWidth(),
			cursorPos.y / (float)m_pView->GetHeight()
			);
		m_pView->FetchMouseOnUIQuery();

		if (m_pView->IsMouseOnView())
		{
			outDist = 0;
			outViewX = cursorPos.x;
			outViewY = cursorPos.y;

			return true;
		}

		return false;
	}

}
