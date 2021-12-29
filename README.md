TWITCH-CLI
=

About
-
Twitch-cli is a lightweight cli to watch and manage twitch streamers.

How To Use
-

To add streamers to the program, you need to edit the config.h file located in the include folder.
Here is an example:

```
char *streamers[] = {
  "streamer1",
  "streamer2",
  "streamer3"
};
```

To get the correct name, got to [twitch](https://www.twitch.tv) and go to a streamer page.
The correct name is in the url.

Then, simply run **twitch-cli** to get a list of you streamer activity. Select the one you want to watch and mpv will start. 

Installation
-

For twitch-cli to work, you need to install some dependencies before building.

- curl
- mpv
- yt-dlp (and configure it to work with mpv)

```
git clone https://git.chambaz.xyz/twitch-cli
cd twitch-cli/include
# add the streamers in this file
$EDITOR config.h
cd ../build
sudo make clean install
```

This project is only available on Linux. However, the only this that needs updating is the makefile, so you could compile it yourself and install the binary in the correct directories yourself.

Dependencies
-

Twitch-cli has three dependencies: cURL to fetch streamer status info, mpv to play the stream and yt-dlp for mpv to get the stream.

Licence
-

This project is licenced under the GPLv2 licence.
For more information, read the LICENSE file.
