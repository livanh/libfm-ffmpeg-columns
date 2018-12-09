/*
 * gtk-column-ffmpeg-duration.c
 *
 * Copyright (C) 2017 Livanh <livanh@protonmail.com>
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

FM_DEFINE_MODULE(gtk_folder_col, ffmpeg_duration)

GType get_type() { return G_TYPE_STRING; };

float get_duration( FmFileInfo *fi );

void get_value( FmFileInfo *fi, GValue *value ) {
	int duration = (int)get_duration( fi );
	if ( duration <= 0 ) {
		return;
	}
	
	char *string;
	if ( duration > 3600 ) {
		string = g_strdup_printf( "%d:%02d:%02d", duration/3600, (duration%3600)/60, duration%60 );
	} else {
		string = g_strdup_printf( "%02d:%02d", duration/60, duration%60 );
	}
	g_value_set_string( value, string );
};

gint sort(FmFileInfo *fi1, FmFileInfo *fi2) {
	float duration1 = get_duration( fi1 );
	float duration2 = get_duration( fi2 );
	return (int)(duration1*1000) - (int)(duration2*1000);
}

FmFolderModelColumnInit fm_module_init_gtk_folder_col = {
	"Duration",
	0, 
	&get_type,
	&get_value,
	&sort
};

float get_duration( FmFileInfo *fi ) {
	char *filename;
	float duration = 0;
	AVFormatContext *fmt_ctx = NULL;
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
	
	if (
		strcmp( fmt_ctx->iformat->name, "image2" ) == 0 ||
		strcmp( fmt_ctx->iformat->name, "png_pipe" ) == 0 ||
		strcmp( fmt_ctx->iformat->name, "gif" ) == 0 ||
		strcmp( fmt_ctx->iformat->name, "tty" ) == 0
	) {
		avformat_close_input( &fmt_ctx );
		return 0;
	}
	
	duration = fmt_ctx->duration / (float)AV_TIME_BASE;
	avformat_close_input( &fmt_ctx );
	return duration;
}
