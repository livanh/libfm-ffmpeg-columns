/*
 * gtk-column-ffmpeg-resolution.c
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

FM_DEFINE_MODULE(gtk_folder_col, ffmpeg_resolution)

GType get_type() { return G_TYPE_STRING; };

typedef struct {
	int width;
	int height;
} t_resolution;

t_resolution get_resolution( FmFileInfo *fi );

void get_value( FmFileInfo *fi, GValue *value ){
	t_resolution resolution = get_resolution( fi );
	if ( resolution.width <= 0 || resolution.height <= 0 ) {
		return;
	}
	
	char *string = g_strdup_printf( "%dx%d", resolution.width, resolution.height );
	g_value_set_string( value, string );
};

gint sort( FmFileInfo *fi1, FmFileInfo *fi2 ){
	t_resolution resolution1 = get_resolution( fi1 );
	t_resolution resolution2 = get_resolution( fi2 );
	return resolution1.width*resolution1.height - resolution2.width*resolution2.height;
}

FmFolderModelColumnInit fm_module_init_gtk_folder_col = {
	"Resolution",
	0, 
	&get_type,
	&get_value,
	&sort
};

t_resolution get_resolution( FmFileInfo *fi ) {
	char *filename;
	t_resolution resolution = { 0, 0 };
	AVFormatContext *fmt_ctx = NULL;
	int vstream_index;
	AVStream *vstream;
	AVCodecParameters *vparam;
	int ret;
	
	av_register_all();
	
	filename = fm_path_to_str( fm_file_info_get_path( fi ) );
	ret = avformat_open_input( &fmt_ctx, filename, NULL, NULL );
	free( filename );
	
	if ( ret < 0 ) {
		avformat_close_input( &fmt_ctx );
		return resolution;
	}
	
	ret = avformat_find_stream_info( fmt_ctx, NULL );
	if ( ret < 0 ) {
		avformat_close_input( &fmt_ctx );
		return resolution;
	}
	
	if ( fmt_ctx->probe_score < 50 ) {
		avformat_close_input( &fmt_ctx );
		return resolution;
	}
	
	if ( strcmp( fmt_ctx->iformat->name, "tty" ) == 0 ) {
		avformat_close_input( &fmt_ctx );
		return resolution;
	}
	
	vstream_index = av_find_best_stream( fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0 );
	if ( vstream_index < 0 ) {
		avformat_close_input( &fmt_ctx );
		return resolution;
	}
	
	vstream = fmt_ctx->streams[vstream_index];
	vparam = vstream->codecpar;
	const AVCodecDescriptor *vcodec = avcodec_descriptor_get( vparam->codec_id );
	
	if (
		(strcmp( vcodec->name, "mjpeg" ) == 0 || strcmp( vcodec->name, "png" ) == 0) &&
		(strcmp( fmt_ctx->iformat->name, "mp3" ) == 0 || strcmp( fmt_ctx->iformat->name, "ogg" ) == 0)
	) {
		avformat_close_input( &fmt_ctx );
		return resolution;
	}
	
	resolution.width = vparam->width;
	resolution.height = vparam->height;
	avformat_close_input( &fmt_ctx );
	return resolution;
}
