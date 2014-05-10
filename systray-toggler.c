/*

systray-toggler

Copyright (c) 2014 Tin Benjamin Matuka (www.tbmatuka.com)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.


What does this program do?
  It creates a systray icon. When the icon is left clicked, next option
  will be selected and the specified command will be ran with that option
  as an argument. When the icon is right clicked it will open a menu with
  all options, any of which can be selected.

How do I compile this thing?
 Just run this command:
$ gcc -o systray-toggler systray-toggler.c `pkg-config --cflags --libs gtk+-2.0`

How do I change the number of options?
  Change the definition of OPTIONS to how many options you want to have
  and make sure that you have the same number of options and icons listed
  in the arrays.

Why is the coding style not consistent?
  Because like most small hacks, parts of this one are "borrowed".

*/

#include <gtk/gtk.h>
#include <string.h>

#define OPTIONS 2
#define COMMAND "notify-send "
#define TOOLTIP "Current option: "

//#define DEBUG

gchar option[OPTIONS][256] = {
"on",
"off"
	}; // options to be used as arguments

gchar icon[OPTIONS][512] = {
"/usr/share/icons/gnome/32x32/emotes/face-laugh.png",
"/usr/share/icons/gnome/32x32/emotes/face-crying.png"
	}; // paths to icons for each option

guint selected = 0;

GtkStatusIcon *tray_icon;

void changeOption()
	{
	// execute command with selected option
	gchar command[128] = COMMAND;
	strcat(command, option[selected]);
	system(command);
	
	#ifdef DEBUG
		printf("executed command: %s\n", command);
	#endif
	
	// change icon
	gtk_status_icon_set_from_file(tray_icon, icon[selected]);
	
	#ifdef DEBUG
		printf("changed icon to: %s\n", icon[selected]);
	#endif
	
	// change tooltip text
	gchar tooltip[512] = TOOLTIP;
	strcat(tooltip, option[selected]);
	gtk_status_icon_set_tooltip(tray_icon, tooltip);
	
	#ifdef DEBUG
		printf("changed tooltip to: %s\n", tooltip);
	#endif
	}

void setOption(GtkWidget *menuItem, gpointer user_data)
	{
	guint setTo = GPOINTER_TO_UINT(user_data);
	#ifdef DEBUG
		printf("option selected from menu: %s\n", option[setTo]);
	#endif
	
	if(selected != setTo)
		{
		selected = setTo;
		changeOption();
		}
	else
		{
		#ifdef DEBUG
			printf("selected option is the same as current one, not running command\n");
		#endif
		}
	}

void tray_icon_on_click(GtkStatusIcon *tray_icon, gpointer user_data)
	{
	selected++;
	selected%= OPTIONS;
	
	#ifdef DEBUG
		printf("icon clicked, rotating selected option to: %s\n", option[selected]);
	#endif
	
	changeOption();
	}

void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer menu)
	{
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
	}

static GtkStatusIcon *createTrayIcon(GtkWidget *menu)
	{
	GtkStatusIcon *tray_icon;

	tray_icon = gtk_status_icon_new();
	g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(tray_icon_on_click), NULL);
	g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(tray_icon_on_menu), menu);
	gtk_status_icon_set_from_file(tray_icon, icon[selected]);
	
	gchar tooltip[512] = TOOLTIP;
	strcat(tooltip, option[selected]);
	gtk_status_icon_set_tooltip(tray_icon, tooltip);
	gtk_status_icon_set_visible(tray_icon, TRUE);

	return tray_icon;
	}

GtkWidget *createMenu()
	{
	guint i;
	GtkWidget *menu, *menuItem[OPTIONS];
	
	menu = gtk_menu_new();
	for(i = 0; i < OPTIONS; i++)
		{
		menuItem[i] = gtk_menu_item_new_with_label(option[i]);
		g_signal_connect(G_OBJECT(menuItem[i]), "activate", G_CALLBACK(setOption), GUINT_TO_POINTER((guint)i));
		gtk_menu_shell_append(GTK_MENU_SHELL (menu), menuItem[i]);
		}
	gtk_widget_show_all(menu);
	return menu;
	}

int main(int argc, char **argv){
	
	gtk_init(&argc, &argv);
	tray_icon = createTrayIcon(createMenu());
	gtk_main();

	return 0;
}
