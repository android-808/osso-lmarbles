/**
   @file plugin.c

   Osso Lmarbles plugin functions.

    Copyright (c) 2004, 2005 Nokia Corporation.
	
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#include <stdio.h>
#include <stdlib.h>             /* for getenv */
#include <libintl.h>
#include <gtk/gtk.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <hildon/hildon-note.h>
#include <startup_plugin.h>
#include <hildon/hildon-caption.h>
#include <gdk/gdkkeysyms.h>
#include <hildon/hildon-banner.h>
#include <glib/gstdio.h>


#define _(String) dgettext("osso-games", String)
#define __(String) dgettext("osso-games", String)

#define OSSO_MARBLES_HELP_PATH "//marbles/a/1"

#define LMARBLES_ENABLE_SOUND           "/apps/osso/lmarbles/enable_sound"
#define LMARBLES_RUNNING           "/apps/osso/lmarbles/running"
#define LMARBLES_DIFFICULTY_LEVEL       "/apps/osso/lmarbles/difficulty_level"

#define LMARBLES_SETTINGS_EASY          0
#define LMARBLES_SETTINGS_NORMAL        1
#define LMARBLES_SETTINGS_HARD          2
#define LMARBLES_SETTINGS_BRAINSTORM    3
#define LMARBLES_SETTINGS_SOUND         4
#define LMARBLES_SETTINGS_RESET         5
#define MA_GAME_CHECKSTATE              16
#define MA_GAME_PLAYING_START 30
#define MA_GAME_PLAYING 31
#define COMBOBOX_SIZE 200


#define MA_GAME_ENDED 37
#define MA_GAME_RESET_OLD_SETTINGS 35
#define MA_GAME_RESTORE_LAST_SETTINGS 38

GConfClient *gcc = NULL;
GtkWidget *sound_check = NULL;
GtkWidget *difficult_box = NULL;

static void lmarbles_sound_cb(GtkWidget * widget, gpointer data);
static void lmarbles_difficulty_cb(GtkWidget * widget, gpointer data);

static GtkWidget *load_plugin(void);
static void unload_plugin(void);
static void write_config(void);
static GtkWidget **load_menu(guint *);
static void update_menu(void);
/*static void plugin_game_help_cb(GtkMenuItem * menuitem, gpointer user_data);*/

static void plugin_callback(GtkWidget * menu_item, gpointer data);
static void plugin_restore_original_settings(void);
static void plugin_restore_last_settings(void);
static gint plugin_settings_get_int_fallback(gchar * key, gint fall);
static GConfValue *plugin_settings_get(const gchar * key);
static void plugin_cb(GtkWidget * menu_item, gpointer cb_data);
static gboolean plugin_ui_show_reset_game_progress_dialog(void);
static gboolean plugin_dialog_key_press(GtkWidget * widget,
                                        GdkEventKey * event, gpointer data);
static gboolean plugin_settings_get_bool_fallback(gchar * key, gboolean fall);
static gboolean plugin_settings_get_bool(const gchar * key);
static void set_all_insensitive(void);
static void set_all_sensitive(void);
/*
static void plugin_show_infoprint(gchar * msg);
*/
gboolean plugin_sound_keypress_cb(GtkWidget * widget, GdkEventKey * event,
                                  gpointer user_data);
gboolean plugin_level_keypress_cb(GtkWidget * widget, GdkEventKey * event,
                                  gpointer user_data);

static StartupPluginInfo plugin_info = {
    load_plugin,
    unload_plugin,
    write_config,
    load_menu,
    update_menu,
    plugin_cb,
    NULL
};

static int changed = FALSE;
static int selected_difficulty = -1;

static GameStartupInfo gs;
static GtkWidget *menu_items[1];

STARTUP_INIT_PLUGIN(plugin_info, gs, FALSE, FALSE)

/*
static void plugin_show_infoprint(gchar * msg)
{

    hildon_banner_show_information(GTK_WIDGET(gs.ui->hildon_appview), NULL,
                                   msg);
}
*/




static void
plugin_checkstate() {
    gchar *gamestate_file = NULL;
    gamestate_file = g_build_filename(g_get_home_dir(), ".lmarbles_state", NULL);
    if (gamestate_file != NULL) {
        if ( !g_file_test(gamestate_file,G_FILE_TEST_IS_REGULAR) ) {
            hildon_banner_show_information(GTK_WIDGET(gs.ui->hildon_appview), NULL, dgettext("ke-recv","cerm_device_memory_full") );
        }
        g_free(gamestate_file);
    }


}


static gboolean
plugin_dialog_key_press(GtkWidget * widget, GdkEventKey * event,
                        gpointer data)
{
    (void) data;

    if (event->state & (GDK_CONTROL_MASK |
                        GDK_SHIFT_MASK |
                        GDK_MOD1_MASK |
                        GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK))
    {
        return FALSE;
    }

    switch (event->keyval)
    {
        case GDK_Escape:
            gtk_dialog_response(GTK_DIALOG(widget), FALSE);
            return TRUE;
            break;
        default:
            break;
    }

    return FALSE;
}


/* Get int type key value */
static gint
plugin_settings_get_int(const gchar * key)
{
    return gconf_client_get_int(gcc, key, NULL);
}


/* Get key value */
static GConfValue *
plugin_settings_get(const gchar * key)
{
    return gconf_client_get(gcc, key, NULL);
}

/* Get boolean type key value */
static gboolean
plugin_settings_get_bool(const gchar * key)
{
    return gconf_client_get_bool(gcc, key, NULL);
}


/**
 Helper function to get an bool entry, returning defined value, if not found.
 @param key GConf key to be get.
 @param fall Fallback to this, if not found, or invalid.
 @return Value got from GConf or the value specified in fall.
*/
static gboolean
plugin_settings_get_bool_fallback(gchar * key, gboolean fall)
{
    GConfValue *gc_val = plugin_settings_get(key);

    if (gc_val)
    {
        if (gc_val->type == GCONF_VALUE_BOOL)
        {
            gconf_value_free(gc_val);
            return plugin_settings_get_bool(key);
        }
        gconf_value_free(gc_val);
    }
    return fall;
}


static gint
plugin_settings_get_int_fallback(gchar * key, gint fall)
{
    GConfValue *gc_val = plugin_settings_get(key);
    if (gc_val)
    {
        if (gc_val->type == GCONF_VALUE_INT)
        {
            gconf_value_free(gc_val);
            return plugin_settings_get_int(key);
        }
        gconf_value_free(gc_val);
    }
    return fall;
}

gboolean
plugin_level_keypress_cb(GtkWidget * widget,
                         GdkEventKey * event, gpointer user_data)
{
    user_data = NULL;
    widget = NULL;
    if (event->keyval == GDK_Down)
    {
        gtk_widget_grab_focus(sound_check);
        return TRUE;
    }
    return FALSE;
}

gboolean
plugin_sound_keypress_cb(GtkWidget * widget,
                         GdkEventKey * event, gpointer user_data)
{
    widget = NULL;
    user_data = NULL;
    if (event->keyval == GDK_Up)
    {
        gtk_widget_grab_focus(difficult_box);
        return TRUE;
    }
    return FALSE;
}

static GtkWidget *
load_plugin(void)
{
    int sound, difficulty;
    /* GtkWidget *game_hbox; */
    GtkWidget *difficult_label;
    GtkWidget *sound_label;
    GtkAttachOptions xoptions = GTK_EXPAND, yoptions = GTK_EXPAND;
    GtkWidget *table = NULL;
    table = gtk_table_new(1, 2, FALSE);

    gcc = gconf_client_get_default();
    sound = plugin_settings_get_bool_fallback(LMARBLES_ENABLE_SOUND, TRUE);
    difficulty =
        plugin_settings_get_int_fallback(LMARBLES_DIFFICULTY_LEVEL, 1);
    gconf_client_set_bool(gcc, LMARBLES_RUNNING, TRUE, NULL);
    /* game_hbox = gtk_hbox_new (FALSE, 0); g_assert (game_hbox); */
    g_assert(table);

    // difficult_hbox = gtk_hbox_new (FALSE, 4);

    difficult_box = gtk_combo_box_new_text();
    g_assert(difficult_box);

    gtk_combo_box_append_text(GTK_COMBO_BOX(difficult_box),
                              __("game_va_lmarbles_settings_difficulty_1"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(difficult_box),
                              __("game_va_lmarbles_settings_difficulty_2"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(difficult_box),
                              __("game_va_lmarbles_settings_difficulty_3"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(difficult_box),
                              __("game_va_lmarbles_settings_difficulty_4"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(difficult_box), difficulty);


    difficult_label =
        hildon_caption_new(NULL, __("game_fi_lmarbles_settings_difficulty"),
                           difficult_box, NULL, HILDON_CAPTION_OPTIONAL);
    g_assert(difficult_label);
    gtk_widget_set_size_request(difficult_box, COMBOBOX_SIZE, -1);

    gtk_table_attach(GTK_TABLE(table), difficult_label, 0, 1, 0, 1,
                     (GtkAttachOptions) xoptions,
                     (GtkAttachOptions) yoptions, 0, 0);


    sound_check = gtk_check_button_new();
    g_assert(sound_check);
    sound_label =
        hildon_caption_new(NULL, __("game_fi_lmarbles_settings_sound"),
                           sound_check, NULL, HILDON_CAPTION_OPTIONAL);
    g_assert(sound_label);
    gtk_table_attach(GTK_TABLE(table), sound_label, 1, 2, 0, 1,
                     (GtkAttachOptions) xoptions,
                     (GtkAttachOptions) yoptions, 0, 0);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sound_check), sound);

    g_signal_connect(G_OBJECT(difficult_box), "changed",
                     G_CALLBACK(lmarbles_difficulty_cb), NULL);

    g_signal_connect(G_OBJECT(sound_check), "clicked",
                     G_CALLBACK(lmarbles_sound_cb), NULL);
    g_signal_connect(G_OBJECT(sound_check), "key-press-event",
                     G_CALLBACK(plugin_sound_keypress_cb), NULL);
    g_signal_connect(G_OBJECT(difficult_box), "key-press-event",
                     G_CALLBACK(plugin_level_keypress_cb), NULL);

    /* return game_hbox; */
    return table;
}

static gboolean
plugin_ui_show_reset_game_progress_dialog(void)
{
    gboolean answer = FALSE;
    HildonNote *note =
        HILDON_NOTE(hildon_note_new_confirmation
                    (GTK_WINDOW(gs.ui->hildon_appview),
                     __("game_nc_reset_game_progress")));
    gtk_widget_show_all(GTK_WIDGET(note));
    g_signal_connect(G_OBJECT(note), "key_press_event",
                     G_CALLBACK(plugin_dialog_key_press), NULL);
    answer = gtk_dialog_run(GTK_DIALOG(note));
    gtk_widget_destroy(GTK_WIDGET(note));
    return ( (GTK_RESPONSE_OK==answer) ? TRUE: FALSE);
}

static void
unload_plugin(void)
{
    /* restoring state file to beginning of level */
    FILE *dFile;
    char buf[1024];

    gchar *file = g_build_filename(g_get_home_dir(),
                                   ".lmarbles_state",
                                   NULL);
    dFile = g_fopen(file, "r");
    if (dFile)
    {
	    buf[fread(buf, 1, 1023, dFile)] = '\0';
	    fclose(dFile);
	    buf[2] = '\0';
	    dFile = g_fopen(file, "wb");
	    if (dFile)
	    {
		    fprintf(dFile, "%s", buf);
		    fclose(dFile);
	    }
    }

    gconf_client_set_bool(gcc, LMARBLES_RUNNING, FALSE, NULL);
}

static void
write_config(void)
{
    gconf_client_set_bool(gcc, LMARBLES_ENABLE_SOUND,
                          gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
                                                       (sound_check)), NULL);
    gconf_client_set_int(gcc, LMARBLES_DIFFICULTY_LEVEL,
                         gtk_combo_box_get_active(GTK_COMBO_BOX
                                                  (difficult_box)), NULL);
}
/* Rama - Bug#100799 */
#if 0
static void
plugin_game_help_cb(GtkMenuItem * menuitem, gpointer user_data)
{
    user_data = NULL;
    menuitem = NULL;
    StartupApp *app = NULL;
    app = gs.ui->app;
    hildon_help_show(app->osso, OSSO_MARBLES_HELP_PATH, 0);
}
#endif

GtkWidget *difficulty_rd[4], *sound_ck;

static GtkWidget **
load_menu(guint * nitems)
{
    GSList *group = NULL;
    GtkWidget *settings_menu = NULL, *reset_progress = NULL, *reset_menu =
        NULL, *reset_submenu = NULL;

    *nitems = 2;

    menu_items[0] =
        gtk_menu_item_new_with_label(__
                                     ("game_me_lmarbles_main_menu_settings"));
    settings_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_items[0]), settings_menu);

    difficulty_rd[0] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_lmarbles_menu_settings_easy"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(difficulty_rd[0]));
    gtk_menu_append(GTK_MENU(settings_menu), difficulty_rd[0]);
    g_signal_connect(G_OBJECT(difficulty_rd[0]), "toggled",
                     G_CALLBACK(plugin_callback),
                     (gpointer) LMARBLES_SETTINGS_EASY);

    difficulty_rd[1] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_lmarbles_menu_settings_normal"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(difficulty_rd[1]));
    gtk_menu_append(GTK_MENU(settings_menu), difficulty_rd[1]);
    g_signal_connect(G_OBJECT(difficulty_rd[1]), "toggled",
                     G_CALLBACK(plugin_callback),
                     (gpointer) LMARBLES_SETTINGS_NORMAL);

    difficulty_rd[2] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_lmarbles_menu_settings_hard"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(difficulty_rd[2]));
    gtk_menu_append(GTK_MENU(settings_menu), difficulty_rd[2]);
    g_signal_connect(G_OBJECT(difficulty_rd[2]), "toggled",
                     G_CALLBACK(plugin_callback),
                     (gpointer) LMARBLES_SETTINGS_HARD);

    difficulty_rd[3] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_lmarbles_menu_settings_brainstorm"));
    gtk_menu_append(GTK_MENU(settings_menu), difficulty_rd[3]);
    g_signal_connect(G_OBJECT(difficulty_rd[3]), "toggled",
                     G_CALLBACK(plugin_callback),
                     (gpointer) LMARBLES_SETTINGS_BRAINSTORM);

    gtk_menu_append(GTK_MENU(settings_menu), gtk_menu_item_new());

    sound_ck =
        gtk_check_menu_item_new_with_label(__
                                           ("game_me_lmarbles_menu_settings_sound"));
    gtk_menu_append(GTK_MENU(settings_menu), sound_ck);
    g_signal_connect(G_OBJECT(sound_ck), "toggled",
                     G_CALLBACK(plugin_callback),
                     (gpointer) LMARBLES_SETTINGS_SOUND);

    gtk_menu_append(GTK_MENU(settings_menu), gtk_menu_item_new());

    reset_submenu =
        gtk_menu_item_new_with_label(__
                                     ("game_me_lmarbles_menu_settings_reset"));
    reset_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(reset_submenu), reset_menu);
    reset_progress =
        gtk_menu_item_new_with_label(__
                                     ("game_me_lmarbles_menu_settings_reset_progress"));
    gtk_menu_append(GTK_MENU(settings_menu), reset_submenu);
    gtk_menu_append(GTK_MENU(reset_menu), reset_progress);
    g_signal_connect(G_OBJECT(reset_progress), "activate",
                     G_CALLBACK(plugin_callback),
                     (gpointer) LMARBLES_SETTINGS_RESET);

    /* Rama - Bug#100799  - removed help from main menu */



    return menu_items;
}

static void
set_all_insensitive(void)
{
    gtk_widget_set_sensitive(sound_check, FALSE);
    gtk_widget_set_sensitive(difficult_box, FALSE);
    if (GTK_WIDGET_VISIBLE(gs.ui->play_button))
    {
        gtk_widget_set_sensitive(gs.ui->play_button, FALSE);
    }
    if (GTK_WIDGET_VISIBLE(gs.ui->restart_button))
    {
        gtk_widget_set_sensitive(gs.ui->restart_button, FALSE);
    }

}

static void
set_all_sensitive(void)
{
    gtk_widget_set_sensitive(sound_check, TRUE);
    gtk_widget_set_sensitive(difficult_box, TRUE);
}

static void
plugin_restore_original_settings(void)
{
    gint original_settings =
        plugin_settings_get_int_fallback(LMARBLES_DIFFICULTY_LEVEL, 1);
    gint current_value =
        gtk_combo_box_get_active(GTK_COMBO_BOX(difficult_box));
    if (original_settings != current_value)
    {
        selected_difficulty = current_value;
        hildon_banner_show_information( NULL, NULL,
            _( "game_ib_changes_effect_next_game" ) );

        gtk_combo_box_set_active(GTK_COMBO_BOX(difficult_box),
                                 original_settings);
    }

}

static void plugin_restore_last_settings(void)
{
    gint original_settings =
        plugin_settings_get_int_fallback(LMARBLES_DIFFICULTY_LEVEL, 1);
    gint current_value =
        gtk_combo_box_get_active(GTK_COMBO_BOX(difficult_box));
    if (original_settings != current_value)
        selected_difficulty = current_value;

    if (-1 != selected_difficulty)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(difficult_box),
                                 selected_difficulty);
    }
}

static void
plugin_cb(GtkWidget * menu_item, gpointer cb_data)
{
    menu_item = NULL;
    switch ((int) cb_data)
    {
        case MA_GAME_RESET_OLD_SETTINGS:
        {
            plugin_restore_original_settings();
            break;
        }
        case MA_GAME_PLAYING_START:
        {
            set_all_insensitive();
            break;
        }
        case MA_GAME_PLAYING:
        {
            set_all_sensitive();
            break;
        }
        case MA_GAME_ENDED:
        case MA_GAME_RESTORE_LAST_SETTINGS:
        {
            plugin_restore_last_settings();
            break;
        }
        case MA_GAME_CHECKSTATE:
        {
            plugin_checkstate();
            break;
        }
    }
}

void
plugin_callback(GtkWidget * menu_item, gpointer data)
{
    menu_item = NULL;
    char filename[128];

    switch ((int) data)
    {
        case LMARBLES_SETTINGS_EASY:
        case LMARBLES_SETTINGS_NORMAL:
        case LMARBLES_SETTINGS_HARD:
        case LMARBLES_SETTINGS_BRAINSTORM:
            if (!changed)
            {
                changed = TRUE;
                gtk_combo_box_set_active(GTK_COMBO_BOX(difficult_box),
                                         (int) data - LMARBLES_SETTINGS_EASY);
                changed = FALSE;
            }
            break;

        case LMARBLES_SETTINGS_SOUND:
            if (!changed)
            {
                changed = TRUE;
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sound_check),
                                             gtk_check_menu_item_get_active
                                             (GTK_CHECK_MENU_ITEM(sound_ck)));
                changed = FALSE;
            }
            break;
        case LMARBLES_SETTINGS_RESET:
            if (plugin_ui_show_reset_game_progress_dialog() == TRUE)
            {
                sprintf(filename, "%s/.lmarbles_profile",
                        (char *) getenv("HOME"));
                remove(filename);
                sprintf(filename, "%s/.lmarbles_state",
                        (char *) getenv("HOME"));
                remove(filename);
                gs.startup_ui_state_change_cb(NULL, 0, 0, gs.ui);
            }
            break;
    }
}

static void
update_menu(void)
{
    lmarbles_sound_cb(sound_check, NULL);
    lmarbles_difficulty_cb(difficult_box, NULL);
}

/* Set menu item activity */
static void
lmarbles_difficulty_cb(GtkWidget * widget, gpointer data)
{
    data = NULL;
    int active;

    if (!changed)
    {
        changed = TRUE;
        active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
        if (active >= 0 && active < 4)
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
                                           (difficulty_rd[active]), TRUE);
        changed = FALSE;
    }
}

static void
lmarbles_sound_cb(GtkWidget * widget, gpointer data)
{
    data = NULL;
    if (!changed)
    {
        changed = TRUE;
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sound_ck),
                                       gtk_toggle_button_get_active
                                       (GTK_TOGGLE_BUTTON(widget)));
        changed = FALSE;
        gconf_client_set_bool(gcc, LMARBLES_ENABLE_SOUND,
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (sound_check)), NULL);
    }
}
