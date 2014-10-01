/* vim:set et sts=4: */
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <ibus.h>

#define IBUS_TYPE_RUSTPINYIN_ENGINE	\
	(ibus_rustpinyin_engine_get_type ())

GType   ibus_rustpinyin_engine_get_type    (void);

#endif
