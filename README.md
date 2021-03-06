# Preface

**Please note that mooproxy has been unmaintained since 2011.**

I'm not aware of any serious bugs, and I expect it to still work fine, but who knows.

Also please be aware that it is a network service written in C, the language of buffer overflows.
At the time, I took great care to make the network code secure, but I offer no guarantees.

Finally note that the password hashing uses single-pass MD5, which is considered inadequate today.



# About

**_Mooproxy_ is a smart proxy for MUD/MOO connections.**



Mooproxy acts a layer between your MUD/MOO client and a MUD/MOO server, as shown below:

    MUD/MOO client  <---->  Mooproxy  <---->  MUD/MOO server

The intention is that the proxy remains connected to the server, even if you disconnect your MUD/MOO client, for example, if you shutdown your computer, take your laptop to a different place, or have network problems.

Mooproxy also provides centralised logging, which is useful if you use multiple MUD/MOO clients.



# Manual compiling and installing

First, you can edit the Makefile to change some options (like installation path and compiler flags), if you want to.

Next, to compile mooproxy, simply run

    make

in the source directory.
You need a working C compiler, and the development libraries for libc.
Mooproxy also uses some functionality that may not be present on all Unices; see Portability.

You can install by running

    make install

This will install the mooproxy binary (by default in `/usr/local/bin`), and the manpage (by default in `/usr/local/share/man/man1`).
Alternatively, you can install by simply copying the mooproxy executable to a suitable place, such as `/usr/local/bin` or `~/bin`.
Mooproxy requires no other files (except configuration files) to run.




# Installing the Debian package

Currently, an i386 Debian package is provided. You can install it with

    dpkg -i mooproxy_<version>.deb



# Creating a Debian package

You can create your own Debian package as well (for another architecture,
for example). Follow the following steps:

  - Download `mooproxy-debsrc-<version>.tar.gz`
  - Unpack it somewhere
  - cd into `mooproxy-<version>`
  - Unpack `mooproxy_<version>.orig.tar.gz`
  - Run `zcat mooproxy_<version>.diff.gz | patch -p0`
  - cd into `mooproxy-<version>`
  - Run `dpkg-buildpackage -us -uc -rfakeroot`

The Debian package (and some other files) will be generated in the top-level `mooproxy-<version>` directory.



# Using

Running mooproxy is simple, just invoke it like this:

    mooproxy -w <wld>

where `<wld>` is the name of the world.
Mooproxy will use the configuration file `~/.mooproxy/worlds/<wld>`.
See [ExampleConfig](ExampleConfig) for an example configuration file.
The ExampleConfig file contains all configurable options, and the settings in ExampleConfig are exactly the same as the builtin defaults in mooproxy.

Mooproxy accepts commands to change a lot of its behaviour run-time.
When connected to mooproxy, do `/help` for a brief list of commands.



# Authentication

There are two situations in mooproxy that require authentication;

  - A MUD/MOO client connects to mooproxy
  - Mooproxy connects to a MUD/MOO server

Mooproxy uses the same authentication string for both.
In the configuration file for the world, you should put a MD5 hash of the authentication string.
When the client connects to mooproxy, the client or the user supplies the authentication string to mooproxy.
Mooproxy hashes the authentication string from the client, compares it to the stored MD5 hash, and if they match, the client is accepted.

At this point, mooproxy stores the _literal_ authentication string received from the client in its memory.
When mooproxy logs in to the server (for example, because autologin is enabled), mooproxy uses this literal authentication string to authenticate itself to the MUD/MOO server.

The MD5 hash can be created with the following command:

    mooproxy --md5crypt


An example scenario.
Assume the MUD/MOO server expects `connect <username> <password>`, and your MUD/MOO client sends `connect Gandalf Shire`.

Now, run the following command:

    mooproxy --md5crypt

When mooproxy asks for your authentication string (it will ask twice, to save you from typos), you should enter:

    connect Gandalf Shire

Mooproxy will print a MD5 hash of the string, e.g.:

    $1$DWuIlrlu$vjiUiV7i23jNMCpSkDzDz.

You put this string in your configuration file, like so:

    auth_hash = "$1$DWuIlrlu$vjiUiV7i23jNMCpSkDzDz."

Now, when your client connects to mooproxy, it is prompted for the authentication string.
If your client responds with:

    connect Gandalf Shire

Mooproxy will accept the client, and store `connect Gandalf Shire` in its memory.
Now, when mooproxy connects to the MUD/MOO server, and autologin is enabled, mooproxy sends the saved string to the MUD/MOO server, logging you in, just like your MUD/MOO client would without mooproxy in the middle.

To change the authentication string from within mooproxy, you must also provide the old literal authentication string, like this:

    /set auth_hash <new hash> <old literal>

So, continuing the example scenario above, if you change your password to `Mirkwood`, you must run `mooproxy --md5crypt` to generate a MD5 hash of the new authentication string:

    $ mooproxy --md5crypt
    Authentication string: connect Gandalf Mirkwood
    MD5 hash: $1$PzGmXA.I$qmZomnNK2gBW6tR8iTJLA/

You can then set the new hash like this:

    /set auth_hash $1$PzGmXA.I$qmZomnNK2gBW6tR8iTJLA/ "connect Gandalf Shire"

The MD5 hash should _not_ be enclosed in quotes.
It is recommended to enclose the old literal in quotes, to remove ambiguity.
After changing `auth_hash` from within mooproxy, mooproxy no longer has a literal authentication string.
Therefore, autologin will not work until you re-authenticate to mooproxy (for example, by disconnecting your client, and connecting again).



# MCP

MUD/MOO servers and clients may use the MUD/MOO Client Protocol (MCP) to exchange some information out-of-band.
Common uses include periodically showing a tip or something else in a status bar, or maintaining a userlist on the
client.
For more information, see <http://www.moo.mud.org/mcp/index.html>.

MCP is a stateful protocol; it has to be initialised and maintained properly.
When a MUD/MOO client connects to the MUD/MOO server, it expects to setup a MCP session.
Consider the following scenario:

 - Mooproxy is started
 - The client connects to mooproxy
 - Mooproxy connects to the MUD/MOO server
 - The MUD/MOO server sends MCP handshake. The client replies, and a MCP session is set up.
   Mooproxy relays all MCP messages.
 - The client disconnects
 - The client connects again

Now, from the perspective of the MUD/MOO server, a MCP session still exists, but the client cannot continue this old session (due to a lack of information on the client).
So the MCP session needs to be torn down and rebuilt.

Because Standard MCP doesn't provide a mechanism to re-initialise the MCP session, a MCP package has been created for this purpose.
The package is called `dns-nl-icecrew-mcpreset`.
More information can be found in [mcpreset.html](mcpreset.html).

Mooproxy uses the server MCP reset.
When a client connects to mooproxy, and mooproxy is already connected to a MUD/MOO server, mooproxy sends a MCP reset command to the MUD/MOO server:

    #$#dns-nl-icecrew-mcpreset-reset <mcpkey>

When the MUD/MOO server receives this message, it should terminate the current MCP session for the client, and start a new one by sending the MCP handshake.
Mooproxy then relays this handshake to the newly connected client, which will respond, and a new MCP session will be set up.

If working MCP is desired, mooproxy can only be used with MUD/MOO servers that support `dns-nl-icecrew-mcpreset-reset`.



# /recall

In mooproxy 0.1.3, the recall command has received a significant update.
If you invoke the recall command with just a number, it will recall that number of lines in their full glory, just like before:

    /recall 50

But now recall supports the following syntax as well:

    /recall [from <timespec>] [to <timespec>] [search <text>]

This will recall any lines matching `<text>` in the period from `<from timespec>` to `<to timespec>`.

A timespec is one or more of the following:

    now
       Sets the time to the current date and time.
    today
       Sets the time to 0:00 today.
    yesterday
       Sets the time to 0:00 yesterday.
    last/next monday/tuesday/...
       Next <weekday> sets the time to 0:00 of the first <weekday> from today.
       Last <weekday> sets it to 0:00 of the last <weekday> before today.
    [YY/]MM/DD
       Set the time to 0:00 of the given day.
       If no year is specified, use MM/DD within the current year.
    HH:MM[:SS]
       Set the time to HH:MM within the current day.
       If seconds are not specified, set them to 00.
    -/+ <number> seconds/minutes/hours/days
       Change the time to be <number> <units> earlier or later.
    -/+ <number> lines
       Search in <number> lines before/after <from timespec>.

Notes:

   - Lines are recalled with their colours stripped.
   - -/+ <number> lines is special. It can only be used together with 'to',
     and when used, it must be the only timespec.
   - If both from and to are used, from must appear before to.
   - If search is used, it must be the last keyword.
   - The search keyword is case insensitive. It also treats <text> as a very
     primitive regular expression, which _only_ supports . (matches any
     character) and .* (matches zero or more any characters).
   - Weekdays support the usual abbreviations (mon, tue, wed, ...).
   - The units in relative timespecs can be abbreviated too (down to individual
     characters). +2 minutes, +2 mins, +2 min, +2 m are all equivalent.
   - The spacing in -/+ <number> <unit> is optional. + 2 min, +2 min, +2min
     are all equivalent.

Examples. Assume the current date and time are Wed Aug 22, 20:42:11.

Recall all lines between 10:00:00 and 11:00:00 today:

    /recall from 10:00 to 11:00

Recall all lines from 10:00 to 11:00 yesterday:

    /recall from yesterday 10:00 to 11:00

Search the last half hour for any lines containing "yes" before "play":

    /recall from -30 mins search yes.*play

Recall 50 lines after 10:00:00 today:

    /recall from 10:00 to +50 lines

Recall the last 80 lines:

    /recall to -80 lines

Recall everything from Mon 20/08 00:00 to Wed 22/08 00:00:

    /recall from last monday to today.

Recall everything from Sat 18/08 00:00 to Mon 20/08 00:00:

    /recall from last saturday to next monday

Recall from 11:35 to 12:35 the wednesday after 22/04 (a sunday):

    /recall from 04/22 next wed 11:35 to +1 hour

When you invoke recall, it will first report what it is going to recall, so you can check if recall will actually inspect the lines you intended it to inspect:

    % Recalling from Wed 2007/04/25 11:35:00 to Wed 2007/04/25 12:35:00.

Then it will produce the matching lines, with all colours stripped.
Finally, it will report how many lines it considered:

    % Recall end (X / Y / Z).

Here, X is the number of lines mooproxy has in its history.
Y is the number of lines that matched the time criteria (i.e. from and to).
Finally, Z is the number of lines actually recalled (i.e. that matched the time criteria and search criteria).



# Logfiles change from 0.1.1 to 0.1.2

In mooproxy 0.1.2, the logging was changed to log into a nested hierarchy of directories, instead of all files in a single directory.

Mooproxy versions older than 0.1.2 log like this:

    logs
    |-- world one - 2007-01-01.log
    |-- world one - 2007-02-01.log
    |-- world two - 2007-02-01.log
    |-- world two - 2007-03-01.log
    |-- world two - 2007-03-02.log
    `-- world two - 2007-03-03.log

Mooproxy 0.1.2 and later log like this:

    logs
    |-- world one
    |   |-- 2007-01
    |   |   `-- world one - 2007-01-01.log
    |   `-- 2007-02
    |       `-- world one - 2007-02-01.log
    `-- world two
        |-- 2007-02
        |   `-- world two - 2007-02-01.log
        `-- 2007-03
            |-- world two - 2007-03-01.log
            |-- world two - 2007-03-02.log
            `-- world two - 2007-03-03.log

To move any existing logs from the old layout to the new layout, use the update-logs.sh that's provided with mooproxy 0.1.2 (in the Debian package, the shell script resides in `/usr/share/doc/mooproxy`).

Please make a backup of your current logfiles before running this script.



# Portability

Mooproxy is designed to run on modern Unices, most notably GNU/Linux and the open source BSD variants.
Mooproxy is written in ANSI C, using mostly POSIX-defined systemcalls.
This section documents the additional requirements for mooproxy.

As of this writing, mooproxy is tested on Debian Unstable with Linux 2.6.

Portability pitfalls:

 - Mooproxy uses `vasprintf()`. This is a GNU extension that is also implemented in the BSDs.
 - Mooproxy uses `getopt_long()`. This is a GNU extension that is also implemented in the BSDs.
 - Mooproxy uses `getaddrinfo()` and `getnameinfo()` for address family independent networking.
   These functions are defined in POSIX, but they're fairly recent additions, which could lead to problems on older OSes.
 - On systems with IPv6, mooproxy works best if the (pretty recent) `IPV6_V6ONLY` socket option is available, but this is not required.
 - Mooproxy uses `crypt()`, and expects it to support MD5 hashing.
   `crypt()` is defined in POSIX, but MD5 hashing is a GNU extension that is also implemented in the BSDs.
 - Mooproxy uses the `S_ISLNK()` macro, which is mandated in `POSIX.1-2001` but not in earlier versions.



# Copyright, license, acknowledgements

Mooproxy is copyright 2001-2011 Marcel Moreaux.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 dated June, 1991.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Thanks (for testing, reporting bugs, and suggestions) to:

 - Paul van Tilburg
 - Wouter Lueks
 - Admar Schoonen
 - Bram Senders
 - Christian Luijten



# Further information

 - [BUGS](BUGS): A list of known problems.
 - [TODO](TODO): Todo items, mostly wishlist stuff.
 - [Changelog](Changelog): List of changes by release.
 - [ExampleConfig](ExampleConfig): Example configuration file with all default values.
 - [dataflow.pdf](dataflow.pdf): Documents the flow of user input / server responses within the program.
 - [mcpreset.html](mcpreset.html): MCP Package (`dns-nl-icecrew-mcpreset`) specification.
