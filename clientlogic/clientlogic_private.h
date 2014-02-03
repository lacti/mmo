#include "stdafx.h"
#include "ClContext.h"
#include "entity_manager.h"

extern std::vector<ClContext*> l_ctx_array;
extern mmocl_context l_bound_ctx;

inline ClContext* ClCtx()
{
	if (l_ctx_array.size() > l_bound_ctx)
		return l_ctx_array[l_bound_ctx];
	else
		return nullptr;
}

inline client::entity_manager* ClCtxEm()
{
    return ClCtx() ? ClCtx()->em.get() : nullptr;
}

template <typename _MsgTy>
inline void ClSessionSend(const _MsgTy& msg)
{
    if (ClCtx() && ClCtx()->session)
        ClCtx()->session->write(msg);
}
