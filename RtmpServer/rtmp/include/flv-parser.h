#ifndef _flv_parser_h_
#define _flv_parser_h_

#include "stdint.h"
#include <stddef.h>

typedef struct flv_parser_t flv_parser_t;

/// Audio/Video Elementary Stream
/// @param[in] param user-defined parameter
/// @param[in] codec audio/video format (see more flv-proto.h)
/// @param[in] data audio/video element data, AAC: AAC-Frame, H.264: MP4 Stream, MP3-Raw data
/// @param[in] bytes data length in byte
/// @param[in] pts audio/video presentation timestamp
/// @param[in] dts audio/video decoding timestamp
/// @param[in] flags 1-video keyframe, other-undefined
/// @return 0-ok, other-error
typedef int(*flv_parser_handler)(void* param, int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags);

flv_parser_t* flv_parser_create(flv_parser_handler handler, void* param);
void flv_parser_destroy(flv_parser_t* parser);

/// Input FLV Audio/Video Stream
/// @param[in] type 8-audio, 9-video, 18-script (see more flv-proto.h)
/// @param[in] data flv audio/video Stream, AudioTagHeader/VideoTagHeader + A/V Data
/// @param[in] bytes data length in byte
/// @param[in] timestamp milliseconds relative to the first tag(DTS)
/// @return 0-ok, other-error
int flv_parser_input(flv_parser_t* parser, int type, const void* data, size_t bytes, uint32_t timestamp);

#endif /* !_flv_parser_h_ */
