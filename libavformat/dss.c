/*
 * dss demuxer
 * Copyright (c) 2014 Oleksij Rempel
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

#define DSS_HEAD_OFFSET_AUTHOR        0xc
#define DSS_AUTHOR_SIZE               16

#define DSS_HEAD_OFFSET_START_TIME    0x26
#define DSS_HEAD_OFFSET_END_TIME      0x32
#define DSS_TIME_SIZE                 12

#define DSS_HEAD_OFFSET_ACODEC        0x2a4
#define DSS_ACODEC_0                  0x0    /* SP mode */
#define DSS_ACODEC_G723_1             0x2    /* LP mode */

#define DSS_HEAD_OFFSET_COMMENT       0x31e
#define DSS_COMMENT_SIZE              64

#define DSS_BLOCK_SIZE                512
#define DSS_HEADER_SIZE               (DSS_BLOCK_SIZE * 2)
#define DSS_AUDIO_BLOCK_HEADER_SIZE   6

static const uint8_t frame_size[4] = { 24, 20, 4, 1 };

typedef struct DSSDemuxContext {

    unsigned int audio_codec;
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


static int dss_read_methadata_date(AVFormatContext *s, unsigned int offset,
           const char *key)
{
    AVIOContext     *pb  = s->pb;
    char            string[DSS_TIME_SIZE], datetime[64];
    int             y, month, d, h, minute, sec;
    int             ret;

    avio_seek(pb, offset, SEEK_SET);

    ret = avio_read(s->pb, string, DSS_TIME_SIZE);
    if (ret < DSS_TIME_SIZE)
        return ret < 0 ? ret : AVERROR_EOF;

    sscanf(string, "%2d%2d%2d%2d%2d%2d", &y, &month, &d, &h, &minute, &sec);
    /* We deal here with two digit year, so set default date to 2000
     * and hope it will never be used in next century
     */
    snprintf(datetime, sizeof(datetime), "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
            y + 2000, month, d, h, minute, sec);
    av_dict_set(&s->metadata, key, datetime, 0);

    return ret;
}

static int dss_read_methadata_string(AVFormatContext *s, unsigned int offset,
           unsigned int size, const char *key)
{
    AVIOContext     *pb  = s->pb;
    char            *value;
    int             ret;

    avio_seek(pb, offset, SEEK_SET);

    value = av_malloc(size);
    if (!value)
        return AVERROR(ENOMEM);
    /*make sure, string will end with \0 */
    *(value + size) = '\0';

    ret = avio_read(s->pb, value, size);
    av_dict_set(&s->metadata, key, value, 0);

    av_free(value);
    if (ret < size)
        return ret < 0 ? ret : AVERROR_EOF;

    return ret;
}

static int dss_read_header(AVFormatContext *s)
{
    DSSDemuxContext *priv = s->priv_data;
    AVStream        *st;
    AVIOContext     *pb  = s->pb;

    if (avio_size(pb) <= DSS_HEADER_SIZE)
        return AVERROR(EINVAL);

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    dss_read_methadata_string(s, DSS_HEAD_OFFSET_AUTHOR,
            DSS_AUTHOR_SIZE, "author");
    dss_read_methadata_date(s, DSS_HEAD_OFFSET_END_TIME, "date");

    dss_read_methadata_string(s, DSS_HEAD_OFFSET_COMMENT,
            DSS_COMMENT_SIZE, "comment");

    avio_seek(pb, DSS_HEAD_OFFSET_ACODEC, SEEK_SET);
    priv->audio_codec = avio_r8(pb);

    if (priv->audio_codec == DSS_ACODEC_0) {
        printf("Not supported codec. DSS_ACODEC_0.\n");
        return AVERROR(EINVAL);
    } else if (priv->audio_codec == DSS_ACODEC_G723_1) {
        st->codec->codec_id       = AV_CODEC_ID_G723_1;
    } else {
        printf("Not supported codec: %x.\n", priv->audio_codec);
        return AVERROR(EINVAL);
    }

    st->codec->codec_type     = AVMEDIA_TYPE_AUDIO;
    st->codec->channel_layout = AV_CH_LAYOUT_MONO;
    st->codec->channels       = 1;
    st->codec->sample_rate    = 8000;

    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);
    st->start_time = 0;

    /* Jump over header */

    if(avio_seek(pb, DSS_HEADER_SIZE, SEEK_SET) != DSS_HEADER_SIZE)
        return AVERROR(EIO);

    priv->counter = 0;

    return 0;

}

static void dss_skip_audio_header(AVFormatContext *s, AVPacket *pkt)
{
    DSSDemuxContext *priv = s->priv_data;
    AVIOContext *pb = s->pb;

    avio_skip(pb, DSS_AUDIO_BLOCK_HEADER_SIZE);
    priv->counter += DSS_BLOCK_SIZE - DSS_AUDIO_BLOCK_HEADER_SIZE;
}

static int dss_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    DSSDemuxContext *priv = s->priv_data;
    int size, byte, ret, offset;

    if (priv->counter == 0)
        dss_skip_audio_header(s, pkt);

    pkt->pos = avio_tell(s->pb);
    /* We make here one byte step.
     * Don't forget to add offset.
     */
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

        dss_skip_audio_header(s, pkt);
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
