/* CoherentUI_Plugin - for licensing and copyright see license.txt */

#pragma once

#include <Coherent/UI/FileHandler.h>
#include <Coherent/UI/URLParse.h>

namespace CoherentUIPlugin
{
    /**
    * @brief Override File Operations for CryEngine to support pak files
    */
    class PakFileHandler :
        public Coherent::UI::FileHandler
    {
    public:
        PakFileHandler();
        virtual void ReadFile(const wchar_t*  sURL, Coherent::UI::ResourceResponse* pResponse);
        virtual void WriteFile(const wchar_t* sURL, Coherent::UI::ResourceData* pResource);

    private:
    };
}