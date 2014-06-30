/*
 * dss2_decoder
 *
 *  Created on: 03.06.2014
 *      Autor: lex
 */

#include "libavutil/common.h"
#include "libavutil/channel_layout.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "avcodec.h"
#include "internal.h"

#include "dss_sp_dec_data.h"

#define DSS_FORMULA(a, b, c)		(((a) * (b) + (c) * 32768) + 16384) / 32768

struct dss_sp_subframe {
	int16_t gain;
	int32_t combined_pulse_pos;
	int16_t pulse_pos[7]; /* this values was calculate */
	int16_t pulse_val[7]; /* this values was get directly from decompressor */
};

struct dsp_sp_frame {
	int16_t	filter_idx[14];

	int16_t sf_adaptive_gain[SUBFRAMES];
	int16_t pitch_lag[SUBFRAMES];

	struct dss_sp_subframe sf[SUBFRAMES];
};

struct lpc_data {
	int32_t filter[14];
};

typedef struct dss_sp_context {
    AVClass *class;
    int32_t excitation[288 + 6];
    int32_t history[187];
    struct dsp_sp_frame fparam;
    int32_t working_buffer[SUBFRAMES][72];
    int32_t audio_buf[15];
    int32_t err_buf1[15];
    struct lpc_data lpc;
    int32_t filter[15];
    int32_t vector_buf[72];
    int noise_state;
    int32_t err_buf2[15];

    int pulse_dec_mode;

} DSS_SP_Context;

static av_cold int dss_sp_decode_init(AVCodecContext *avctx)
{
    DSS_SP_Context *p = avctx->priv_data;
    avctx->channel_layout = AV_CH_LAYOUT_MONO;
    avctx->sample_fmt     = AV_SAMPLE_FMT_S16;
    avctx->channels       = 1;
    avctx->sample_rate    = 11025;

    memset(p->history, 0, sizeof(p->history));
    p->pulse_dec_mode = 1;

    return 0;
}

static void dss_sp_unpack_coeffs(DSS_SP_Context *p, const int16_t *src) {

    struct dsp_sp_frame *fparam = &p->fparam;
    int i;
    int subframe_idx;
    uint32_t v43;
    int v46;
    uint32_t v51;
    uint32_t v48;

    fparam->filter_idx[0] = (src[0] >> 11) & 0x1F;
    fparam->filter_idx[1] = (src[0] >> 6) & 0x1F;
    fparam->filter_idx[2] = (src[0] >> 2) & 0xF;
    fparam->filter_idx[3] = ((src[1] >> 14) & 3)
            + 4 * (src[0] & 3);

    fparam->filter_idx[4] = (src[1] >> 10) & 0xF;
    fparam->filter_idx[5] = (src[1] >> 6) & 0xF;
    fparam->filter_idx[6] = (src[1] >> 2) & 0xF;
    fparam->filter_idx[7] = ((src[2] >> 14) & 3)
            + 4 * (src[1] & 3);

    fparam->filter_idx[8] = (src[2] >> 11) & 7;
    fparam->filter_idx[9] = (src[2] >> 8) & 7;
    fparam->filter_idx[10] = (src[2] >> 5) & 7;
    fparam->filter_idx[11] = (src[2] >> 2) & 7;
    fparam->filter_idx[12] = ((src[3] >> 15) & 1)
            + 2 * (src[2] & 3);

    fparam->filter_idx[13] = (src[3] >> 12) & 7;

    fparam->sf_adaptive_gain[0] = (src[3] >> 7) & 0x1F;

    fparam->sf[0].combined_pulse_pos =
              (src[5] & 0xff00) >> 8
            | (src[4] & 0xffff) << 8
            | (src[3] & 0x7f) << 24;

    fparam->sf[0].gain = (src[5] >> 2) & 0x3F;

    fparam->sf[0].pulse_val[0] = ((src[6] >> 15) & 1)
            + 2 * (src[5] & 3);
    fparam->sf[0].pulse_val[1] = (src[6] >> 12) & 7;
    fparam->sf[0].pulse_val[2] = (src[6] >> 9) & 7;
    fparam->sf[0].pulse_val[3] = (src[6] >> 6) & 7;
    fparam->sf[0].pulse_val[4] = (src[6] >> 3) & 7;
    fparam->sf[0].pulse_val[5] = src[6] & 7;
    fparam->sf[0].pulse_val[6] = (src[7] >> 13) & 7;

    fparam->sf_adaptive_gain[1] = (src[7] >> 8) & 0x1F;

    fparam->sf[1].combined_pulse_pos =
              (src[9] & 0xfe00) >> 9
            | (src[8] & 0xffff) << 7
            | (src[7] & 0xff) << 23;

    fparam->sf[1].gain = (src[9] >> 3) & 0x3F;

    fparam->sf[1].pulse_val[0] = src[9] & 7;
    fparam->sf[1].pulse_val[1] = (src[10] >> 13) & 7;
    fparam->sf[1].pulse_val[2] = (src[10] >> 10) & 7;
    fparam->sf[1].pulse_val[3] = (src[10] >> 7) & 7;
    fparam->sf[1].pulse_val[4] = (src[10] >> 4) & 7;
    fparam->sf[1].pulse_val[5] = (src[10] >> 1) & 7;
    fparam->sf[1].pulse_val[6] = ((src[11] >> 14) & 3)
            + 4 * (src[10] & 1);

    fparam->sf_adaptive_gain[2] = (src[11] >> 9) & 0x1F;

    fparam->sf[2].combined_pulse_pos =
              (src[13] & 0xfc00) >> 10
            | (src[12] & 0xffff) << 6
            | (src[11] & 0x1ff) << 22;

    fparam->sf[2].gain = (src[13] >> 4) & 0x3F;

    fparam->sf[2].pulse_val[0] = (src[13] >> 1) & 7;
    fparam->sf[2].pulse_val[1] = ((src[14] >> 14) & 3)
            + 4 * (src[14] & 1);
    fparam->sf[2].pulse_val[2] = (src[14] >> 11) & 7;
    fparam->sf[2].pulse_val[3] = (src[14] >> 8) & 7;
    fparam->sf[2].pulse_val[4] = (src[14] >> 5) & 7;
    fparam->sf[2].pulse_val[5] = (src[14] >> 2) & 7;
    fparam->sf[2].pulse_val[6] = ((src[15] >> 15) & 1)
            + 2 * (src[14] & 3);

    fparam->sf_adaptive_gain[3] = (src[15] >> 10) & 0x1F;

    fparam->sf[3].combined_pulse_pos =
              (src[17] & 0xf800) >> 11
            | (src[16] & 0xffff) << 5
            | (src[15] & 0x3ff) << 21;

    fparam->sf[3].gain = (src[17] >> 5) & 0x3F;

    fparam->sf[3].pulse_val[0] = (src[17] >> 2) & 7;
    fparam->sf[3].pulse_val[1] = ((src[18] >> 15) & 1)
            + 2 * (src[17] & 3);
    fparam->sf[3].pulse_val[2] = (src[18] >> 12) & 7;
    fparam->sf[3].pulse_val[3] = (src[18] >> 9) & 7;
    fparam->sf[3].pulse_val[4] = (src[18] >> 6) & 7;
    fparam->sf[3].pulse_val[5] = (src[18] >> 3) & 7;
    fparam->sf[3].pulse_val[6] = src[18] & 7;

////////////////////////////////////////////////////////////////////
    for (subframe_idx = 0; subframe_idx < 4; subframe_idx++) {
        unsigned int C72_binomials[PULSE_MAX] = {
            72, 2556, 59640, 1028790, 13991544, 156238908,
            1473109704, 3379081753
        };
        unsigned int combined_pulse_pos =
                fparam->sf[subframe_idx].combined_pulse_pos;
        int index = 6;

        if (combined_pulse_pos < C72_binomials[PULSE_MAX - 1]) {
            if (p->pulse_dec_mode) {
                int pulse, pulse_idx;
                pulse = PULSE_MAX - 1;
                pulse_idx = 71; //GRID_SIZE
                combined_pulse_pos =
                    fparam->sf[subframe_idx].combined_pulse_pos;

                /* this part seems to be close to g723.1 gen_fcb_excitation() RATE_6300 */
                /* TODO: 7 is what? size of subframe? */
                for (i = 0; i < 7; i++) {
                    for (; combined_pulse_pos < dss_sp_combinatorial_table[pulse][pulse_idx];
                         --pulse_idx)
                        ;
                    combined_pulse_pos -=
                        dss_sp_combinatorial_table[pulse][pulse_idx];
                    pulse--;
                    fparam->sf[subframe_idx].pulse_pos[i] = pulse_idx;
                }
            }
        } else {
            p->pulse_dec_mode = 0;

            /* why do we need this? */
            fparam->sf[subframe_idx].pulse_pos[6] = 0;

            //////////////////
            for (i = 71; i >= 0; i--) {
                if (C72_binomials[index] <= combined_pulse_pos) {
                    combined_pulse_pos -= C72_binomials[index];

                    fparam->sf[subframe_idx].pulse_pos[(index ^ 7) - 1] = i;

                    if (!index)
                        break;
                    --index;
                }
                --C72_binomials[0];
                if (index) {
                    int a;
                    for (a = 0; a < index; a++)
                        C72_binomials[a + 1] -= C72_binomials[a];
                }
            }
        }
    }

/////////////////////////////////////////////////////////////////////////
    v43 = (src[19] & 0xffff) << 8;

    v46 = (v43 | ((src[20] & 0xff00) >> 8)) / 151;

    fparam->pitch_lag[0] =
            (v43 | ((src[20] & 0xff00) >> 8)) % 151 + 36;
    for (i = 1; i < SUBFRAMES; i++) {
        int v47 = v46;
        v46 /= 48;
        fparam->pitch_lag[i] = v47 - 48 * v46;
    }
////////////////////////////////////////////////////////////////////////
    v48 = fparam->pitch_lag[0];
    for (i = 1; i < SUBFRAMES; i++) {
        if (v48 > 162) {
            fparam->pitch_lag[i] += 162 - 23;
        } else {
            v51 = v48 - 23;
            if (v51 < 36)
                v51 = 36;
            fparam->pitch_lag[i] += v51;
        }
        v48 = fparam->pitch_lag[i];
    }
}

/* create stage 1 array14_stage0 based on stage0 and some kind of pulse table */
static void dss_sp_unpack_filter(DSS_SP_Context *p) {
    int i;

    for (i = 0; i < 14; i++)
        p->lpc.filter[i] = dss_sp_filter_cb[i][p->fparam.filter_idx[i]];
}

static void dss_sp_convert_coeffs(struct lpc_data *lpc,
        int32_t *coeffs) {
    int v3; // esi@1
    int v5; // ebx@2
    signed int v6; // eax@2
    int v7; // esi@3
    int v8; // edi@3

    int counter; // [sp+2Ch] [bp+8h]@2

    int tmp;

    coeffs[0] = 0x2000;
    for (v3 = 0; v3 < 14; v3++) {
        v5 = v3;
        v6 = v3 + 1;
        coeffs[v3 + 1] = lpc->filter[v3] >> 2;
        if (v6 / 2 >= 1) {
            for (counter = 1; counter <= v6/ 2; counter++) {
                v7 = coeffs[counter];
                // 4, 4, 4
                v8 = coeffs[1 + v5 - counter];
                // 8, 8, c
                tmp = DSS_FORMULA(lpc->filter[v5], v8, v7);
                coeffs[counter] = av_clip_int16(tmp);

                tmp = DSS_FORMULA(lpc->filter[v5], v7, v8);
                coeffs[1 + v5 - counter] = av_clip_int16(tmp);
            }            
        }
    }

    /////////////////////////////
#if 0 // executed only when there are overflows in the inner loop
    struct struc_6 *struc_6_v14 = struc_6_a1;
    int v24, v14 = 0;
    int word_3D9B7C = 1;
    coeffs[0] = 0x1000;
    int v26 = 0;
    struct struc_6 *struc_6_v29 = struc_6_a1;
    int32_t *array14a = coeffs;
    int v28 = 14;
    do
    {
        int v15 = v14 + 1;
        *array14a = struc_6_v14->filter[0] >> 3;
        counter = 1;
        int v30 = v14 + 1;
        int v29 = (v14 + 1) / 2;
        if ( (v14 + 1) / 2 >= 1 )
        {
            int v16 = 1;
            while ( 1 )
            {
                int v17 = v14 - v16;
                int v19 = coeffs[v17 + 1];

                tmp = DSS_FORMULA(struc_6_v14->filter[0], v19, coeffs[v16]);
                coeffs[v16] = tmp;
                tmp &= 0xFFFF8000;
                if ( tmp && tmp != 0xFFFF8000 )
                    coeffs[v16] = ((tmp <= 0) - 1) - 0x8000;

                struc_6_v14 = struc_6_v29;

                tmp = DSS_FORMULA(coeffs[v16], struc_6_v29->filter[0], v19);
                coeffs[v17] = v22;
                v22 &= 0xFFFF8000;
                if ( v22 && v22 != 0xFFFF8000 )
                {
                    if ( v22 <= 0 )
                        coeffs[v17] = 0xFFFF8000;
                    else
                        coeffs[v17] = 0x7FFF;
                }
                ++counter;
                v16 = counter;
                if ( counter > v29 )
                break;
                v14 = v26;
            }
            v15 = v30;
        }
        v14 = v15;
        struc_6_v14 = (struct struc_6 *)((char *)struc_6_v14 + 4);
        v24 = v28 == 1;
        v26 = v15;
        array14a++;
        struc_6_v29 = struc_6_v14;
        --v28;
    }
    while ( !v24 );
#endif
}

/* this function will get pointer to one of 4 subframes */
static void dss_sp_add_pulses(int32_t *array72_a1, const struct dss_sp_subframe *sf) {
    int i;

    //looks like "output[sf->pulse_pos[i]] += g_gains[sf->gain] * g_pulse_val[sf->pulse_val[i]] + 0x4000 >> 15;"
    for (i = 0; i < 7; i++)
        array72_a1[sf->pulse_pos[i]] += (dss_sp_fixed_cb_gain[sf->gain]
                * dss_sp_pulse_val[sf->pulse_val[i]] + 0x4000) >> 15;

}

static void dss_sp_gen_exc(int32_t *vector, int32_t *prev_exc, int pitch_lag, int gain) {

    int i;

    /* do we actually need this check? we can use just [a3 - i % a3] for both cases */
    if (pitch_lag < 72)
        for (i = 0; i < 72; i++)
            vector[i] = prev_exc[pitch_lag - i % pitch_lag];
    else
        for (i = 0; i < 72; i++)
            vector[i] = prev_exc[pitch_lag - i];

    for (i = 0; i < 72; i++) {
        int tmp = gain * vector[i] >> 11;
        vector[i] = av_clip_int16(tmp);
    }
}

static void dss_sp_scale_vector(int32_t *vec, int bits, int size) {
    int i;

    if (bits < 0)
        for (i = 0; i < size; i++)
            vec[i] = vec[i] >> -bits;
    else
        for (i = 0; i < size; i++)
            vec[i] = vec[i] << bits;
}

static void dss_sp_update_buf(int32_t *hist, int32_t *vector) {
    int i;

    for (i = 114; i > 0; i--)
        vector[i + 72] = vector[i];

    for (i = 0; i < 72; i++)
        vector[72 - i] = hist[i];
}

static void dss_sp_shift_sq_sub(const int32_t *array_a1,
        int32_t *array_a2, int32_t *dst) {
    int a;

    for (a = 0; a < 72; a++) {
        int i, tmp;

        tmp = dst[a] * array_a1[0];

        for (i = 14; i > 0; i--)
            tmp -= array_a2[i] * array_a1[i];

        /* original code overwrite array_a2[1] two times - makes no sense for me. */
        for (i = 14; i > 0; i--)
            array_a2[i] = array_a2[i - 1];

        tmp = (tmp + 4096) >> 13;

        array_a2[1] = tmp;

        dst[a] = av_clip_int16(tmp);
    }
}

static void dss_sp_shift_sq_add(const int32_t *array_a1, int32_t *array_a2,
        int32_t *dst) {
    int a;

    for (a = 0; a < 72; a++) {
        int i, tmp = 0;

        array_a2[0] = dst[a];

        for (i = 14; i >= 0; i--)
            tmp += array_a2[i] * array_a1[i];

        for (i = 14; i > 0; i--)
            array_a2[i] = array_a2[i - 1];

        tmp = (tmp + 4096) >> 13;

        dst[a] = av_clip_int16(tmp);
    }
}

static void dss_sp_vec_mult(const int32_t *array15_ro_src, int32_t *dst,
        const int32_t *array15_ro_a3) {
    int i;

    dst[0] = array15_ro_src[0];

    for (i = 1; i < 15; i++)
        dst[i] = (array15_ro_src[i] * array15_ro_a3[i] + 0x4000) >> 15;
}

static int dss_sp_get_normalize_bits(int32_t *array_var, int16_t size) {
    unsigned int val;
    int max_val;
    int i;

    val = 1;
    for (i = 0; i < size; i++)
        val |= FFABS(array_var[i]);

    for (max_val = 0; val <= 0x4000; ++max_val)
        val *= 2;
    return max_val;
}

static void dss_sp_sf_synthesis(DSS_SP_Context *p, int32_t a0,
        int32_t *dst, int size) {

    int32_t local_rw_array15_v1a[15];
    int32_t local_rw_array15_v39[15];
    int32_t noise[72];
    int v11, v22, bias, v18, v34, v36, normalize_bits;
    int i, tmp;

    v34 = 0;
    if (size > 0) {
        for (i = 0; i < size; i++)
            v34 += FFABS(p->vector_buf[i]);

        if (v34 > 0xFFFFF)
            v34 = 0xFFFFF;
    }

    normalize_bits = dss_sp_get_normalize_bits(p->vector_buf, size);

    dss_sp_scale_vector(p->vector_buf, normalize_bits - 3, size);
    dss_sp_scale_vector(p->audio_buf, normalize_bits, 15);
    dss_sp_scale_vector(p->err_buf1, normalize_bits, 15);

    v36 = p->err_buf1[1];

    dss_sp_vec_mult(p->filter, local_rw_array15_v39,
            binary_decreasing_array);
    dss_sp_shift_sq_add(local_rw_array15_v39, p->audio_buf,
            p->vector_buf);

    dss_sp_vec_mult(p->filter, local_rw_array15_v1a,
            dss_sp_unc_decreasing_array);
    dss_sp_shift_sq_sub(local_rw_array15_v1a,
            p->err_buf1, p->vector_buf);

    /* a0 can be negative */
    v11 = a0 >> 1;
    if (v11 >= 0)
        v11 = 0;

    if (size > 1) {
        for (i = size - 1; i > 0; i--) {
            tmp = DSS_FORMULA(v11, p->vector_buf[i - 1], p->vector_buf[i]);
            p->vector_buf[i] = av_clip_int16(tmp);
        }
    }

    tmp = DSS_FORMULA(v11, v36, p->vector_buf[0]);
    p->vector_buf[0] = av_clip_int16(tmp);

    dss_sp_scale_vector(p->vector_buf, -normalize_bits, size);
    dss_sp_scale_vector(p->audio_buf, -normalize_bits, 15);
    dss_sp_scale_vector(p->err_buf1, -normalize_bits, 15);

    v18 = 0;
    if (size > 0)
        for (i = 0; i < size; i++)
            v18 += FFABS(p->vector_buf[i]);

    if (v18 >= 0x40)
        v22 = (v34 << 11) / v18;
    else
        v22 = 1;

    bias = 409 * v22 >> 15 << 15;
    tmp = (bias + 32358 * p->noise_state) >> 15;
    noise[0] = av_clip_int16(tmp);

    for (i = 1; i < size; i++) {
        tmp = (bias + 32358 * noise[i - 1]) >> 15;
        noise[i] = av_clip_int16(tmp);
    }

    p->noise_state = noise[size - 1];
    for (i = 0; i < size; i++) {
        tmp = (p->vector_buf[i] * noise[i]) >> 11;
        dst[i] = av_clip_int16(tmp);
    }
}

static void dss_sp_update_state(DSS_SP_Context *p, int32_t *dst) {
    int v1;

    signed int v10;

    int tmp;
    int i, offset, counter;

    v1 = 0;

    for (i = 0; i < 6; i++)
        p->excitation[i] = p->excitation[288 + i];

    for (i = 0; i < 72 * SUBFRAMES; i++)
        p->excitation[6 + i] = dst[i];

    offset = 6;
    counter = 0;
    do {
        v10 = 0;
        for (i = 0; i < 6; i++)
            v10 += p->excitation[offset--]
                    * dss_sp_sinc[v1 + i * 11];

        offset += 7;

        tmp = v10 >> 15;
        dst[counter] = av_clip_int16(tmp);

        counter++;

        v1 = (v1 + 1) % 11;
        if (!v1)
            offset++;
    } while (offset < FF_ARRAY_ELEMS(p->excitation));
}

static void dss_sp_32to16bit(int16_t *dst, int32_t *src, int size) {
    int i;

    for (i = 0; i < size; i++)
        dst[i] = src[i];
}

static int dss_sp_decode_one_frame(DSS_SP_Context *p, int16_t *abuf_dst, const int8_t *abuff_src) {

    int i, tmp, sf_idx;

    dss_sp_unpack_coeffs(p, (const int16_t *)abuff_src); //todo proper bitstream reading

    dss_sp_unpack_filter(p);

    dss_sp_convert_coeffs(&p->lpc, p->filter);

////////
    for (sf_idx = 0; sf_idx < SUBFRAMES; sf_idx++) {

        dss_sp_gen_exc(p->vector_buf, p->history,
                p->fparam.pitch_lag[sf_idx],
                dss_sp_adaptive_gain[p->fparam.sf_adaptive_gain[sf_idx]]);

        dss_sp_add_pulses(p->vector_buf, &p->fparam.sf[sf_idx]);

        dss_sp_update_buf(p->vector_buf, p->history);

        /* swap and copy buffer */
        for (i = 0; i < 72; i++)
            p->vector_buf[i] = p->history[72 - i];

        dss_sp_shift_sq_sub(p->filter,
                p->err_buf2, p->vector_buf);

        dss_sp_sf_synthesis(p, p->lpc.filter[0],
                &p->working_buffer[sf_idx][0], 72);

    }
////////

    dss_sp_update_state(p, &p->working_buffer[0][0]);

    dss_sp_32to16bit(abuf_dst,
                    &p->working_buffer[0][0], 264);
    return 0;

}

static int dss_sp_decode_frame(AVCodecContext *avctx, void *data,
                               int *got_frame_ptr, AVPacket *avpkt)
{
    DSS_SP_Context *p  = avctx->priv_data;
    AVFrame *frame     = data;
    const uint8_t *buf = avpkt->data;
    int buf_size       = avpkt->size;

    int16_t *out;
    int ret;

    if (buf_size < DSS_SP_FRAME_SIZE) {
        if (buf_size)
            av_log(avctx, AV_LOG_WARNING,
                   "Expected %d bytes, got %d - skipping packet\n",
                   DSS_SP_FRAME_SIZE, buf_size);
        *got_frame_ptr = 0;
        return buf_size;
    }

    frame->nb_samples = DSS_SP_SAMPLE_COUNT;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0) {
         av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
         return ret;
    }

    out = (int16_t *)frame->data[0];


    /* process frame here */
    dss_sp_decode_one_frame(p, out, buf);

    *got_frame_ptr = 1;

    return DSS_SP_FRAME_SIZE;
}

static const AVClass dss_sp_dec_class = {
    .class_name = "DSS SP decoder",
    .item_name  = av_default_item_name,
    .version    = LIBAVUTIL_VERSION_INT,
};

AVCodec ff_dss_sp_decoder = {
    .name           = "dss_sp",
    .long_name      = NULL_IF_CONFIG_SMALL("DSS SP"),
    .type           = AVMEDIA_TYPE_AUDIO,
    .id             = AV_CODEC_ID_DSS_SP,
    .priv_data_size = sizeof(DSS_SP_Context),
    .init           = dss_sp_decode_init,
    .decode         = dss_sp_decode_frame,
    .priv_class     = &dss_sp_dec_class,
};
