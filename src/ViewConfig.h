#include <IEntity.h>
#include <Coherent/UI/ViewInfo.h>

namespace CoherentUIPlugin
{

    struct ViewConfig
    {
        /// The view parameters
        ///
        Coherent::UI::ViewInfo ViewInfo;

        /// The initial url
        ///
        std::wstring Url;

        /// The entity to map the view on
        ///
        IEntity* Entity;

        /// The name of the material  to map the view on
        /// (if defined, it Entity will be ignored)
        ///
        std::string MaterialName;

        /// The name of the collision mesh object if used
        ///
        std::string CollisionMesh;
    };

}
