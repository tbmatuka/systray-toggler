# systray-toggler

## What does this program do?
It creates a systray icon. When the icon is left clicked, next option
will be selected and the specified command will be ran with that option
as an argument. When the icon is right clicked it will open a menu with
all options, any of which can be selected.

## How do I compile this thing?
Just run this command:

    $ gcc -o systray-toggler systray-toggler.c `pkg-config --cflags --libs gtk+-2.0`

## How do I change the number of options?
Change the definition of OPTIONS to how many options you want to have
and make sure that you have the same number of options and icons listed
in the arrays.

## Why is the coding style not consistent?
Because like most small hacks, parts of this one are "borrowed".
