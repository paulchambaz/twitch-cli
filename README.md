# twitch-cli

## About

Twitch-cli is a lightweight cli to watch and manage twitch streamers.

## How To Use

First add the list of streamer you want to follow to **~/.config/twitchrc**.

Here is an example of such a file:
```
streamer1
streamer2
streamer3
```

To find what url to add to the file, go to **https://twitch.tv/streamer**.
The name to add is the streamer part of the url.

Then run **twitch-cli -l** to get a list of all streamers that are live.

You can then run **twitch-cli &lt;streamer>** to start their stream.

Note that you can use these commands together - run **twitch-cli $(twitch-cli -l)** will use dmenu to prompt the user on which live streamer they want to watch.

Finally, you can run **twitch-cli -d** or better **twitch-cli -d & disown** to run this program as a daemon.
It will notify you every time a streamer goes live.

## Installation

For twitch-cli to work, you need to install some dependencies before building.
You can usually install them with your package manager.

- [curl](https://github.com/curl/curl)
- [mpv](https://github.com/mpv-player/mpv)
- [yt-dlp](https://github.com/yt-dlp/yt-dlp) ([and probably configure it to work with mpv](https://www.funkyspacemonkey.com/replace-youtube-dl-with-yt-dlp-how-to-make-mpv-work-with-yt-dlp))
- [dmenu](https://tools.suckless.org/dmenu/)(optional - useful to combine the commands)

```
git clone https://gitlab.com/Paul-Chambaz/twitch-cli
cd twitch-cli/build
sudo make clean install
```

This project is only available on Linux.
However, the only thing that needs updating is the makefile and the paths, so you could compile it yourself and install the binary in the correct directories yourself.

## Dependencies

Twitch-cli has three dependencies: cURL to fetch streamer status info, mpv to play the stream and yt-dlp for mpv to get the stream. I also recommend using dmenu to combine the commands together. Please support their work.

## Licence

This project is licenced under the GPLv2 licence.
For more information, read the LICENSE file.
