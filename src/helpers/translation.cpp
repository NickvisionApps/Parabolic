#include "translation.hpp"

const char* pgettext_aux(const char* msg_ctxt_id, const char* msgid)
{
    const char* translation{ dcgettext(GETTEXT_PACKAGE, msg_ctxt_id, LC_MESSAGES) };
    if (translation == msg_ctxt_id)
    {
    	return msgid;
    }
    return translation;
}
