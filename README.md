# Calendar
A calendar program written in C, using SDL2 as the graphics library.


Instructions of operation:

LEFT Key jumps to the previous month.

RIGHT Key jumps to the next month.

UP Key jumps to the next year.

DOWN Key jumps to the previous year.

Q Key changes locale to USA->INT->EUR->USA

E Key changes locale to USA->EUR->INT->USA

ENTER Key jumps to the current month and year.


Locale is set to North America as default. This can be changed to Europe, and International.

This program uses the Gregorian calendar, but if you set the date to anywhere before October 1582, the Julian calendar will be used instead. If viewed on October 1582, the first four days are based on the Julian calendar, but after that will use the Gregorian dates, starting on October 15, 1582. October 15, 1582 (Friday) is positioned the day after October 4, 1582 (Thursday).

SDL2 may need to be installed and set as dependencies for this to work.
