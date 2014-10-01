/* vim:set et sts=4: */

#include <stdio.h>
#include "rustpinyin.h"
#include "engine.h"

typedef struct _IBusRustPinyinEngine IBusRustPinyinEngine;
typedef struct _IBusRustPinyinEngineClass IBusRustPinyinEngineClass;

struct _IBusRustPinyinEngine {
	IBusEngine parent;

    /* members */
    GString *preedit;
    gint cursor_pos;

    IBusLookupTable *table;
};

struct _IBusRustPinyinEngineClass {
	IBusEngineClass parent;
};

/* functions prototype */
static void	ibus_rustpinyin_engine_class_init	(IBusRustPinyinEngineClass	*klass);
static void	ibus_rustpinyin_engine_init		(IBusRustPinyinEngine		*engine);
static void	ibus_rustpinyin_engine_destroy		(IBusRustPinyinEngine		*engine);
static gboolean 
			ibus_rustpinyin_engine_process_key_event
                                            (IBusEngine             *engine,
                                             guint               	 keyval,
                                             guint               	 keycode,
                                             guint               	 modifiers);
static void ibus_rustpinyin_engine_focus_in    (IBusEngine             *engine);
static void ibus_rustpinyin_engine_focus_out   (IBusEngine             *engine);
static void ibus_rustpinyin_engine_reset       (IBusEngine             *engine);
static void ibus_rustpinyin_engine_enable      (IBusEngine             *engine);
static void ibus_rustpinyin_engine_disable     (IBusEngine             *engine);
static void ibus_engine_set_cursor_location (IBusEngine             *engine,
                                             gint                    x,
                                             gint                    y,
                                             gint                    w,
                                             gint                    h);
static void ibus_rustpinyin_engine_set_capabilities
                                            (IBusEngine             *engine,
                                             guint                   caps);
static void ibus_rustpinyin_engine_page_up     (IBusEngine             *engine);
static void ibus_rustpinyin_engine_page_down   (IBusEngine             *engine);
static void ibus_rustpinyin_engine_cursor_up   (IBusEngine             *engine);
static void ibus_rustpinyin_engine_cursor_down (IBusEngine             *engine);
static void ibus_rustpinyin_property_activate  (IBusEngine             *engine,
                                             const gchar            *prop_name,
                                             gint                    prop_state);
static void ibus_rustpinyin_engine_property_show
											(IBusEngine             *engine,
                                             const gchar            *prop_name);
static void ibus_rustpinyin_engine_property_hide
											(IBusEngine             *engine,
                                             const gchar            *prop_name);

static void ibus_rustpinyin_engine_commit_string
                                            (IBusRustPinyinEngine      *rustpinyin,
                                             const gchar            *string);
static void ibus_rustpinyin_engine_update      (IBusRustPinyinEngine      *rustpinyin);

static void *db = NULL;

G_DEFINE_TYPE (IBusRustPinyinEngine, ibus_rustpinyin_engine, IBUS_TYPE_ENGINE)

static void
ibus_rustpinyin_engine_class_init (IBusRustPinyinEngineClass *klass)
{
	IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);
	
	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_rustpinyin_engine_destroy;

    engine_class->process_key_event = ibus_rustpinyin_engine_process_key_event;
}

static void
ibus_rustpinyin_engine_init (IBusRustPinyinEngine *rustpinyin)
{
    if (db == NULL) {
        db =  db_new("plop.csv");
    }

    rustpinyin->preedit = g_string_new ("");
    rustpinyin->cursor_pos = 0;

    rustpinyin->table = ibus_lookup_table_new (9, 0, TRUE, TRUE);
    g_object_ref_sink (rustpinyin->table);
}

static void
ibus_rustpinyin_engine_destroy (IBusRustPinyinEngine *rustpinyin)
{
    if (rustpinyin->preedit) {
        g_string_free (rustpinyin->preedit, TRUE);
        rustpinyin->preedit = NULL;
    }

    if (rustpinyin->table) {
        g_object_unref (rustpinyin->table);
        rustpinyin->table = NULL;
    }

	((IBusObjectClass *) ibus_rustpinyin_engine_parent_class)->destroy ((IBusObject *)rustpinyin);
}

static void
ibus_rustpinyin_engine_update_lookup_table (IBusRustPinyinEngine *rustpinyin)
{
    gboolean retval;

    if (rustpinyin->preedit->len == 0) {
        ibus_engine_hide_lookup_table ((IBusEngine *) rustpinyin);
        return;
    }

    ibus_lookup_table_clear (rustpinyin->table);
    

    void* suggestions = pinyin2suggestions_c(
        db,
        rustpinyin->preedit->str
    );
    unsigned n_sug = suggestions_size(suggestions);
    //sugs = rustpinyin_dict_suggest (dict,
    //                             rustpinyin->preedit->str,
    //                             rustpinyin->preedit->len,
    //                             &n_sug);

    if (suggestions == NULL || n_sug == 0) {
        ibus_engine_hide_lookup_table ((IBusEngine *) rustpinyin);
        return;
    }

    for (unsigned i = 0; i < n_sug; i++) {
        const gchar* value = suggestions_value_get(
            suggestions,
            i
        );
        ibus_lookup_table_append_candidate (
            rustpinyin->table,
            ibus_text_new_from_string (value)
        );

        suggestions_value_free(value);
    }

    ibus_engine_update_lookup_table ((IBusEngine *) rustpinyin, rustpinyin->table, TRUE);

    if (suggestions != NULL) {
        suggestions_free(suggestions);
    }
}

static void
ibus_rustpinyin_engine_update_preedit (IBusRustPinyinEngine *rustpinyin)
{
    IBusText *text;
    gint retval;

    text = ibus_text_new_from_static_string (rustpinyin->preedit->str);
    text->attrs = ibus_attr_list_new ();
    
    ibus_attr_list_append (
        text->attrs,
        ibus_attr_underline_new (
            IBUS_ATTR_UNDERLINE_SINGLE,
            0,
            rustpinyin->preedit->len
        )
    );
    //TODO : need to check what this is doing
    //if (rustpinyin->preedit->len > 0) {
    //    retval = rustpinyin_dict_check (dict, rustpinyin->preedit->str, rustpinyin->preedit->len);
    //    if (retval != 0) {
    //        ibus_attr_list_append (text->attrs,
    //                           ibus_attr_foreground_new (0xff0000, 0, rustpinyin->preedit->len));
    //    }
    //}
    
    ibus_engine_update_preedit_text ((IBusEngine *)rustpinyin,
                                     text,
                                     rustpinyin->cursor_pos,
                                     TRUE);

}

/* commit preedit to client and update preedit */
static gboolean
ibus_rustpinyin_engine_commit_preedit (IBusRustPinyinEngine *rustpinyin)
{
    if (rustpinyin->preedit->len == 0)
        return FALSE;
    
    // TODO: quick hack, instead of commiting the string typed
    // by the user, if we have something in the lookup table
    // we commit instead the first candidate
    IBusText* candidate = ibus_lookup_table_get_candidate(
        rustpinyin->table,
        0
    );
    if (candidate != NULL) {
        ibus_engine_commit_text ((IBusEngine *)rustpinyin, candidate);
    } else {
        ibus_rustpinyin_engine_commit_string (rustpinyin, rustpinyin->preedit->str);
    }
    g_string_assign (rustpinyin->preedit, "");
    rustpinyin->cursor_pos = 0;

    ibus_rustpinyin_engine_update (rustpinyin);

    return TRUE;
}


static void
ibus_rustpinyin_engine_commit_string (IBusRustPinyinEngine *rustpinyin,
                                   const gchar       *string)
{
    IBusText *text;
    text = ibus_text_new_from_static_string (string);
    ibus_engine_commit_text ((IBusEngine *)rustpinyin, text);
}

static void
ibus_rustpinyin_engine_update (IBusRustPinyinEngine *rustpinyin)
{
    ibus_rustpinyin_engine_update_preedit (rustpinyin);
    ibus_engine_hide_lookup_table ((IBusEngine *)rustpinyin);
}

#define is_alpha(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))
#define is_tone(c) ((c) >= IBUS_1 && (c) <= IBUS_5)

static gboolean 
ibus_rustpinyin_engine_process_key_event (IBusEngine *engine,
                                       guint       keyval,
                                       guint       keycode,
                                       guint       modifiers)
{
    IBusText *text;
    IBusRustPinyinEngine *rustpinyin = (IBusRustPinyinEngine *)engine;

    if (modifiers & IBUS_RELEASE_MASK) {
        return FALSE;
    }

    modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

    if (modifiers != 0) {
        if (rustpinyin->preedit->len == 0) {
            return FALSE;
        } else {
            return TRUE;
        }
    }


    switch (keyval) {
    case IBUS_space:
        g_string_append (rustpinyin->preedit, " ");
        return ibus_rustpinyin_engine_commit_preedit (rustpinyin);
    case IBUS_Return:
        return ibus_rustpinyin_engine_commit_preedit (rustpinyin);

    case IBUS_Escape:
        if (rustpinyin->preedit->len == 0)
            return FALSE;

        g_string_assign (rustpinyin->preedit, "");
        rustpinyin->cursor_pos = 0;
        ibus_rustpinyin_engine_update (rustpinyin);
        return TRUE;        

    case IBUS_Left:
        if (rustpinyin->preedit->len == 0)
            return FALSE;
        if (rustpinyin->cursor_pos > 0) {
            rustpinyin->cursor_pos --;
            ibus_rustpinyin_engine_update (rustpinyin);
        }
        return TRUE;

    case IBUS_Right:
        if (rustpinyin->preedit->len == 0)
            return FALSE;
        if (rustpinyin->cursor_pos < rustpinyin->preedit->len) {
            rustpinyin->cursor_pos ++;
            ibus_rustpinyin_engine_update (rustpinyin);
        }
        return TRUE;
    
    case IBUS_Up:
        if (rustpinyin->preedit->len == 0)
            return FALSE;
        if (rustpinyin->cursor_pos != 0) {
            rustpinyin->cursor_pos = 0;
            ibus_rustpinyin_engine_update (rustpinyin);
        }
        return TRUE;

    case IBUS_Down:
        if (rustpinyin->preedit->len == 0)
            return FALSE;
        
        if (rustpinyin->cursor_pos != rustpinyin->preedit->len) {
            rustpinyin->cursor_pos = rustpinyin->preedit->len;
            ibus_rustpinyin_engine_update (rustpinyin);
        }
        
        return TRUE;
    
    case IBUS_BackSpace:
        if (rustpinyin->preedit->len == 0)
            return FALSE;
        if (rustpinyin->cursor_pos > 0) {
            rustpinyin->cursor_pos --;
            g_string_erase (rustpinyin->preedit, rustpinyin->cursor_pos, 1);
            ibus_rustpinyin_engine_update (rustpinyin);
            ibus_rustpinyin_engine_update_lookup_table (rustpinyin);
        }
        return TRUE;
    
    case IBUS_Delete:
        if (rustpinyin->preedit->len == 0)
            return FALSE;
        if (rustpinyin->cursor_pos < rustpinyin->preedit->len) {
            g_string_erase (rustpinyin->preedit, rustpinyin->cursor_pos, 1);
            ibus_rustpinyin_engine_update (rustpinyin);
        }
        return TRUE;
    }

    if (is_alpha (keyval) || is_tone(keyval)) {
        g_string_insert_c (rustpinyin->preedit,
                           rustpinyin->cursor_pos,
                           keyval);

        rustpinyin->cursor_pos ++;
        ibus_rustpinyin_engine_update (rustpinyin);
        ibus_rustpinyin_engine_update_lookup_table (rustpinyin);
        
        return TRUE;
    }

    return FALSE;
}
