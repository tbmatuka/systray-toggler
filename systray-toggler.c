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

Why is the coding style not consistent?
  Because like most small hacks, parts of this one are "borrowed".

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define TOOLTIP "Selected option:" // default tooltip prefix

guint selected = 0;

int debugFlag; // flag set by --debug
int runAtStartFlag; // flag set by --run-at-start
int runOnSameFlag; // flag set by --run-on-same
int quitMenuOptionFlag; // flag set by --quit-menu-option

int optionNum = 0; // number of options
struct menuOption *firstOption = NULL;
struct menuOption *lastOption = NULL;
gchar *defaultIcon = NULL;

struct menuOption
	{
	gchar *option;
	gchar *command;
	gchar *icon;
	gchar *tooltip;
	struct menuOption *next;
	};

#include "parseArgs.h"

struct menuOption *selectedOption = NULL;

GtkStatusIcon *tray_icon;

void changeOption();
void setOption(GtkWidget *menuItem, gpointer user_data);
void tray_icon_on_click(GtkStatusIcon *tray_icon, gpointer user_data);
void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer menu);
void GtkSetIcon(GtkStatusIcon *tray_icon, gchar *icon);
static GtkStatusIcon *createTrayIcon(GtkWidget *menu);
GtkWidget *createMenu();

void changeOption()
	{
	// execute selected option's command if it's set
	if(selectedOption->command != NULL)
		{
		system((*selectedOption).command);
		if(debugFlag) fprintf(stderr, "executed command: %s\n", (*selectedOption).command);
		}
		
	// change icon
	if(selectedOption->icon != NULL)
		{
		GtkSetIcon(tray_icon, (*selectedOption).icon);
		if(debugFlag) fprintf(stderr, "changed icon to: %s\n", (*selectedOption).icon);
		}
	else
		{
		// use default icon (the first one listed)
		GtkSetIcon(tray_icon, defaultIcon);
		if(debugFlag) fprintf(stderr, "changed icon to default icon: %s\n", defaultIcon);
		}
		
	// change tooltip text
	gchar *tooltip = NULL;
	if(selectedOption->tooltip == NULL) asprintf(&tooltip, "%s %s", TOOLTIP, (*selectedOption).option);
	else asprintf(&tooltip, (*selectedOption).tooltip);

	gtk_status_icon_set_tooltip(tray_icon, tooltip);
	
	if(debugFlag) fprintf(stderr, "changed tooltip to: %s\n", tooltip);
	}

void setOption(GtkWidget *menuItem, gpointer user_data)
	{
	gint setTo = 0;
	if(debugFlag) fprintf(stderr, "option selected from menu: %s\n", user_data);
		
	if((*selectedOption).option != user_data)
		{
		struct menuOption *currentOption = firstOption;
		while(currentOption != NULL)
			{
			if((*currentOption).option == user_data)
				{
				selectedOption = currentOption;
				break;
				}
			
			currentOption = currentOption->next;
			}
		
		changeOption();
		}
	else
		{
		if(runOnSameFlag)
			{
			changeOption();
			}
		else
			{
			if(debugFlag) fprintf(stderr, "selected option is the same as current one, not running changeOption()\n");
			}
		}
	}

void tray_icon_on_click(GtkStatusIcon *tray_icon, gpointer user_data)
	{
	if(selectedOption->next != NULL) selectedOption = selectedOption->next;
	else selectedOption = firstOption;
	
	if(debugFlag) fprintf(stderr, "icon clicked, rotating selected option to: %s\n", (*selectedOption).option);
		
	changeOption();
	}

void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer menu)
	{
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
	}

void GtkSetIcon(GtkStatusIcon *tray_icon, gchar *icon)
	{
	GError *gerror = NULL;
	GdkPixbuf *pixbuf = NULL;
	pixbuf = gdk_pixbuf_new_from_file(icon, &gerror);
	if(pixbuf == NULL)
		{
		fprintf(stderr, "Failed to open icon: %s\n", icon);
		exit(1);
		}
	gtk_status_icon_set_from_pixbuf(tray_icon, pixbuf);
	}

static GtkStatusIcon *createTrayIcon(GtkWidget *menu)
	{
	GtkStatusIcon *tray_icon;

	tray_icon = gtk_status_icon_new();
	g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(tray_icon_on_click), NULL);
	g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(tray_icon_on_menu), menu);
	if(selectedOption->icon != NULL)
		{
		GtkSetIcon(tray_icon, (*selectedOption).icon);
		if(debugFlag) fprintf(stderr, "changed icon to: %s\n", (*selectedOption).icon);
		}
	else
		{
		GtkSetIcon(tray_icon, defaultIcon);
		if(debugFlag) fprintf(stderr, "changed icon to default icon: %s\n", defaultIcon);
		}
	
	gchar *tooltip = NULL;
	if(selectedOption->tooltip == NULL) asprintf(&tooltip, "%s %s", TOOLTIP, (*selectedOption).option);
	else asprintf(&tooltip, (*selectedOption).tooltip);
	
	gtk_status_icon_set_tooltip(tray_icon, tooltip);
	gtk_status_icon_set_visible(tray_icon, TRUE);

	return tray_icon;
	}

GtkWidget *createMenu()
	{
	guint i;
	GtkWidget *menu, *menuItem;
	
	menu = gtk_menu_new();
	
	struct menuOption *currentOption = firstOption;
	while(currentOption != NULL)
		{
		menuItem = gtk_menu_item_new_with_label((*currentOption).option);
		g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(setOption), (*currentOption).option);
		gtk_menu_shell_append(GTK_MENU_SHELL (menu), menuItem);
		
		currentOption = currentOption->next;
		}

	if(quitMenuOptionFlag)
		{
		if(debugFlag) fprintf(stderr, "quitMenuOptionFlag is set, adding the menu option\n");
		menuItem = gtk_menu_item_new_with_label("Quit");
		g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(gtk_main_quit), NULL);
		gtk_menu_shell_append(GTK_MENU_SHELL (menu), menuItem);
		}
	
	gtk_widget_show_all(menu);
	return menu;
	}

int main(int argc, char **argv)
	{
	parseArgs(argc, argv);

	if(debugFlag)
		{
		fprintf(stderr, "debugFlag is set\n");
		if(runAtStartFlag) fprintf(stderr, "runAtStartFlag is set\n");
		if(runOnSameFlag) fprintf(stderr, "runOnSameFlag is set\n");
		if(quitMenuOptionFlag) fprintf(stderr, "quitMenuOptionFlag is set\n");
		}

	/* Print any remaining command line arguments(not options). */
	if(debugFlag && optind < argc)
		{
		fprintf(stderr, "non-option ARGV-elements: ");
		while(optind < argc) fprintf(stderr, "%s ", argv[optind++]);
		fprintf(stderr, "\n");
		}

	if(debugFlag && firstOption != NULL)
		{
		struct menuOption *nextOption = firstOption;
		struct menuOption *currentOption = NULL;
		while(nextOption != NULL)
			{
			currentOption = nextOption;
			nextOption = currentOption->next;
			
			fprintf(stderr, "option - %s", (*currentOption).option);
			if(currentOption->icon != NULL) fprintf(stderr, " - %s", (*currentOption).icon);
			if(currentOption->command != NULL) fprintf(stderr, " - %s", (*currentOption).command);
			fprintf(stderr, "\n");
			}
		}
	
	if(firstOption != NULL) selectedOption = firstOption;
	
	gtk_init(0, NULL);
	tray_icon = createTrayIcon(createMenu());
	
	if(runAtStartFlag && selectedOption->command != NULL)
		{
		system((*selectedOption).command);
		if(debugFlag) fprintf(stderr, "executed command at start: %s\n", (*selectedOption).command);
		}
	
	gtk_main();

	if(debugFlag) fprintf(stderr, "exited main gtk loop\n");
	
	destroyOptions();
	return 0;
	}
