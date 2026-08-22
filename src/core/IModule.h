#ifndef CORE_I_MODULE_H_
#define CORE_I_MODULE_H_

#include "IContext.h"
#include "IEventProvider.h"
#include "Verdict.h"

namespace core {
    // IIS-independent mapping of CHttpModule, reduced to the
    // notifications the module actually handles.
    class IModule {
    public:
        virtual ~IModule() = default;

        virtual auto OnBeginRequest(
            IContext * context,
            IEventProvider * provider) -> Verdict = 0;
    };
}
#endif // CORE_I_MODULE_H_
