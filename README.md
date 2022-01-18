TWITCH-CLI
=

About
-
Twitch-cli is a lightweight cli to watch and manage twitch streamers.

How To Use
-

To run this program, simply run **twitch-cli streamer** and the streamers stream will start.

To get the correct name, got to [twitch](https://www.twitch.tv) and go to a streamer page.
The correct name is in the url.

If you want to add streamer, and get update status as to when they are live, you need to edit *config.h*.
Here is an example:

```
char *streamers[] = {
  "streamer1",
  "streamer2",
  "streamer3"
};
```

Then, simply run **twitch-cli** to get a list of you streamer activity. Select the one you want to watch and mpv will start.

If you want to add more streamers, simply modify the config.h file again and reinstall.

You can also **twitch-cli -p** tp print the list of connected streamers to stdout.

You can also run this application in the background and get notifications when a streamer is live.
Run **twitch-cli -d**, and the program will run until you stop it. You can then put it as a deamon in your init system, start it in your .xprofile, or just **twitch-cli -d & disown**.

Installation
-

For twitch-cli to work, you need to install some dependencies before building.
You can usually install them with your package manager.

- [curl](https://github.com/curl/curl)
- [mpv](https://github.com/mpv-player/mpv)
- [yt-dlp](https://github.com/yt-dlp/yt-dlp) ([and probably configure it to work with mpv](https://www.funkyspacemonkey.com/replace-youtube-dl-with-yt-dlp-how-to-make-mpv-work-with-yt-dlp))

```
git clone https://git.chambaz.xyz/twitch-cli
cd twitch-cli/include
# add the streamers in this file
$EDITOR config.h
cd ../build
sudo make clean install
```

This project is only available on Linux.
However, the only thing that needs updating is the makefile, so you could compile it yourself and install the binary in the correct directories yourself.

Dependencies
-

Twitch-cli has three dependencies: cURL to fetch streamer status info, mpv to play the stream and yt-dlp for mpv to get the stream.

Licence
-

This project is licenced under the GPLv2 licence.
For more information, read the LICENSE file.
