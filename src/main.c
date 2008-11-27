/* vim:set et sts=4: */

#include <ibus.h>

#define IBUS_TYPE_ENCHANT_ENGINE	\
	(ibus_enchant_engine_get_type ())

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
                                            (IBusEnchantEngine      *engine,
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
static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

static GType
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
}

static void
ibus_enchant_engine_destroy (IBusEnchantEngine *engine)
{
	IBUS_OBJECT_CLASS (parent_class)->destroy ((IBusObject *)engine);
}

static gboolean 
ibus_enchant_engine_process_key_event (IBusEnchantEngine *engine,
                                       guint              keyval,
                                       guint              modifiers)
{
    return FALSE;
}

int main()
{
	ibus_init ();

	bus = ibus_bus_new ();
	factory = ibus_factory_new ("/org/freedesktop/IBus/enchant/Factory",
								ibus_bus_get_connection (bus),
								"English Writer",
								"en",
								"",
								"Peng Huang <shawn.p.huang@gmail.com>",
								"GPLv2",
								"/org/freedesktop/IBus/enchant/Engine",
								IBUS_TYPE_ENCHANT_ENGINE);
	ibus_main ();
}
