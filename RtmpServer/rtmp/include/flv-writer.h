#ifndef _flv_writer_h_
#define _flv_writer_h_

#include <stddef.h>
#include "stdint.h"

void* flv_writer_create(const char* file);

void flv_writer_destroy(void* flv);

///Video: FLV VideoTagHeader + AVCVIDEOPACKET: AVCDecoderConfigurationRecord(ISO 14496-15) / One or more NALUs(four-bytes length + NALU)
///Audio: FLV AudioTagHeader + AACAUDIODATA: AudioSpecificConfig(14496-3) / Raw AAC frame data in UI8
///@param[in] data FLV Audio/Video Data(don't include FLV Tag Header)
///@param[in] type 8-audio, 9-video
int flv_writer_input(void* flv, int type, const void* data, size_t bytes, uint32_t timestamp);

#endif /* !_flv_writer_h_ */
