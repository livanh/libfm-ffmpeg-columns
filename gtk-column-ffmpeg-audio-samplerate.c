/*
 * gtk-column-ffmpeg-audio-samplerate.c
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

FM_DEFINE_MODULE(gtk_folder_col, ffmpeg_audio_samplerate)

GType get_type() { return G_TYPE_STRING; };

int get_audio_samplerate( FmFileInfo *fi );

void get_value( FmFileInfo *fi, GValue *value ) {
	int samplerate = get_audio_samplerate( fi );
	if ( samplerate <= 0 ) {
		return;
	}
	
	char *string = g_strdup_printf( "%d khz", samplerate/1000 );
	g_value_set_string( value, string );
};

gint sort( FmFileInfo *fi1, FmFileInfo *fi2 ){
	int samplerate1 = get_audio_samplerate( fi1 );
	int samplerate2 = get_audio_samplerate( fi2 );
	return samplerate1 - samplerate2;
}

FmFolderModelColumnInit fm_module_init_gtk_folder_col = {
	"Sample rate",
	0, 
	&get_type,
	&get_value,
	&sort
};

int get_audio_samplerate( FmFileInfo *fi ) {
	char *filename;
	int samplerate = 0;
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
		return 0;
	}
	
	ret = avformat_find_stream_info( fmt_ctx, NULL );
	if ( ret < 0 ) {
		avformat_close_input( &fmt_ctx );
		return 0;
	}
	
	if ( fmt_ctx->probe_score < 50 ) {
		avformat_close_input( &fmt_ctx );
		return 0;
	}
	
	astream_index = av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0 );
	if ( astream_index < 0 ) {
		avformat_close_input( &fmt_ctx );
		return 0;
	}
	
	astream = fmt_ctx->streams[astream_index];
	aparam = astream->codecpar;
	
	samplerate = aparam->sample_rate;
	avformat_close_input( &fmt_ctx );
	return samplerate;
}
