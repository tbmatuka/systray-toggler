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

*/

struct menuOption *parseNewMenuOption(char *newOption)
	{
	struct menuOption *retVal = malloc(sizeof(struct menuOption));
	
	if(retVal == NULL) return NULL;
	
	if(asprintf(&retVal->option, newOption) == -1) return NULL;
	
	if(optionNum == 0)
		{
		firstOption = retVal;
		lastOption = retVal;
		}
	else
		{
		lastOption->next = retVal;
		lastOption = retVal;
		}
	
	optionNum++;
	
	retVal->icon = NULL;
	retVal->command = NULL;
	retVal->tooltip = NULL;
	
	return retVal;
	}

int parseSetIcon(gchar *newIcon)
	{
	if(lastOption == NULL) return -1;
	
	if(lastOption->icon != NULL)
		{
		free(lastOption->icon);
		lastOption->icon = NULL;
		}
	
	if(asprintf(&lastOption->icon, newIcon) == -1) return -1;
	
	if(defaultIcon == NULL) defaultIcon = lastOption->icon;
	return 0;
	}

int parseSetCommand(gchar *newCommand)
	{
	if(lastOption == NULL) return -1;
	
	if(lastOption->command != NULL)
		{
		free(lastOption->command);
		lastOption->command = NULL;
		}
	
	if(asprintf(&lastOption->command, newCommand) == -1) return -1;
	
	return 0;
	}

int parseSetTooltip(gchar *newTooltip)
	{
	if(lastOption == NULL) return -1;
	
	if(lastOption->tooltip != NULL)
		{
		free(lastOption->tooltip);
		lastOption->tooltip = NULL;
		}
	
	if(asprintf(&lastOption->tooltip, newTooltip) == -1) return -1;
	
	return 0;
	}

void destroyOptions(void)
	{
	if(firstOption == NULL) return;
	
	struct menuOption *nextOption = firstOption;
	struct menuOption *currentOption = NULL;
	while(nextOption != NULL)
		{
		currentOption = nextOption;
		nextOption = currentOption->next;
		
		if(currentOption->icon != NULL) free(currentOption->icon);
		if(currentOption->command != NULL) free(currentOption->command);
		free(currentOption->option);
		free(currentOption);
		}
	}

void parseHelp(void)
	{
	puts("Usage: systray-toggler [OPTION]... [ENTRY]...\n");
	puts("Every [ENTRY] must begin with an -o");
	puts("  -o, --option=STRING      String that is displayed in the menu for this [ENTRY]");
	puts("  -c, --command=STRING     Command that is run for this [ENTRY] - optional");
	puts("  -i, --icon=PATH          Icon that is shown for this [ENTRY] - optional");
	puts("  -t, --tooltip=STRING     Tooltip that is shown for this [ENTRY] - optional\n");
	puts("Other options, all off by default:");
	puts("      --run-at-start       Run the first [ENTRY]'s command on startup");
	puts("      --run-on-same        Run the [ENTRY]'s command when it's selected in the menu,");
	puts("                           even if that's the currently selected [ENTRY]");
	puts("      --quit-menu-option   Show a 'Quit' option in the menu\n");
	puts("      --debug              Display debug info");
	puts("  -h, --help               Display this text\n");
	}

void parseArgs(int argc, char **argv)
	{
	int c;

	while(1)
		{
		static struct option long_options[] = {
			/* These options set a flag. */
			{"debug",   no_argument,       &debugFlag, 1},
			{"run-at-start", no_argument,  &runAtStartFlag, 1},
			{"run-on-same",  no_argument,  &runOnSameFlag, 1},
			{"quit-menu-option",  no_argument,  &quitMenuOptionFlag, 1},
			/* These options don't set a flag. We distinguish them by their indices. */
			{"option",  required_argument, 0, 'o'},
			{"command", required_argument, 0, 'c'},
			{"icon",    required_argument, 0, 'i'},
			{"tooltip", required_argument, 0, 't'},
			{"help",    no_argument, 0, 'h'},
			{0, 0, 0, 0}
		};
		
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "o:c:i:t:", long_options, &option_index);

		/* Detect the end of the options. */
		if(c == -1) break;

		switch(c)
			{
			case 0:
				/* If this option set a flag, do nothing else now. */
				if(long_options[option_index].flag != 0) break;
				fprintf(stderr, "option %s", long_options[option_index].name);
				if(optarg) fprintf(stderr, " with arg %s", optarg);
				fprintf(stderr, "\n");
				break;
			
			case 'o':
				// new option
				//fprintf(stderr, "option -o with value `%s'\n", optarg);
				parseNewMenuOption(optarg);
				break;
			
			case 'c':
				// command for current option
				//fprintf(stderr, "option -c with value `%s'\n", optarg);
				parseSetCommand(optarg);
				break;
			
			case 'i':
				// icon for current option
				//fprintf(stderr, "option -i with value `%s'\n", optarg);
				parseSetIcon(optarg);
				break;

			case 't':
				// tooltip for current option
				//fprintf(stderr, "option -t with value `%s'\n", optarg);
				parseSetTooltip(optarg);
				break;

			case 'h':
				// display help and exit
				//fprintf(stderr, "option -h\n");
				parseHelp();
				exit(0);
				break;

			case '?':
				/* getopt_long already printed an error message. */
				break;

			default:
				abort();
			}
		}
	
	if(optionNum == 0)
		{
		fprintf(stderr, "You need to specify at least one [ENTRY] and give it an icon.\nUse --help if you need more information.\n");
		exit(1);
		}
	
	if(defaultIcon == NULL)
		{
		fprintf(stderr, "You need to specify at least one icon, you can't have a systray app without an icon.\nUse --help if you need more information.\n");
		exit(1);
		}
	}
