#include <pestacle/memory.h>

#include <SDL_log.h>
#include <SDL_video.h>
#include <SDL_render.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

#include "load.h"


// --- Interface --------------------------------------------------------------

static bool
node_setup(
	Node* self
);


static void
node_destroy(
	Node* self
);


static void
node_update(
	Node* self
);


static NodeOutput
node_output(
	const Node* self
);


static const NodeInputDefinition
node_inputs[] = {
	NODE_INPUT_DEFINITION_END
};


#define PATH_PARAMETER 0

static const ParameterDefinition
node_parameters[] = {
	{
		ParameterType__string,
		"path",
		{ .string_value = "" }
	},
	PARAMETER_DEFINITION_END
};


const NodeDelegate
ffmpeg_load_node_delegate = {
	"load",
	true,
	node_inputs,
	node_parameters,
	{
		node_setup,
		node_destroy,
		node_update,
		node_output
	},
};


// --- Implementation ---------------------------------------------------------

typedef struct {
	AVFormatContext* format_ctx;
	AVCodecParameters* params;
	int video_id;
	AVCodecContext* codec_ctx;
	const AVCodec* codec;
	AVFrame* frame;
	AVPacket* packet;

	SDL_Surface* surface;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
} InputStreamData;


static bool
InputStreamData_init(
	InputStreamData* self,
	const char* path
) {
	self->format_ctx = 0;
	self->params = 0;
	self->video_id = 0;
	self->codec_ctx = 0;
	self->frame = 0;
	self->packet = 0;
	self->surface = 0;
	self->renderer = 0;
	self->texture = 0;

	// Allocate format context
	self->format_ctx = avformat_alloc_context();

	// Open the video file
	if (avformat_open_input(&(self->format_ctx), path, NULL, NULL) != 0) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"could not open video stream '%s'\n",
			path
		);
		return false;
	}

	// Retrieve stream information
	if (avformat_find_stream_info(self->format_ctx, 0) < 0) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"could not retrieve informations from video stream '%s'\n",
			path
		);
		return false;
	}

	// Find coded
	bool found_video = false;
	for (unsigned int i = 0; i < self->format_ctx->nb_streams; i++) {
		AVCodecParameters* localparam = self->format_ctx->streams[i]->codecpar;
		const AVCodec* localcodec = avcodec_find_decoder(localparam->codec_id);
		if (localparam->codec_type == AVMEDIA_TYPE_VIDEO && !found_video) {
			self->codec = localcodec;
			self->params = localparam;
			self->video_id = i;
			/*
			AVRational rational = pFormatCtx->streams[i]->avg_frame_rate;
			fpsrendering = 1.0 / ((double)rational.num / (double)(rational.den));
			*/
			found_video = true;
			break;
		}
	}

	if (!found_video) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"could not find video codec from video stream '%s'\n",
			path
		);
		return false;
	}

	self->codec_ctx = avcodec_alloc_context3(self->codec);

	// Dafuck
	if (avcodec_parameters_to_context(self->codec_ctx, self->params) < 0) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"dafuck from video stream '%s'\n",
			path
		);
		return false;
	}

	// Wat
	if (avcodec_open2(self->codec_ctx, self->codec, NULL) < 0) {
		SDL_LogError(
			SDL_LOG_CATEGORY_SYSTEM,
			"wat from video stream '%s'\n",
			path
		);
		return false;
	}

	// Frame allocation
	self->frame = av_frame_alloc();

	// Packet allocation
	self->packet = av_packet_alloc();

	// Allocate SDL surface
	self->surface =
		SDL_CreateRGBSurfaceWithFormat(
			0,
			self->params->width,
			self->params->height,
			32,
			SDL_PIXELFORMAT_RGBA32
		);

	if (!self->surface) {
		SDL_LogError(
			SDL_LOG_CATEGORY_VIDEO,
			"Could not create SDL surface : %s\n",
			SDL_GetError()
		);
		return false;
	}

	// Create renderer
	self->renderer = SDL_CreateSoftwareRenderer(self->surface);
	if (!self->renderer) {
		SDL_LogError(
			SDL_LOG_CATEGORY_VIDEO,
			"Could not create SDL renderer : %s\n",
			SDL_GetError()
		);
		return false;
	}

	// Create texture
	self->texture = SDL_CreateTexture(
		self->renderer,
		SDL_PIXELFORMAT_IYUV,
		SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET,
		self->params->width,
		self->params->height
	);
	
	if (!self->texture) {
		SDL_LogError(
			SDL_LOG_CATEGORY_VIDEO,
			"Could not create SDL texture : %s\n",
			SDL_GetError()
		);
		return false;
	}

	// Job done
	return true;
}


static void
InputStreamData_destroy(
	InputStreamData* self
) {
	if (self->packet)
    	av_packet_free(&(self->packet));

	if (self->frame)
		av_frame_free(&(self->frame));

	if (self->codec_ctx)
		avcodec_free_context(&(self->codec_ctx));

	if (self->format_ctx) {
		avformat_close_input(&(self->format_ctx));
		avformat_free_context(self->format_ctx);
    }

	if (self->texture)
		SDL_DestroyTexture(self->texture);

	if (self->renderer)
		SDL_DestroyRenderer(self->renderer);

	if (self->surface)
		SDL_FreeSurface(self->surface);
}


static void
InputStreamData_update(
	InputStreamData* self
) {
	// Attempt to read one frame
	int ret = av_read_frame(self->format_ctx, self->packet);

	// If the end of the video is reached, rewind
	if (ret < 0) {
		if (av_seek_frame(self->format_ctx, self->video_id, self->format_ctx->start_time, AVSEEK_FLAG_BACKWARD) < 0)
			return;

		avcodec_flush_buffers(self->codec_ctx);
	}

	// Decode the frame
	if (ret >= 0) {
		if (self->packet->stream_index == self->video_id) {
			ret = avcodec_send_packet(self->codec_ctx, self->packet);
			
			if (ret < 0) {
				SDL_LogError(
					SDL_LOG_CATEGORY_VIDEO,
					"avcodec_send_packet error\n"
				);
				return;
			}

			ret = avcodec_receive_frame(self->codec_ctx, self->frame);
			if (ret < 0) {
				if (ret == AVERROR(EINVAL))
					SDL_LogError(
						SDL_LOG_CATEGORY_VIDEO,
						"avcodec_receive_frame error : codec not opened\n"
					);
				else if (ret == AVERROR(EAGAIN))
					SDL_LogError(
						SDL_LOG_CATEGORY_VIDEO,
						"avcodec_receive_frame error : output not available\n"
					);
				else
					SDL_LogError(
						SDL_LOG_CATEGORY_VIDEO,
						"avcodec_receive_frame error\n"
					);
				return;
			}

			SDL_UpdateYUVTexture(
				self->texture,
				0,
				self->frame->data[0], self->frame->linesize[0],
				self->frame->data[1], self->frame->linesize[1],
				self->frame->data[2], self->frame->linesize[2]
			);
			SDL_RenderClear(self->renderer);
			SDL_RenderCopy(self->renderer, self->texture, 0, 0);
			SDL_RenderPresent(self->renderer);
		}

		av_packet_unref(self->packet);
	}
}


static bool
node_setup(
	Node* self
) {
	// Retrieve path to video file
	const char* path = self->parameters[PATH_PARAMETER].string_value;

	// Allocate data
	InputStreamData* data = (InputStreamData*)checked_malloc(sizeof(InputStreamData));
	if (!data)
		return false;

	// Initialise data
	if (!InputStreamData_init(data, path)) {
		free(data);
		return false;
	}

	// Setup node type metadata
	self->type = NodeType__rgb_surface;
	self->type_metadata.matrix.width = data->params->width;
	self->type_metadata.matrix.height = data->params->height;

	// Job done
	self->data = data;
	return true;
}


static void
node_destroy(
	Node* self
) {
	InputStreamData* data = (InputStreamData*)self->data;
	if (data) {
		InputStreamData_destroy(data);
		free(data);
	}
}


static void
node_update(
	Node* self
) {
	InputStreamData* data = (InputStreamData*)self->data;
	InputStreamData_update(data);
}


static NodeOutput
node_output(
	const Node* self
) {
	InputStreamData* data = (InputStreamData*)self->data;
	NodeOutput ret = { .rgb_surface = data->surface };
	return ret;
}
