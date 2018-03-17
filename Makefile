prefix=/usr

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

target=$(shell gcc -dumpmachine)
builddir=.build
libdir = $(DESTDIR)$(prefix)/lib/$(target)
CFLAGS = `pkg-config --cflags libfm gtk+-2.0 libavcodec libavformat libavutil`
LDFLAGS = -rpath $(libdir) -no-undefined -module -avoid-version -lavcodec -lavformat -lavutil

all:
	mkdir -p build
	$(foreach MODULE,$(MODULES),libtool --mode=compile gcc $(CFLAGS) -c src/$(MODULE).c -o $(builddir)/$(MODULE).o;)
	$(foreach MODULE,$(MODULES),libtool --mode=link gcc $(LDFLAGS) $(builddir)/$(MODULE).lo -o $(builddir)/$(MODULE).la;)

install:
	mkdir -p $(libdir)/libfm/modules/
	$(foreach MODULE,$(MODULES),libtool --mode=install install -c $(builddir)/$(MODULE).la $(libdir)/libfm/modules/;)
	libtool --finish $(libdir)/libfm/modules/

