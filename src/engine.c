/* vim:set et sts=4: */

#include <enchant.h>
#include "engine.h"

typedef struct _IBusEnchantEngine IBusEnchantEngine;
typedef struct _IBusEnchantEngineClass IBusEnchantEngineClass;

struct _IBusEnchantEngine {
	IBusEngine parent;
};

struct _IBusEnchantEngineClass {
	IBusEngineClass parent;
};

/* functions prototype */
static void	ibus_enchant_engine_class_init	(IBusEnchantEngineClass	*klass);
static void	ibus_enchant_engine_init		(IBusEnchantEngine		*engine);
static void	ibus_enchant_engine_destroy		(IBusEnchantEngine		*engine);
static gboolean 
			ibus_enchant_engine_process_key_event
                                            (IBusEngine             *engine,
                                             guint               	 keyval,
                                             guint               	 modifiers);
static void ibus_enchant_engine_focus_in    (IBusEngine             *engine);
static void ibus_enchant_engine_focus_out   (IBusEngine             *engine);
static void ibus_enchant_engine_reset       (IBusEngine             *engine);
static void ibus_enchant_engine_enable      (IBusEngine             *engine);
static void ibus_enchant_engine_disable     (IBusEngine             *engine);
static void ibus_engine_set_cursor_location (IBusEngine             *engine,
                                             gint                    x,
                                             gint                    y,
                                             gint                    w,
                                             gint                    h);
static void ibus_enchant_engine_set_capabilities
                                            (IBusEngine             *engine,
                                             guint                   caps);
static void ibus_enchant_engine_page_up     (IBusEngine             *engine);
static void ibus_enchant_engine_page_down   (IBusEngine             *engine);
static void ibus_enchant_engine_cursor_up   (IBusEngine             *engine);
static void ibus_enchant_engine_cursor_down (IBusEngine             *engine);
static void ibus_enchant_property_activate  (IBusEngine             *engine,
                                             const gchar            *prop_name,
                                             gint                    prop_state);
static void ibus_enchant_engine_property_show
											(IBusEngine             *engine,
                                             const gchar            *prop_name);
static void ibus_enchant_engine_property_hide
											(IBusEngine             *engine,
                                             const gchar            *prop_name);

static IBusEngineClass *parent_class = NULL;
static EnchantBroker *broker = NULL;
static EnchantDict *dict = NULL;

GType
ibus_enchant_engine_get_type (void)
{
	static GType type = 0;

	static const GTypeInfo type_info = {
		sizeof (IBusEnchantEngineClass),
		(GBaseInitFunc)		NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc)	ibus_enchant_engine_class_init,
		NULL,
		NULL,
		sizeof (IBusEnchantEngine),
		0,
		(GInstanceInitFunc)	ibus_enchant_engine_init,
	};

	if (type == 0) {
		type = g_type_register_static (IBUS_TYPE_ENGINE,
									   "IBusEnchantEngine",
									   &type_info,
									   (GTypeFlags) 0);
	}

	return type;
}

static void
ibus_enchant_engine_class_init (IBusEnchantEngineClass *klass)
{
	IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

	parent_class = (IBusEngineClass *) g_type_class_peek_parent (klass);
	
	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_enchant_engine_destroy;

    engine_class->process_key_event = ibus_enchant_engine_process_key_event;
}

static void
ibus_enchant_engine_init (IBusEnchantEngine *engine)
{
    if (broker == NULL) {
        broker = enchant_broker_init ();
        dict = enchant_broker_request_dict (broker, "en");
    }
}

static void
ibus_enchant_engine_destroy (IBusEnchantEngine *engine)
{
	IBUS_OBJECT_CLASS (parent_class)->destroy ((IBusObject *)engine);
}

static gboolean 
ibus_enchant_engine_process_key_event (IBusEngine *engine,
                                       guint       keyval,
                                       guint       modifiers)
{
    g_debug ("0x%04x 0x%08x", keyval, modifiers);
    return FALSE;
}
