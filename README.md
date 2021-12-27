TWITCH-CLI
=

About
-
Twitch-cli is a lightweight cli to watch and manage twitch streamers.

How To Use
-

First, navigate to ~/.config/twitch-cli/streamers.txt.
On this file, you can add streamers that you want to follow, navigate to https://www.twitch.tv and find your streamer. 
The real name is in the url, but most of the time, it's the one you're thinking about. Be careful to only add one name per line and not to leave empty lines at the start or end.

Then, simply run *twitch-cli* to get a list of you streamer activity. Select the one you want to watch and mpv will start. 

Installation
-

```
git clone https://gitlab.com/Paul-Chambaz/twitch-cli
cd twitch-cli/build
sudo make clean install
```
This project is only available on Linux. However, the only this that needs updating is the makefile, so you could compile it yourself and install the binary in the correct directories yourself.

Dependencies
-

Twitch-cli has two dependencies: cURL to fetch streamer status info and mpv to play the stream.

Licence
-

This project is licenced under the GPLv2 licence.
For more information, read the LICENSE file.
