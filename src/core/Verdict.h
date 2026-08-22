#ifndef CORE_VERDICT_H_
#define CORE_VERDICT_H_

namespace core {
    // IIS-independent mapping of REQUEST_NOTIFICATION_STATUS. Only the
    // values the module actually returns are mapped for now:
    // Continue -> RQ_NOTIFICATION_CONTINUE
    // Finish   -> RQ_NOTIFICATION_FINISH_REQUEST
    enum class Verdict {
        Continue,
        Finish,
    };
}
#endif // CORE_VERDICT_H_
