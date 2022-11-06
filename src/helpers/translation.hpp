#pragma once

#include <libintl.h>
#include <locale.h>

#define _(String) gettext(String)

const char* pgettext_aux(const char* msg_ctxt_id, const char* msgid);

#define GETTEXT_CONTEXT_GLUE "\004"
#define pgettext(Ctxt, String) pgettext_aux(Ctxt GETTEXT_CONTEXT_GLUE String, String)