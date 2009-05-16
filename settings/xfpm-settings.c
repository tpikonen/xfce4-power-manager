/*
 * * Copyright (C) 2008-2009 Ali <aliov@xfce.org>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <glade/glade.h>

#include <xfconf/xfconf.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/libxfcegui4.h>

#include "libxfpm/xfpm-common.h"
#include "libxfpm/xfpm-string.h"

#include "xfpm-settings-glade.h"

#include "xfpm-settings.h"
#include "xfpm-config.h"

static 	GladeXML *xml 				= NULL;
#ifdef HAVE_DPMS
static  GtkWidget *on_battery_dpms_sleep 	= NULL;
static  GtkWidget *on_battery_dpms_off  	= NULL;
static  GtkWidget *on_ac_dpms_sleep 		= NULL;
static  GtkWidget *on_ac_dpms_off 		= NULL;
static  GtkWidget *sleep_dpms_mode 		= NULL;
static  GtkWidget *suspend_dpms_mode		= NULL;
#endif

/*
 * Callback settings 
 */
static void
battery_critical_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint value = 0;
    
    if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &selected_row))
	return;
	
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(w));
   
    gtk_tree_model_get(model,
                       &selected_row,
                       1,
                       &value,
                       -1);
		       
    if (!xfconf_channel_set_string (channel, CRITICAL_BATT_ACTION_CFG, xfpm_int_to_shutdown_string(value)) )
    {
	g_critical ("Cannot set value for property %s\n", CRITICAL_BATT_ACTION_CFG);
    }
}

static void
set_show_tray_icon_cb (GtkWidget *w, XfconfChannel *channel)
{
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint value = 0;
    
    if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &selected_row))
	return;
	
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(w));
   
    gtk_tree_model_get(model,
                       &selected_row,
                       1,
                       &value,
                       -1);
    
    if (!xfconf_channel_set_uint (channel, SHOW_TRAY_ICON_CFG, value) )
    {
	g_critical ("Cannot set value for property %s\n", SHOW_TRAY_ICON_CFG);
    }
}

static void
inactivity_on_ac_value_changed_cb (GtkWidget *widget, XfconfChannel *channel)
{
    gint value    = (gint)gtk_range_get_value (GTK_RANGE (widget));
    
    if (!xfconf_channel_set_uint (channel, ON_AC_INACTIVITY_TIMEOUT, value))
    {
	g_critical ("Cannot set value for property %s\n", ON_AC_INACTIVITY_TIMEOUT);
    }
}

static void
inactivity_on_battery_value_changed_cb (GtkWidget *widget, XfconfChannel *channel)
{
    gint value    = (gint)gtk_range_get_value (GTK_RANGE (widget));
    
    if (!xfconf_channel_set_uint (channel, ON_BATTERY_INACTIVITY_TIMEOUT, value))
    {
	g_critical ("Cannot set value for property %s\n", ON_BATTERY_INACTIVITY_TIMEOUT);
    }
}

static void
set_sleep_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint value = 0;
    
    if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &selected_row))
	return;
	
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(w));
   
    gtk_tree_model_get(model,
                       &selected_row,
                       1,
                       &value,
                       -1);
    
    if (!xfconf_channel_set_string (channel, SLEEP_SWITCH_CFG, xfpm_int_to_shutdown_string(value) ) )
    {
	g_critical ("Cannot set value for property %s\n", SLEEP_SWITCH_CFG);
    }
}

static void
set_power_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint value = 0;
    
    if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &selected_row))
	return;
	
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(w));
   
    gtk_tree_model_get(model,
                       &selected_row,
                       1,
                       &value,
                       -1);
    
    if (!xfconf_channel_set_string (channel, POWER_SWITCH_CFG, xfpm_int_to_shutdown_string(value) ) )
    {
	g_critical ("Cannot set value for property %s\n", POWER_SWITCH_CFG);
    }
}

static void
set_hibernate_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint value = 0;
    
    if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &selected_row))
	return;
	
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(w));
   
    gtk_tree_model_get(model,
                       &selected_row,
                       1,
                       &value,
                       -1);
    
    if (!xfconf_channel_set_string (channel, HIBERNATE_SWITCH_CFG, xfpm_int_to_shutdown_string(value) ) )
    {
	g_critical ("Cannot set value for property %s\n", HIBERNATE_SWITCH_CFG);
    }
}

static void
power_save_toggled_cb (GtkWidget *w, XfconfChannel *channel)
{
    gboolean val = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w));
    
    if (!xfconf_channel_set_bool (channel, POWER_SAVE_ON_BATTERY, val) )
    {
	g_critical ("Cannot set value for property %s\n", POWER_SAVE_ON_BATTERY);
    }
}

static void
notify_toggled_cb (GtkWidget *w, XfconfChannel *channel)
{
    gboolean val = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w));
    
    if (!xfconf_channel_set_bool (channel, GENERAL_NOTIFICATION_CFG, val) )
    {
	g_critical ("Cannot set value for property %s\n", GENERAL_NOTIFICATION_CFG);
    }
}

static void
set_hibernate_inactivity (GtkWidget *w, XfconfChannel *channel)
{
    if (!xfconf_channel_set_string (channel, INACTIVITY_SLEEP_MODE, "hibernate") )
    {
	g_critical ("Cannot set value hibernate for property %s", INACTIVITY_SLEEP_MODE);
    }
}

static void
set_suspend_inactivity (GtkWidget *w, XfconfChannel *channel)
{
    if (!xfconf_channel_set_string (channel, INACTIVITY_SLEEP_MODE, "suspend") )
    {
	g_critical ("Cannot set value suspend for property %s", INACTIVITY_SLEEP_MODE);
    }
}

#ifdef HAVE_DPMS
static void
set_dpms_sleep_mode (GtkWidget *w, XfconfChannel *channel)
{
    if (!xfconf_channel_set_string (channel, DPMS_SLEEP_MODE, "sleep") )
    {
	g_critical ("Cannot set value sleep for property %s\n", DPMS_SLEEP_MODE);
    }
}

static void
set_dpms_suspend_mode (GtkWidget *w, XfconfChannel *channel)
{
    if (!xfconf_channel_set_string (channel, DPMS_SLEEP_MODE, "suspend") )
    {
	g_critical ("Cannot set value sleep for property %s\n", DPMS_SLEEP_MODE);
    }
}

static void
dpms_toggled_cb (GtkWidget *w, XfconfChannel *channel)
{
    gboolean val = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w));
    
    xfconf_channel_set_bool (channel, DPMS_ENABLED_CFG, val);
    
    gtk_widget_set_sensitive (on_ac_dpms_off, val);
    gtk_widget_set_sensitive (on_ac_dpms_sleep, val);
    
    if ( GTK_IS_WIDGET (on_battery_dpms_off ) )
    {
	gtk_widget_set_sensitive (on_battery_dpms_off, val);
    	gtk_widget_set_sensitive (on_battery_dpms_sleep, val);
    }
}

static void
sleep_on_battery_value_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    gint off_value    = (gint)gtk_range_get_value (GTK_RANGE(on_battery_dpms_off));
    gint sleep_value  = (gint)gtk_range_get_value (GTK_RANGE(w));
    
    if ( off_value != 0 )
    {
	if ( sleep_value >= off_value )
	{
	    gtk_range_set_value (GTK_RANGE(on_battery_dpms_off), sleep_value + 1 );
	}
    }
    
    if (!xfconf_channel_set_uint (channel, ON_BATT_DPMS_SLEEP, sleep_value))
    {
	g_critical ("Cannot set value for property %s\n", ON_BATT_DPMS_SLEEP);
    }
}

static void
off_on_battery_value_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    gint off_value    = (gint)gtk_range_get_value (GTK_RANGE(w));
    gint sleep_value  = (gint)gtk_range_get_value (GTK_RANGE(on_battery_dpms_sleep));
    
    if ( sleep_value != 0 )
    {
	if ( off_value <= sleep_value )
	{
	    gtk_range_set_value (GTK_RANGE(on_battery_dpms_sleep), off_value -1 );
	}
    }
    
    if (!xfconf_channel_set_uint (channel, ON_BATT_DPMS_OFF, off_value))
    {
	g_critical ("Cannot set value for property %s\n", ON_BATT_DPMS_OFF);
    }
    
}

static void
sleep_on_ac_value_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    gint off_value    = (gint)gtk_range_get_value (GTK_RANGE(on_ac_dpms_off));
    gint sleep_value  = (gint)gtk_range_get_value (GTK_RANGE(w));
    
    if ( off_value > 60 || sleep_value > 60 )
    	return;
	
    if ( off_value != 0 )
    {
	if ( sleep_value >= off_value )
	{
	    gtk_range_set_value (GTK_RANGE(on_ac_dpms_off), sleep_value + 1 );
	}
    }

    if (!xfconf_channel_set_uint (channel, ON_AC_DPMS_SLEEP, sleep_value))
    {
	g_critical ("Cannot set value for property %s\n", ON_AC_DPMS_SLEEP);
    }
}

static void
off_on_ac_value_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    gint off_value    = (gint)gtk_range_get_value (GTK_RANGE(w));
    gint sleep_value  = (gint)gtk_range_get_value (GTK_RANGE(on_ac_dpms_sleep));
    
    if ( off_value > 60 || sleep_value > 60 )
    	return;
    
    if ( sleep_value != 0 )
    {
	if ( off_value <= sleep_value )
	{
	    gtk_range_set_value (GTK_RANGE(on_ac_dpms_sleep), off_value -1 );
	}
    }

    if (!xfconf_channel_set_uint (channel, ON_AC_DPMS_OFF, off_value))
    {
	g_critical ("Cannot set value for property %s\n", ON_AC_DPMS_OFF);
    }
}

/*
 * Format value of GtkRange used with DPMS
 */
static gchar *
format_dpms_value_cb (GtkScale *scale, gdouble value)
{
    if ( (gint)value == 0 )
    	return g_strdup (_("Never"));
    
    if ( (int)value == 1 )
	return g_strdup (_("One minute"));
	
    return g_strdup_printf ("%d %s", (int)value, _("Minutes"));
}
#endif /* HAVE_DPMS */

static gchar *
format_inactivity_value_cb (GtkScale *scale, gdouble value)
{
    gint h, min;
    
    if ( (gint)value <= 30 )
	return g_strdup (_("Never"));
    else if ( (gint)value < 60 )
	return g_strdup_printf ("%d %s", (gint)value, _("Minutes"));
    else if ( (gint)value == 60)
	return g_strdup (_("One hour"));
    else if ( (gint)value > 60 )
    {
	h = (gint)value/60;
	min = (gint)value%60;
	
	if ( h <= 1 )
	    if ( min == 0 )      return g_strdup_printf ("%s", _("One hour"));
	    else if ( min == 1 ) return g_strdup_printf ("%s %s", _("One hour"),  _("one minute"));
	    else                 return g_strdup_printf ("%s %d %s", _("One hour"), min, _("minutes"));
	else 
	    if ( min == 0 )      return g_strdup_printf ("%d %s", h, _("hours"));
	    else if ( min == 1 ) return g_strdup_printf ("%d %s %s", h, _("hours"), _("one minute"));
	    else            return g_strdup_printf ("%d %s %d %s", h, _("hours"), min, _("minutes"));
    }
	
    return g_strdup_printf ("%d %s", (int)value, _("Minutes"));
}

/*
 * Format value of GtkRange used with Brightness
 */
static gchar *
format_brightness_value_cb (GtkScale *scale, gdouble value)
{
    if ( (gint)value <= 9 )
    	return g_strdup (_("Never"));
        
    return g_strdup_printf ("%d %s", (int)value, _("Seconds"));
}

static void
brightness_on_battery_value_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    gint value    = (gint)gtk_range_get_value (GTK_RANGE(w));
    
    if (!xfconf_channel_set_uint (channel, BRIGHTNESS_ON_BATTERY, value))
    {
	g_critical ("Cannot set value for property %s\n", BRIGHTNESS_ON_BATTERY);
    }
}

static void
brightness_on_ac_value_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    gint value    = (gint)gtk_range_get_value (GTK_RANGE(w));
    
    if (!xfconf_channel_set_uint (channel, BRIGHTNESS_ON_AC, value))
    {
	g_critical ("Cannot set value for property %s\n", BRIGHTNESS_ON_AC);
    }
}

static gboolean
critical_spin_output_cb (GtkSpinButton *w, gpointer data)
{
    gint val = (gint) gtk_spin_button_get_value (w);
    gchar *text = g_strdup_printf ("%d %%", val);
    
    gtk_entry_set_text (GTK_ENTRY(w), text);
    g_free (text);
    
    return TRUE;
}

static void
on_battery_lid_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint value = 0;
    
    if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &selected_row))
	return;
	
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(w));
   
    gtk_tree_model_get(model,
                       &selected_row,
                       1,
                       &value,
                       -1);
    
    if (!xfconf_channel_set_string (channel, LID_SWITCH_ON_BATTERY_CFG, xfpm_int_to_shutdown_string(value)) )
    {
	g_critical ("Cannot set value for property %s\n", LID_SWITCH_ON_BATTERY_CFG);
    }
}

static void
on_ac_lid_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint value = 0;
    
    if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (w), &selected_row))
	return;
	
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(w));
   
    gtk_tree_model_get(model,
                       &selected_row,
                       1,
                       &value,
                       -1);
		       
    if (!xfconf_channel_set_string (channel, LID_SWITCH_ON_AC_CFG, xfpm_int_to_shutdown_string(value)) )
    {
	g_critical ("Cannot set value for property %s\n", LID_SWITCH_ON_AC_CFG);
    }
} 

static void
critical_level_value_changed_cb (GtkSpinButton *w, XfconfChannel *channel)
{
    guint val = (guint) gtk_spin_button_get_value (w);
    
    if (!xfconf_channel_set_uint (channel, CRITICAL_POWER_LEVEL, val) )
    {
	g_critical ("Unable to set value %d for property %s\n", val, CRITICAL_POWER_LEVEL);
    }
}

static void
lock_screen_toggled_cb (GtkWidget *w, XfconfChannel *channel)
{
    gboolean val = (gint) gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w));
    if ( !xfconf_channel_set_bool (channel, LOCK_SCREEN_ON_SLEEP, val) )
    {
	g_critical ("Unable to set value for property %s\n", LOCK_SCREEN_ON_SLEEP);
    }
}

#ifdef SYSTEM_IS_LINUX
static void
cpu_freq_control_changed_cb (GtkWidget *w, XfconfChannel *channel)
{
    gboolean val = (gint) gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w));
    
    if ( !xfconf_channel_set_bool (channel, CPU_FREQ_CONTROL, val) )
    {
	g_critical ("Unable to set value for property %s\n", CPU_FREQ_CONTROL);
    }
}
#endif

static void
xfpm_settings_on_battery (XfconfChannel *channel, gboolean user_privilege, gboolean can_suspend, 
			 gboolean can_hibernate, gboolean has_lcd_brightness, gboolean has_lid)
{
    gboolean valid;
    gint list_value;
    gint val;
    gboolean save_power;
    gchar *str;
    GtkListStore *list_store;
    GtkTreeIter iter;
    GtkWidget *power_save;
    GtkWidget *inact;
    GtkWidget *battery_critical;
    GtkWidget *lid;
    GtkWidget *label;
    GtkWidget *brg;
    GtkWidget *frame;
#ifdef HAVE_DPMS
    GtkWidget *dpms_frame_on_battery;
#endif

    battery_critical = glade_xml_get_widget (xml, "battery-critical-combox");
    
    inact = glade_xml_get_widget (xml, "inactivity-on-battery");
    
    if ( !can_suspend && !can_hibernate )
    {
	gtk_widget_set_sensitive (inact, FALSE);
	gtk_widget_set_tooltip_text (inact, _("Hibernate and suspend operations not permitted"));
    }
    
    val = xfconf_channel_get_uint (channel, ON_BATTERY_INACTIVITY_TIMEOUT, 30);
    gtk_range_set_value (GTK_RANGE (inact), val);
    g_signal_connect (inact, "value-changed",
		      G_CALLBACK (inactivity_on_battery_value_changed_cb), channel);
    g_signal_connect (inact, "format-value",
		      G_CALLBACK (format_inactivity_value_cb), NULL);
    
    if (!user_privilege )
    {
	gtk_widget_set_sensitive (battery_critical, FALSE);
	gtk_widget_set_tooltip_text (battery_critical, _("Shutdown and hibernate operations not permitted"));
    }
    
    list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
    
    gtk_combo_box_set_model (GTK_COMBO_BOX(battery_critical), GTK_TREE_MODEL(list_store));
    
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, 0, _("Nothing"), 1, 0, -1);
    
    if ( can_hibernate )
    {
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Hibernate"), 1, 2, -1);
    }
    
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, 0, _("Shutdown"), 1, 3, -1);
    
    str = xfconf_channel_get_string (channel, CRITICAL_BATT_ACTION_CFG, "Nothing");
    
    val = xfpm_shutdown_string_to_int (str);
    if ( G_UNLIKELY (val == -1 || val == 1) ) /* we don't do suspend */
    {
	g_warning ("Invalid value %s for property %s\n", str, CRITICAL_BATT_ACTION_CFG);
	val = 0;
    }
    
    for ( valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter);
	  valid;
	  valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store), &iter) )
    {
	gtk_tree_model_get (GTK_TREE_MODEL (list_store), &iter,
			    1, &list_value, -1);
	if ( val == list_value )
	{
	    gtk_combo_box_set_active_iter (GTK_COMBO_BOX (battery_critical), &iter);
	    break;
	}
    }
    
    g_signal_connect (battery_critical, "changed", 
		      G_CALLBACK(battery_critical_changed_cb), channel);
    
    g_free(str);
    
    power_save = glade_xml_get_widget (xml, "power-save");
    save_power = xfconf_channel_get_bool (channel, POWER_SAVE_ON_BATTERY, TRUE);
    
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(power_save), save_power);
    g_signal_connect (power_save, "toggled",
		      G_CALLBACK(power_save_toggled_cb), channel);
	
    
    /*
     * DPMS settings when running on battery power
     */
#ifdef HAVE_DPMS
    dpms_frame_on_battery = glade_xml_get_widget (xml, "dpms-on-battery-frame");
    gtk_widget_show (GTK_WIDGET(dpms_frame_on_battery));
    
    on_battery_dpms_sleep = glade_xml_get_widget (xml, "sleep-dpms-on-battery");
  
    val = xfconf_channel_get_uint (channel, ON_BATT_DPMS_SLEEP, 3);
    gtk_range_set_value (GTK_RANGE(on_battery_dpms_sleep), val);
    
    g_signal_connect (on_battery_dpms_sleep, "value-changed",
		      G_CALLBACK(sleep_on_battery_value_changed_cb), channel);
		      
    g_signal_connect (on_battery_dpms_sleep, "format-value", 
		      G_CALLBACK(format_dpms_value_cb), NULL);
		      
    on_battery_dpms_off = glade_xml_get_widget (xml, "off-dpms-on-battery");
    
    val = xfconf_channel_get_uint (channel, ON_BATT_DPMS_OFF, 5);
    gtk_range_set_value (GTK_RANGE(on_battery_dpms_off), val);
    
    g_signal_connect (on_battery_dpms_off, "value-changed",
		      G_CALLBACK(off_on_battery_value_changed_cb), channel);
    g_signal_connect (on_battery_dpms_off, "format-value", 
		      G_CALLBACK(format_dpms_value_cb), NULL);
#endif

     /*
     * Lid switch settings on battery
     */
    lid = glade_xml_get_widget (xml, "on-battery-lid");
    if ( has_lid )
    {
	if (!user_privilege )
	{
	    gtk_widget_set_sensitive (lid, FALSE);
	    gtk_widget_set_tooltip_text (lid, _("Shutdown and hibernate operations not permitted"));
	}
	
	list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
	
	gtk_combo_box_set_model (GTK_COMBO_BOX(lid), GTK_TREE_MODEL(list_store));
	
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Nothing"), 1, 0, -1);
	
	if ( can_suspend )
	{
	    gtk_list_store_append(list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Suspend"), 1, 1, -1);
	}
	
	if ( can_hibernate)
	{
	    gtk_list_store_append(list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Hibernate"), 1, 2, -1);
	}
	
	g_signal_connect (lid, "changed", 
			  G_CALLBACK(on_battery_lid_changed_cb), channel);
			  
	str = xfconf_channel_get_string (channel, LID_SWITCH_ON_BATTERY_CFG, "Nothing");
	
	val = xfpm_shutdown_string_to_int (str);
	
	if ( G_UNLIKELY ( val == -1 || val == 3 ) )/*we don't do shutdown here */
	{
	    g_warning ("Invalid value %s for property %s\n", str, LID_SWITCH_ON_BATTERY_CFG);
	    val = 0;
	}
	
	for ( valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter);
	      valid;
	      valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store), &iter) )
	{
	    gtk_tree_model_get (GTK_TREE_MODEL (list_store), &iter,
				1, &list_value, -1);
	    if ( val == list_value )
	    {
		gtk_combo_box_set_active_iter (GTK_COMBO_BOX (lid), &iter);
		break;
	    }
	}
	g_free (str);
    }
    else
    {
	label = glade_xml_get_widget (xml, "on-battery-lid-label");
	gtk_widget_hide (label);
	gtk_widget_hide (lid);
    }
    
    /*
     * 
     * Brightness on battery power
     */
    brg = glade_xml_get_widget (xml ,"brg-on-battery");
    if ( has_lcd_brightness )
    {
	val = xfconf_channel_get_uint (channel, BRIGHTNESS_ON_BATTERY, 10);
	
	gtk_range_set_value (GTK_RANGE(brg), val);
	
	g_signal_connect (brg, "value-changed",
			  G_CALLBACK(brightness_on_battery_value_changed_cb), channel);
			  
	g_signal_connect (brg, "format-value", 
			  G_CALLBACK(format_brightness_value_cb), NULL);
    }
    else
    {
	frame = glade_xml_get_widget (xml, "on-battery-brg-frame");
	gtk_widget_hide_all (frame);
    }
}

static void
xfpm_settings_on_ac (XfconfChannel *channel, gboolean user_privilege, gboolean can_suspend, 
		     gboolean can_hibernate, gboolean has_lcd_brightness, gboolean has_lid)
{
    GtkWidget *inact;
    GtkWidget *lid;
    GtkWidget *frame;
    GtkWidget *brg;
    GtkListStore *list_store;
    GtkTreeIter iter;
    gchar *str;
    gint val;
    gboolean valid;
    gint list_value;
    
#ifdef HAVE_DPMS
    GtkWidget *dpms_frame_on_ac;
#endif

    inact = glade_xml_get_widget (xml, "inactivity-on-ac");
    
    if ( !can_suspend && !can_hibernate )
    {
	gtk_widget_set_sensitive (inact, FALSE);
	gtk_widget_set_tooltip_text (inact, _("Hibernate and suspend operations not permitted"));
    }
    
    val = xfconf_channel_get_uint (channel, ON_AC_INACTIVITY_TIMEOUT, 30);
    gtk_range_set_value (GTK_RANGE (inact), val);
    g_signal_connect (inact, "value-changed",
		      G_CALLBACK (inactivity_on_ac_value_changed_cb), channel);
    g_signal_connect (inact, "format-value",
		      G_CALLBACK (format_inactivity_value_cb), NULL);
   
#ifdef HAVE_DPMS
    /*
     * DPMS settings when running on AC power 
     */
    dpms_frame_on_ac = glade_xml_get_widget (xml, "dpms-on-ac-frame");
    gtk_widget_show (GTK_WIDGET(dpms_frame_on_ac));
    
    on_ac_dpms_sleep = glade_xml_get_widget (xml, "sleep-dpms-on-ac");
  
    val = xfconf_channel_get_uint (channel, ON_AC_DPMS_SLEEP, 10);
    gtk_range_set_value (GTK_RANGE (on_ac_dpms_sleep), val);
    
    g_signal_connect (on_ac_dpms_sleep, "value-changed",
		      G_CALLBACK(sleep_on_ac_value_changed_cb), channel);
		      
    g_signal_connect (on_ac_dpms_sleep, "format-value", 
		      G_CALLBACK(format_dpms_value_cb), NULL);
		      
    on_ac_dpms_off = glade_xml_get_widget (xml, "off-dpms-on-ac");
    
    val = xfconf_channel_get_uint (channel, ON_AC_DPMS_OFF, 15);
    gtk_range_set_value (GTK_RANGE(on_ac_dpms_off), val);
    
    g_signal_connect (on_ac_dpms_off, "value-changed",
		      G_CALLBACK(off_on_ac_value_changed_cb), channel);
    g_signal_connect (on_ac_dpms_off, "format-value", 
		      G_CALLBACK(format_dpms_value_cb), NULL);
#endif
    /*
     * Lid switch settings on AC power
     */
    lid = glade_xml_get_widget (xml, "on-ac-lid");
    if ( has_lid )
    {
	list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
	
	if ( !user_privilege )
	{
	    gtk_widget_set_sensitive (lid, FALSE);
	    gtk_widget_set_tooltip_text (lid, _("Hibernate and suspend operations not permitted"));
	    
	}
	gtk_combo_box_set_model (GTK_COMBO_BOX(lid), GTK_TREE_MODEL(list_store));
	
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Nothing"), 1, 0, -1);
	
	if ( can_suspend )
	{
	    gtk_list_store_append(list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Suspend"), 1, 1, -1);
	}
	
	if ( can_hibernate )
	{
	    gtk_list_store_append(list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Hibernate"), 1, 2, -1);
	}
	
	g_signal_connect (lid, "changed", 
			  G_CALLBACK(on_ac_lid_changed_cb), channel);
			  
	str = xfconf_channel_get_string (channel, LID_SWITCH_ON_AC_CFG, "Nothing");
	
	val = xfpm_shutdown_string_to_int (str);
	
	if ( G_UNLIKELY (val == -1 || val == 3) ) /*we don't do shutdown here */
	{
	    g_warning ("Invalid value %s for property %s\n", str, LID_SWITCH_ON_AC_CFG);
	    val = 0;
	}
	for ( valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter);
	      valid;
	      valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store), &iter) )
	{
	    gtk_tree_model_get (GTK_TREE_MODEL (list_store), &iter,
				1, &list_value, -1);
	    if ( val == list_value )
	    {
		gtk_combo_box_set_active_iter (GTK_COMBO_BOX (lid), &iter);
		break;
	    }
	}
	    
	g_free (str);
    }
    else
    {
	frame = glade_xml_get_widget (xml, "on-ac-actions-frame");
	gtk_widget_hide_all (frame);
    }
    
    /*
     * 
     * Brightness on AC power
     */
    brg = glade_xml_get_widget (xml ,"brg-on-ac");
    if ( has_lcd_brightness )
    {
	val = xfconf_channel_get_uint (channel, BRIGHTNESS_ON_AC, 9);
	
	gtk_range_set_value (GTK_RANGE(brg), val);
	
	g_signal_connect (brg, "value-changed",
			  G_CALLBACK(brightness_on_ac_value_changed_cb), channel);
			  
	g_signal_connect (brg, "format-value", 
			  G_CALLBACK(format_brightness_value_cb), NULL);
    }
    else
    {
	frame = glade_xml_get_widget (xml, "on-ac-brg-frame");
	gtk_widget_hide_all (frame);
    }
}

static void
xfpm_settings_general (XfconfChannel *channel, gboolean user_privilege,
		       gboolean can_suspend, gboolean can_hibernate,
		       gboolean has_sleep_button, gboolean has_hibernate_button,
		       gboolean has_power_button)
{
    GtkWidget *tray;
    GtkWidget *power;
    GtkWidget *power_label;
    GtkWidget *hibernate;
    GtkWidget *hibernate_label;
    GtkWidget *sleep_w;
    GtkWidget *sleep_label;
    GtkWidget *notify;
    
    gchar *default_sleep_value;
    gint   sleep_val_int;
    gchar *default_hibernate_value;
    gint   hibernate_val_int;
    gboolean valid;
    gchar *default_power_value;
    gint   power_val_int;
    gint list_value;
    guint show_tray;
    gboolean val;
    
#ifdef HAVE_DPMS
    GtkWidget *dpms;
#endif
    /*
     *  Tray icon settings
     */
    GtkListStore *list_store;
    GtkTreeIter iter;
    
    list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
    
    tray = glade_xml_get_widget (xml, "tray-combox");
    gtk_combo_box_set_model (GTK_COMBO_BOX(tray), GTK_TREE_MODEL(list_store));

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, 0, _("Always show icon"), 1, 0, -1);

    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, 0, _("When battery is present"), 1, 1, -1);
    
    gtk_list_store_append(list_store, &iter);
    gtk_list_store_set (list_store, &iter, 0, _("When battery is charging or discharging"), 1, 2, -1);
    
    show_tray = xfconf_channel_get_uint (channel, SHOW_TRAY_ICON_CFG, 1);
    gtk_combo_box_set_active (GTK_COMBO_BOX(tray), show_tray);
    g_signal_connect (tray, "changed",
		      G_CALLBACK(set_show_tray_icon_cb), channel);
#ifdef HAVE_DPMS
    /*
     * Global dpms settings (enable/disable)
     */
    dpms = glade_xml_get_widget (xml, "enable-dpms");
   
    g_signal_connect (dpms, "toggled",
		      G_CALLBACK(dpms_toggled_cb), channel);
		      
    val = xfconf_channel_get_bool (channel, DPMS_ENABLED_CFG, TRUE);
    
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(dpms), val);
    gtk_widget_set_tooltip_text (dpms, _("Disable Display Power Management Signaling (DPMS), "\
				         "e.g don't attempt to switch off the display or put it in sleep mode."));
#endif
    /*
     * Power button
     */
    list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
    power = glade_xml_get_widget (xml, "power-combox");
    power_label = glade_xml_get_widget (xml, "power-label");
    
    if ( has_power_button )
    {
	if (!user_privilege )
	{
	    gtk_widget_set_sensitive (power, FALSE);
	    gtk_widget_set_tooltip_text (power, _("Hibernate and suspend operations not permitted"));
	}
	
	gtk_combo_box_set_model (GTK_COMBO_BOX(power), GTK_TREE_MODEL(list_store));

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Nothing"), 1, 0, -1);
	
	if ( can_suspend )
	{
	    gtk_list_store_append (list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Suspend"), 1, 1, -1);
	}
	
	if ( can_hibernate )
	{
	    gtk_list_store_append (list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Hibernate"), 1, 2, -1);
	}
	
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Shutdown"), 1, 3, -1);
	
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Ask"), 1, 4, -1);
	
	g_signal_connect (power, "changed",
			  G_CALLBACK(set_power_changed_cb), channel);
	
	default_power_value = xfconf_channel_get_string (channel, POWER_SWITCH_CFG, "Nothing");
	power_val_int = xfpm_shutdown_string_to_int (default_power_value );
	
	if ( G_UNLIKELY (power_val_int == -1) ) 
	{
	    g_warning ("Invalid value %s for property %s\n", default_power_value, POWER_SWITCH_CFG);
	    power_val_int = 0;
	}
	for ( valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter);
		  valid;
		  valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store), &iter) )
	{
	    gtk_tree_model_get (GTK_TREE_MODEL (list_store), &iter,
				    1, &list_value, -1);
	    if ( power_val_int == list_value )
	    {
		gtk_combo_box_set_active_iter (GTK_COMBO_BOX (power), &iter);
		break;
	    }
	}

	g_free (default_power_value);
    }
    else
    {
	gtk_widget_hide (power);
	gtk_widget_hide (power_label);
    }
    
    /*
     * Hibernate button
     */
    list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
    hibernate = glade_xml_get_widget (xml, "hibernate-combox");
    hibernate_label = glade_xml_get_widget (xml, "hibernate-label");
    if (has_hibernate_button )
    {
	if (!user_privilege )
	{
	    gtk_widget_set_sensitive (hibernate, FALSE);
	    gtk_widget_set_tooltip_text (hibernate, _("Hibernate and suspend operations not permitted"));
	}
	
	gtk_combo_box_set_model (GTK_COMBO_BOX(hibernate), GTK_TREE_MODEL(list_store));

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Nothing"), 1, 0, -1);
	
	if ( can_suspend )
	{
	    gtk_list_store_append (list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Suspend"), 1, 1, -1);
	}
	
	if ( can_hibernate )
	{
	    gtk_list_store_append (list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Hibernate"), 1, 2, -1);
	}
	
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Ask"), 1, 4, -1);
	
	g_signal_connect (hibernate, "changed",
			  G_CALLBACK(set_hibernate_changed_cb), channel);
	
	default_hibernate_value = xfconf_channel_get_string (channel, HIBERNATE_SWITCH_CFG, "Nothing");
	hibernate_val_int = xfpm_shutdown_string_to_int (default_hibernate_value );
	
	if ( G_UNLIKELY (hibernate_val_int == -1) ) 
	{
	    g_warning ("Invalid value %s for property %s\n", default_hibernate_value, POWER_SWITCH_CFG);
	    hibernate_val_int = 0;
	}
	for ( valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter);
		  valid;
		  valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store), &iter) )
	{
	    gtk_tree_model_get (GTK_TREE_MODEL (list_store), &iter,
				    1, &list_value, -1);
	    if ( hibernate_val_int == list_value )
	    {
		gtk_combo_box_set_active_iter (GTK_COMBO_BOX (hibernate), &iter);
		break;
	    }
	}

	g_free (default_hibernate_value);
    }
    else
    {
	gtk_widget_hide ( hibernate);
	gtk_widget_hide (hibernate_label);
    }

    /*
     * Sleep button 
     */
    list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
    sleep_w = glade_xml_get_widget (xml, "sleep-combox");
    sleep_label = glade_xml_get_widget (xml, "sleep-label");
    if ( has_sleep_button )
    {
	if (!user_privilege )
	{
	    gtk_widget_set_sensitive (sleep_w, FALSE);
	    gtk_widget_set_tooltip_text (sleep_w, _("Hibernate and suspend operations not permitted"));
	}
	
	gtk_combo_box_set_model (GTK_COMBO_BOX(sleep_w), GTK_TREE_MODEL(list_store));

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Nothing"), 1, 0, -1);
	
	if ( can_suspend )
	{
	    gtk_list_store_append (list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Suspend"), 1, 1, -1);
	}
	
	if ( can_hibernate )
	{
	    gtk_list_store_append (list_store, &iter);
	    gtk_list_store_set (list_store, &iter, 0, _("Hibernate"), 1, 2, -1);
	}
	
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, _("Ask"), 1, 4, -1);
	
	g_signal_connect (sleep_w, "changed",
			  G_CALLBACK(set_sleep_changed_cb), channel);
	
	default_sleep_value = xfconf_channel_get_string (channel, SLEEP_SWITCH_CFG, "Nothing");
	sleep_val_int = xfpm_shutdown_string_to_int (default_sleep_value );
	if ( G_UNLIKELY (sleep_val_int == -1 || sleep_val_int == 3) ) 
	{
	    g_warning ("Invalid value %s for property %s\n", default_sleep_value, SLEEP_SWITCH_CFG);
	    sleep_val_int = 0;
	}
	for ( valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter);
		  valid;
		  valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store), &iter) )
	    {
		gtk_tree_model_get (GTK_TREE_MODEL (list_store), &iter,
				    1, &list_value, -1);
		if ( sleep_val_int == list_value )
		{
		    gtk_combo_box_set_active_iter (GTK_COMBO_BOX (sleep_w), &iter);
		    break;
		}
	    }
	
	g_free (default_sleep_value);
    }
    else
    {
	gtk_widget_hide (sleep_w);
	gtk_widget_hide (sleep_label);
    }
    /*
     * Enable/Disable Notification
     */
    
    notify = glade_xml_get_widget (xml, "notification");
    val = xfconf_channel_get_bool (channel, GENERAL_NOTIFICATION_CFG, TRUE);
    
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(notify), val);
    
    g_signal_connect (notify, "toggled",
		      G_CALLBACK(notify_toggled_cb), channel);
}

static void
xfpm_settings_advanced (XfconfChannel *channel, gboolean system_laptop, gboolean user_privilege,
			gboolean can_suspend, gboolean can_hibernate)
{
    guint val;
    gchar *str;
    GtkWidget *critical_level;
    GtkWidget *lock;
    GtkWidget *cpu;
    GtkWidget *label;
    
    GtkWidget *inact_suspend = glade_xml_get_widget (xml, "inactivity-suspend");
    GtkWidget *inact_hibernate = glade_xml_get_widget (xml, "inactivity-hibernate");
    
    if ( !can_suspend )
    {
	gtk_widget_set_sensitive (inact_suspend, FALSE);
	gtk_widget_set_tooltip_text (inact_suspend, _("Suspend operation not permitted"));
    }
    
    if ( !can_hibernate )
    {
	gtk_widget_set_sensitive (inact_hibernate, FALSE);
	gtk_widget_set_tooltip_text (inact_hibernate, _("Hibernate operation not permitted"));
    }
   
    g_signal_connect (inact_suspend, "toggled",
		      G_CALLBACK (set_suspend_inactivity), channel);
		      
    g_signal_connect (inact_hibernate, "toggled",
		      G_CALLBACK (set_hibernate_inactivity), channel);
		      
    str = xfconf_channel_get_string (channel, INACTIVITY_SLEEP_MODE, "suspend");
    if ( xfpm_strequal (str, "suspend") )
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (inact_suspend), TRUE);
    else if ( xfpm_strequal (str, "hibernate"))
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (inact_hibernate), TRUE);
    else 
    {
	g_warning ("Invalid value %s for property %s ", str, INACTIVITY_SLEEP_MODE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (inact_suspend), TRUE);
    }
   
    g_free (str);
    
#ifdef HAVE_DPMS
    sleep_dpms_mode = glade_xml_get_widget (xml, "sleep-dpms-mode");
    suspend_dpms_mode = glade_xml_get_widget (xml, "suspend-dpms-mode");
    
    g_signal_connect (sleep_dpms_mode, "toggled",
		      G_CALLBACK(set_dpms_sleep_mode), channel);
    g_signal_connect (suspend_dpms_mode, "toggled",
		      G_CALLBACK(set_dpms_suspend_mode), channel);
		      
    str = xfconf_channel_get_string (channel, DPMS_SLEEP_MODE, "sleep");
    
    if ( xfpm_strequal (str, "sleep" ) )
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(sleep_dpms_mode), TRUE);
    else if ( xfpm_strequal (str, "suspend") )
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(suspend_dpms_mode), TRUE);
    else 
    {
	g_critical ("Invalid value %s for property %s\n", str, DPMS_SLEEP_MODE );
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(sleep_dpms_mode), TRUE);
    }
    
    g_free (str);
    
#endif /* HAVE_DPMS */

    /*
     * Critical battery level
     */
    critical_level = glade_xml_get_widget (xml, "critical-spin");
    if ( system_laptop )
    {
	gtk_widget_set_tooltip_text (critical_level, 
				     _("When all the power sources of the computer reach this charge level"));
    
	g_signal_connect (critical_level, "output", 
			  G_CALLBACK(critical_spin_output_cb), NULL);
	g_signal_connect (critical_level, "value-changed", 
			  G_CALLBACK(critical_level_value_changed_cb), channel);
		      
	val = xfconf_channel_get_uint (channel, CRITICAL_POWER_LEVEL, 10 );

	if ( val > 20 )
	{
	    g_critical ("Value %d if out of range for property %s\n", val, CRITICAL_POWER_LEVEL);
	    gtk_spin_button_set_value (GTK_SPIN_BUTTON(critical_level), 10);
	}
	else
	    gtk_spin_button_set_value (GTK_SPIN_BUTTON(critical_level), val);
    }
    else
    {
	label = glade_xml_get_widget (xml, "critical-level-label" );
	gtk_widget_hide (critical_level);
	gtk_widget_hide (label);
    }
	
    /*
     * Lock screen for suspend/hibernate
     */
    lock = glade_xml_get_widget (xml, "lock-screen");
    
    if ( !user_privilege )
    {
	gtk_widget_set_sensitive (lock, FALSE);
	gtk_widget_set_tooltip_text (lock, _("Hibernate and suspend operations not permitted"));
    }
    
    val = xfconf_channel_get_bool (channel, LOCK_SCREEN_ON_SLEEP, TRUE);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(lock), val);
    g_signal_connect (lock, "toggled",
		      G_CALLBACK(lock_screen_toggled_cb), channel);
 
    cpu = glade_xml_get_widget (xml, "cpu-freq");
    
#ifdef SYSTEM_IS_LINUX
    if ( system_laptop )
    {
	val = xfconf_channel_get_bool (channel, CPU_FREQ_CONTROL, TRUE);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(cpu), val);
	g_signal_connect (cpu, "toggled",
		          G_CALLBACK(cpu_freq_control_changed_cb), channel);
    }
    else
	gtk_widget_hide (cpu);
#else
    gtk_widget_hide (cpu);
#endif

}

static void
_cursor_changed_cb(GtkTreeView *view,gpointer data)
{
    GtkTreeSelection *sel;
    GtkTreeModel     *model;
    GtkTreeIter       selected_row;
    gint int_data = 0;

    sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(view));

    gtk_tree_selection_get_selected (sel, &model, &selected_row);

    gtk_tree_model_get(model,
                       &selected_row,
                       2,
                       &int_data,
                       -1);

    if ( G_LIKELY (int_data >= 0 && int_data <= 3) )
    {
	GtkWidget *nt = glade_xml_get_widget (xml, "main-notebook");
        gtk_notebook_set_current_page(GTK_NOTEBOOK(nt), int_data);
    }
}

static void
xfpm_settings_tree_view (XfconfChannel *channel, gboolean system_laptop)
{
    GtkWidget *view = glade_xml_get_widget (xml, "treeview");
    GdkPixbuf *pix;
    GtkListStore *list_store;
    GtkTreeIter iter;
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkTreeSelection *sel;
    GtkTreePath *path;
    gint i = 0;
    
    list_store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);

    gtk_tree_view_set_model (GTK_TREE_VIEW(view), GTK_TREE_MODEL(list_store));
    
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(view),TRUE);
    col = gtk_tree_view_column_new();
    //gtk_tree_view_column_set_spacing(col, 10);

    renderer = gtk_cell_renderer_pixbuf_new();
    
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_set_attributes(col, renderer, "pixbuf", 0, NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start (col, renderer, FALSE);
    gtk_tree_view_column_set_attributes (col, renderer, "text", 1, NULL);
    
    gtk_tree_view_append_column(GTK_TREE_VIEW(view),col);
    
    /*General settings */
    pix = xfpm_load_icon("preferences-system", 48); 
    
    gtk_list_store_append(list_store, &iter);
    
    if ( pix )
    {
	    gtk_list_store_set (list_store, &iter, 0, pix, 1, _("General"), 2, i, -1);
	    g_object_unref(pix);
    }
    else
    {
	    gtk_list_store_set (list_store, &iter, 1, _("General"), 2, i, -1);
    }
    i++;
    
    /* ON ac power */
    pix = xfpm_load_icon("xfpm-ac-adapter", 48); 
    gtk_list_store_append(list_store, &iter);
    if ( pix )
    {
	    gtk_list_store_set(list_store, &iter, 0, pix, 1, _("On AC"), 2, i, -1);
	    g_object_unref(pix);
    }
    else
    {
	    gtk_list_store_set(list_store, &iter, 1, _("On AC"), 2, i, -1);
    }
    i++;
    
    if ( system_laptop )
    {
	pix = xfpm_load_icon("battery", 48); 
	gtk_list_store_append(list_store, &iter);
	if ( pix )
	{
		gtk_list_store_set(list_store, &iter, 0, pix, 1, _("On Battery"), 2, i, -1);
		g_object_unref(pix);
	}
	else
	{
		gtk_list_store_set(list_store, &iter, 1, _("On Battery"), 2, i, -1);
	}
    }
    i++;
    
    pix = xfpm_load_icon("applications-other", 48); 
    gtk_list_store_append(list_store, &iter);
    if ( pix )
    {
	    gtk_list_store_set(list_store, &iter, 0, pix, 1, _("Extended"), 2, i, -1);
	    g_object_unref(pix);
    }
    else
    {
	    gtk_list_store_set(list_store, &iter, 1, _("Extended"), 2, i, -1);
    }

    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));

    path = gtk_tree_path_new_from_string("0");

    gtk_tree_selection_select_path(sel, path);
    gtk_tree_path_free(path);

    g_signal_connect(view,"cursor-changed",G_CALLBACK(_cursor_changed_cb),NULL);
}

static void
settings_quit (GtkWidget *widget, XfconfChannel *channel)
{
    g_object_unref (channel);
    xfconf_shutdown();
    gtk_widget_destroy (widget);
    gtk_main_quit();
}

static void dialog_response_cb (GtkDialog *dialog, gint response, XfconfChannel *channel)
{
    switch(response)
    {
	case GTK_RESPONSE_HELP:
	    xfpm_help();
	    break;
	default:
	    settings_quit (GTK_WIDGET (dialog), channel);
	    break;
    }
}

static void
delete_event_cb (GtkWidget *plug, GdkEvent *ev, XfconfChannel *channel)
{
    settings_quit (plug, channel);
}

void
xfpm_settings_dialog_new (XfconfChannel *channel, gboolean system_laptop, 
			  gboolean user_privilege, gboolean can_suspend, 
			  gboolean can_hibernate, gboolean has_lcd_brightness, 
			  gboolean has_lid, gboolean has_sleep_button, 
			  gboolean has_hibernate_button, gboolean has_power_button,
			  GdkNativeWindow id)
{
    GtkWidget *plug;
    GtkWidget *dialog;
    GtkWidget *allbox;
    TRACE("system_laptop=%s user_privilege=%s can_suspend=%s can_hibernate=%s has_lcd_brightness=%s has_lid=%s "\
          "has_sleep_button=%s has_hibernate_button=%s has_power_button=%s",
	  xfpm_bool_to_string (system_laptop), xfpm_bool_to_string (user_privilege),
	  xfpm_bool_to_string (can_suspend), xfpm_bool_to_string (can_hibernate),
	  xfpm_bool_to_string (has_lcd_brightness), xfpm_bool_to_string (has_lid),
	  xfpm_bool_to_string (has_sleep_button), xfpm_bool_to_string (has_hibernate_button),
	  xfpm_bool_to_string (has_power_button) );
	  
    xml = glade_xml_new_from_buffer (xfpm_settings_glade,
				     xfpm_settings_glade_length,
				     "xfpm-settings-dialog", NULL);
    
    dialog = glade_xml_get_widget (xml, "xfpm-settings-dialog");

    xfpm_settings_on_ac (channel, user_privilege, can_suspend, can_hibernate, has_lcd_brightness, has_lid );
    
    if ( system_laptop )
	xfpm_settings_on_battery (channel, user_privilege, can_suspend, can_hibernate, has_lcd_brightness, has_lid);
	
    xfpm_settings_tree_view (channel, system_laptop);
    
    xfpm_settings_general   (channel, user_privilege, can_suspend, can_hibernate,
			     has_sleep_button, has_hibernate_button, has_power_button );
			     
    xfpm_settings_advanced  (channel, system_laptop, user_privilege, can_suspend, can_hibernate);
    
    if ( id != 0 )
    {
	allbox = glade_xml_get_widget (xml, "allbox");
	plug = gtk_plug_new (id);
	gtk_widget_show (plug);
	gtk_widget_reparent (allbox, plug);
	g_signal_connect (plug, "delete-event", 
			  G_CALLBACK (delete_event_cb), channel);
	gdk_notify_startup_complete ();
    }
    else
    {
	g_signal_connect (dialog, "response", G_CALLBACK (dialog_response_cb), channel);
	gtk_widget_show (dialog);
    }
}
