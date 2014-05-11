# systray-toggler

## What does this program do?
It creates a systray icon. When the icon is left clicked, next option
will be selected and the specified command will be ran with that option
as an argument. When the icon is right clicked it will open a menu with
all options, any of which can be selected.

## How do I compile this thing?
Just run this command:

    $ gcc -o systray-toggler systray-toggler.c `pkg-config --cflags --libs gtk+-2.0`

## Why is the coding style not consistent?
Because like most small hacks, parts of this one are "borrowed".

## How do I use this?
Here is the output of --help:

```
Every [ENTRY] must begin with an -o
  -o, --option=STRING      String that is displayed in the menu for this [ENTRY]
  -c, --command=STRING     Command that is run for this [ENTRY] - optional
  -i, --icon=PATH          Icon that is shown for this [ENTRY] - optional
  -t, --tooltip=STRING     Tooltip that is shown for this [ENTRY] - optional

Other options, all off by default:
      --run-at-start       Run the first [ENTRY]'s command on startup
      --run-on-same        Run the [ENTRY]'s command when it's selected in the menu,
                           even if that's the currently selected [ENTRY]
      --quit-menu-option   Show a 'Quit' option in the menu

      --debug              Display debug info
  -h, --help               Display this text
```
