MODULES = gtk-column-ffmpeg-bitrate \
          gtk-column-ffmpeg-video-framerate \
          gtk-column-ffmpeg-video-bitrate \
          gtk-column-ffmpeg-resolution \
          gtk-column-ffmpeg-audio-bitrate \
          gtk-column-ffmpeg-video-codec \
          gtk-column-ffmpeg-video-codec-long \
          gtk-column-ffmpeg-audio-samplerate \
          gtk-column-ffmpeg-audio-channels \
          gtk-column-ffmpeg-duration \
          gtk-column-ffmpeg-audio-codec-long \
          gtk-column-ffmpeg-audio-codec

LIBDIR = /usr/lib/x86_64-linux-gnu
CFLAGS = `pkg-config --cflags libfm gtk+-2.0 libavcodec libavformat libavutil`
AM_LDFLAGS = -rpath $(LIBDIR) -no-undefined -module -avoid-version -lavcodec -lavformat -lavutil

all:
	$(foreach MODULE,$(MODULES),libtool --mode=compile gcc $(CFLAGS) -c $(MODULE).c;)
	$(foreach MODULE,$(MODULES),libtool --mode=link gcc $(AM_LDFLAGS) $(MODULE).lo -o $(MODULE).la;)

install:
	$(foreach MODULE,$(MODULES),libtool --mode=install install -c $(MODULE).la $(LIBDIR)/libfm/modules/;)
	libtool --finish $(LIBDIR)/libfm/modules/
