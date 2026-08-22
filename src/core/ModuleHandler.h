#ifndef CORE_MODULE_HANDLER_H_
#define CORE_MODULE_HANDLER_H_

#include "IModule.h"

namespace core {
    // The IIS-independent request-handling logic: rejects requests whose
    // URL path ends with "/forbidden" and stamps every other response
    // with the X-IISRequestLevelModule header. Stateless, so a single instance
    // can be shared across concurrent requests.
    class ModuleHandler : public IModule {
    public:
        auto OnBeginRequest(
            IContext * context,
            IEventProvider * provider) -> Verdict override;
    };
}
#endif // CORE_MODULE_HANDLER_H_
