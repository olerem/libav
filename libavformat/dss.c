/*
 * mtv demuxer
 * Copyright (c) 2006 Reynaldo H. Verdejo Pinochet
 *
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * MTV demuxer.
 */

#include "libavutil/bswap.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/attributes.h"
#include "libavutil/channel_layout.h"
#include "avformat.h"
#include "internal.h"

#define DSS_HEADER_SIZE 1024
#define DSS_BLOCK_SIZE 512
#define DSS_AUDIO_PADDING_SIZE 6
#define DSS_ASUBCHUNK_DATA_SIZE 506

static const uint8_t frame_size[4] = { 24, 20, 4, 1 };

typedef struct DSSDemuxContext {

    unsigned int file_size;         ///< filesize, not always right
    unsigned int segments;          ///< number of 512 byte segments
    unsigned int audio_identifier;  ///< 'MP3' on all files I have seen
    unsigned int audio_br;          ///< bitrate of audio channel (mp3)
    int counter;

} DSSDemuxContext;

static int dss_probe(AVProbeData *p)
{
    /* Magic is 'DSS' */
    if (*p->buf != 0x02 || *(p->buf + 1) != 'D' || *(p->buf + 2) != 'S' ||
            *(p->buf + 3) != 'S')
        return 0;

    return AVPROBE_SCORE_MAX;
}

static int dss_read_header(AVFormatContext *s)
{
    DSSDemuxContext *priv = s->priv_data;
    AVStream        *st;
    AVIOContext     *pb  = s->pb;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codec->codec_type     = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id       = AV_CODEC_ID_G723_1;
    st->codec->channel_layout = AV_CH_LAYOUT_MONO;
    st->codec->channels       = 1;
    st->codec->sample_rate    = 8000;

    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);
    st->start_time = 0;

    // Jump over header

    if(avio_seek(pb, DSS_HEADER_SIZE, SEEK_SET) != DSS_HEADER_SIZE)
        return AVERROR(EIO);

    priv->counter = 0;

    return 0;

}

static int dss_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    DSSDemuxContext *priv = s->priv_data;
    AVIOContext *pb = s->pb;
    int size, byte, ret, offset;

    if (priv->counter == 0) {
        avio_skip(pb, DSS_AUDIO_PADDING_SIZE);
        priv->counter = DSS_BLOCK_SIZE - DSS_AUDIO_PADDING_SIZE;
    }

    pkt->pos = avio_tell(s->pb);
    byte     = avio_r8(s->pb);
    size     = frame_size[byte & 3];
    priv->counter -= size;

    ret = av_new_packet(pkt, size);
    if (ret < 0)
        return ret;

    pkt->data[0]      = byte;
    offset = 1;
    pkt->duration     = 240;
    pkt->stream_index = 0;

    if (priv->counter < 0) {
        int size2 = priv->counter + size;
        ret = avio_read(s->pb, pkt->data + offset,
            size2 - offset);
        if (ret < size2 - offset) {
            av_free_packet(pkt);
            return ret < 0 ? ret : AVERROR_EOF;
        }
        priv->counter += DSS_BLOCK_SIZE - DSS_AUDIO_PADDING_SIZE;
        avio_skip(pb, DSS_AUDIO_PADDING_SIZE);
        offset = size2;

    }

    ret = avio_read(s->pb, pkt->data + offset, size - offset);
    if (ret < size - offset) {
        av_free_packet(pkt);
        return ret < 0 ? ret : AVERROR_EOF;
    }

    return pkt->size;
}

AVInputFormat ff_dss_demuxer = {
    .name           = "dss",
    .long_name      = NULL_IF_CONFIG_SMALL("DSS"),
    .priv_data_size = sizeof(DSSDemuxContext),
    .read_probe     = dss_probe,
    .read_header    = dss_read_header,
    .read_packet    = dss_read_packet,
    .extensions     = "dss"
};
