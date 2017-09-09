/*
 * gtk-column-ffmpeg-audio-channels.c
 *
 * Copyright (C) 2017 Livanh <livanh@bulletmail.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdlib.h>
#include <string.h>
#include <libfm/fm.h>
#include <libfm/fm-gtk.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

FM_DEFINE_MODULE(gtk_folder_col, ffmpeg_audio_channels)

GType get_type() { return G_TYPE_STRING; };

typedef struct {
	int n_channels;
	uint64_t channel_layout;
} t_channels;

t_channels get_channels( FmFileInfo *fi );

void get_value( FmFileInfo *fi, GValue *value ) {
	t_channels channels = get_channels( fi );
	if ( channels.n_channels <= 0 ) {
		return;
	}
	
	char channels_string[15];
	av_get_channel_layout_string( channels_string, 15, channels.n_channels, channels.channel_layout );
	g_value_set_string( value, channels_string );
};

gint sort(FmFileInfo *fi1, FmFileInfo *fi2) {
	t_channels channels1 = get_channels( fi1 );
	t_channels channels2 = get_channels( fi2 );
	return channels1.n_channels - channels2.n_channels;
}

FmFolderModelColumnInit fm_module_init_gtk_folder_col = {
	"Audio channels",
	0, 
	&get_type,
	&get_value,
	&sort
};

t_channels get_channels( FmFileInfo *fi ) {
	char *filename;
	t_channels channels = { 0, 0 };
	AVFormatContext *fmt_ctx = NULL;
	int astream_index;
	AVStream *astream;
	AVCodecParameters *aparam;
	int ret;
	
	av_register_all();
	
	filename = fm_path_to_str( fm_file_info_get_path(fi) );
	ret = avformat_open_input( &fmt_ctx, filename, NULL, NULL );
	free( filename );
	
	if ( ret < 0 ) {
		avformat_close_input( &fmt_ctx );
		return channels;
	}
	
	ret = avformat_find_stream_info( fmt_ctx, NULL );
	if ( ret < 0 ) {
		avformat_close_input( &fmt_ctx );
		return channels;
	}
	
	if ( fmt_ctx->probe_score < 50 ) {
		avformat_close_input( &fmt_ctx );
		return channels;
	}
	
	astream_index = av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0 );
	if ( astream_index < 0 ) {
		avformat_close_input( &fmt_ctx );
		return channels;
	}
	
	astream = fmt_ctx->streams[astream_index];
	aparam = astream->codecpar;
	
	channels.n_channels = aparam->channels;
	channels.channel_layout = aparam->channel_layout;
	avformat_close_input( &fmt_ctx );
	return channels;
}
