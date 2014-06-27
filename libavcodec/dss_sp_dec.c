/*
 * dss2_decoder
 *
 *  Created on: 03.06.2014
 *      Autor: lex
 */

#define BITSTREAM_READER_LE
#include "libavutil/channel_layout.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "avcodec.h"
#include "get_bits.h"
#include "acelp_vectors.h"
#include "celp_filters.h"
#include "internal.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "dss_sp_dec_data.h"

typedef struct dss_sp_context {
    AVClass *class;
    int32_t g_unc_rw_array288_3D0DC0[288 + 6];
    int32_t g_unc_rw_arrayXX_3D08FC[187];
	struct dss_sp_frameparam fparam;
	int32_t working_buffer[SUBFRAMES][72];
	int32_t g_unc_rw_array15_3D0420[15];
	int32_t g_unc_rw_array15_3D045C[15];
	struct struc_6 g_unc_rw_array14_stg1_3D0D64;
	int32_t g_unc_rw_array15_stg2_3D08C0[15];
	int32_t g_unc_rw_array72_3D0C44[72];
	int dword_3D0498;
	int dword_3D0DA0;
	int32_t g_unc_rw_array15_3D0BE8[15];

    int pulse_dec_mode;

} DSS_SP_Context;

static av_cold int dss_sp_decode_init(AVCodecContext *avctx)
{
    DSS_SP_Context *p = avctx->priv_data;
    avctx->channel_layout = AV_CH_LAYOUT_MONO;
    avctx->sample_fmt     = AV_SAMPLE_FMT_S16;
    avctx->channels       = 1;
    avctx->sample_rate    = 12000;

    memset(p->g_unc_rw_arrayXX_3D08FC, 0, sizeof(p->g_unc_rw_arrayXX_3D08FC));
    p->pulse_dec_mode = 1;

    return 0;
}

static void dss_sp_unpack_coeffs(DSS_SP_Context *p, const int16_t *compressed_buf) {

	struct dss_sp_frameparam *fparam = &p->fparam;
	int i;
	int subframe_idx;
	uint32_t v43;
	int v46;
	uint32_t v51;
	uint32_t v48;

	fparam->codebook_indices[0] = (compressed_buf[0] >> 11) & 0x1F;
	fparam->codebook_indices[1] = (compressed_buf[0] >> 6) & 0x1F;
	fparam->codebook_indices[2] = (compressed_buf[0] >> 2) & 0xF;
	fparam->codebook_indices[3] = ((compressed_buf[1] >> 14) & 3)
			+ 4 * (compressed_buf[0] & 3);

	fparam->codebook_indices[4] = (compressed_buf[1] >> 10) & 0xF;
	fparam->codebook_indices[5] = (compressed_buf[1] >> 6) & 0xF;
	fparam->codebook_indices[6] = (compressed_buf[1] >> 2) & 0xF;
	fparam->codebook_indices[7] = ((compressed_buf[2] >> 14) & 3)
			+ 4 * (compressed_buf[1] & 3);

	fparam->codebook_indices[8] = (compressed_buf[2] >> 11) & 7;
	fparam->codebook_indices[9] = (compressed_buf[2] >> 8) & 7;
	fparam->codebook_indices[10] = (compressed_buf[2] >> 5) & 7;
	fparam->codebook_indices[11] = (compressed_buf[2] >> 2) & 7;
	fparam->codebook_indices[12] = ((compressed_buf[3] >> 15) & 1)
			+ 2 * (compressed_buf[2] & 3);

	fparam->codebook_indices[13] = (compressed_buf[3] >> 12) & 7;

	fparam->subframe_something[0] = (compressed_buf[3] >> 7) & 0x1F;

	fparam->sf[0].combined_pulse_pos =
			  (compressed_buf[5] & 0xff00) >> 8
			| (compressed_buf[4] & 0xffff) << 8
			| (compressed_buf[3] & 0x7f) << 24;

	fparam->sf[0].gain = (compressed_buf[5] >> 2) & 0x3F;

	fparam->sf[0].pulse_val[0] = ((compressed_buf[6] >> 15) & 1)
			+ 2 * (compressed_buf[5] & 3);
	fparam->sf[0].pulse_val[1] = (compressed_buf[6] >> 12) & 7;
	fparam->sf[0].pulse_val[2] = (compressed_buf[6] >> 9) & 7;
	fparam->sf[0].pulse_val[3] = (compressed_buf[6] >> 6) & 7;
	fparam->sf[0].pulse_val[4] = (compressed_buf[6] >> 3) & 7;
	fparam->sf[0].pulse_val[5] = compressed_buf[6] & 7;
	fparam->sf[0].pulse_val[6] = (compressed_buf[7] >> 13) & 7;

	fparam->subframe_something[1] = (compressed_buf[7] >> 8) & 0x1F;

	fparam->sf[1].combined_pulse_pos =
			  (compressed_buf[9] & 0xfe00) >> 9
			| (compressed_buf[8] & 0xffff) << 7
			| (compressed_buf[7] & 0xff) << 23;

	fparam->sf[1].gain = (compressed_buf[9] >> 3) & 0x3F;

	fparam->sf[1].pulse_val[0] = compressed_buf[9] & 7;
	fparam->sf[1].pulse_val[1] = (compressed_buf[10] >> 13) & 7;
	fparam->sf[1].pulse_val[2] = (compressed_buf[10] >> 10) & 7;
	fparam->sf[1].pulse_val[3] = (compressed_buf[10] >> 7) & 7;
	fparam->sf[1].pulse_val[4] = (compressed_buf[10] >> 4) & 7;
	fparam->sf[1].pulse_val[5] = (compressed_buf[10] >> 1) & 7;
	fparam->sf[1].pulse_val[6] = ((compressed_buf[11] >> 14) & 3)
			+ 4 * (compressed_buf[10] & 1);

	fparam->subframe_something[2] = (compressed_buf[11] >> 9) & 0x1F;

	fparam->sf[2].combined_pulse_pos =
			  (compressed_buf[13] & 0xfc00) >> 10
			| (compressed_buf[12] & 0xffff) << 6
			| (compressed_buf[11] & 0x1ff) << 22;

	fparam->sf[2].gain = (compressed_buf[13] >> 4) & 0x3F;

	fparam->sf[2].pulse_val[0] = (compressed_buf[13] >> 1) & 7;
	fparam->sf[2].pulse_val[1] = ((compressed_buf[14] >> 14) & 3)
			+ 4 * (compressed_buf[14] & 1);
	fparam->sf[2].pulse_val[2] = (compressed_buf[14] >> 11) & 7;
	fparam->sf[2].pulse_val[3] = (compressed_buf[14] >> 8) & 7;
	fparam->sf[2].pulse_val[4] = (compressed_buf[14] >> 5) & 7;
	fparam->sf[2].pulse_val[5] = (compressed_buf[14] >> 2) & 7;
	fparam->sf[2].pulse_val[6] = ((compressed_buf[15] >> 15) & 1)
			+ 2 * (compressed_buf[14] & 3);

	fparam->subframe_something[3] = (compressed_buf[15] >> 10) & 0x1F;

	fparam->sf[3].combined_pulse_pos =
			  (compressed_buf[17] & 0xf800) >> 11
			| (compressed_buf[16] & 0xffff) << 5
			| (compressed_buf[15] & 0x3ff) << 21;

	fparam->sf[3].gain = (compressed_buf[17] >> 5) & 0x3F;

	fparam->sf[3].pulse_val[0] = (compressed_buf[17] >> 2) & 7;
	fparam->sf[3].pulse_val[1] = ((compressed_buf[18] >> 15) & 1)
			+ 2 * (compressed_buf[17] & 3);
	fparam->sf[3].pulse_val[2] = (compressed_buf[18] >> 12) & 7;
	fparam->sf[3].pulse_val[3] = (compressed_buf[18] >> 9) & 7;
	fparam->sf[3].pulse_val[4] = (compressed_buf[18] >> 6) & 7;
	fparam->sf[3].pulse_val[5] = (compressed_buf[18] >> 3) & 7;
	fparam->sf[3].pulse_val[6] = compressed_buf[18] & 7;

////////////////////////////////////////////////////////////////////
	for (subframe_idx = 0; subframe_idx < 4; subframe_idx++) {
		unsigned int C72_binomials[PULSE_MAX] = { 72, 2556, 59640, 1028790,
				13991544, 156238908, 1473109704, 3379081753 };
		unsigned int combined_pulse_pos =
				fparam->sf[subframe_idx].combined_pulse_pos;
		int index = 6;

		if (combined_pulse_pos < C72_binomials[PULSE_MAX - 1]) {
			if (p->pulse_dec_mode != 0)
				goto LABEL_22;
		} else
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
		};
		////////////////////////////

		if (p->pulse_dec_mode) {
			int pulse, pulse_idx;
			LABEL_22: pulse = PULSE_MAX - 1;
			pulse_idx = 71; //GRID_SIZE
			combined_pulse_pos =
					fparam->sf[subframe_idx].combined_pulse_pos;

			/* this part seems to be close to g723.1 gen_fcb_excitation() RATE_6300 */
			/* TODO: 7 is what? size of subframe? */
			for (i = 0; i < 7; i++) {
				for (;combined_pulse_pos < dss_sp_combinatorial_table[pulse][pulse_idx];
						--pulse_idx)
					;
				combined_pulse_pos -=
						dss_sp_combinatorial_table[pulse][pulse_idx];
				pulse--;
				fparam->sf[subframe_idx].pulse_pos[i] = pulse_idx;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////
	v43 = (compressed_buf[19] & 0xffff) << 8;

	v46 = (v43 | ((compressed_buf[20] & 0xff00) >> 8)) / 151;

	fparam->array_20[0] =
			(v43 | ((compressed_buf[20] & 0xff00) >> 8)) % 151 + 36;
	for (i = 1; i < SUBFRAMES; i++) {
		int v47 = v46;
		v46 /= 48;
		fparam->array_20[i] = v47 - 48 * v46;
	}
////////////////////////////////////////////////////////////////////////
	v48 = fparam->array_20[0];
	for (i = 1; i < SUBFRAMES; i++) {
		if (v48 > 162) {
			fparam->array_20[i] += 139;
		} else {
			v51 = v48 - 23;
			if (v51 < 36)
				v51 = 36;
			fparam->array_20[i] += v51;
		}
		v48 = fparam->array_20[i];

	}

}

/* create stage 1 array14_stage0 based on stage0 and some kind of pulse table */
static void dss_sp_get_codebook_val(DSS_SP_Context *p) {
	int i;

	for (i = 0; i < 14; i++)
		p->g_unc_rw_array14_stg1_3D0D64.array14_stage1[i] = g_unc_array_3C84F0[i][p->fparam.codebook_indices[i]];
}

static void dss_sp_stabilize_coeff(struct struc_6 *struc_6_a1,
		int32_t *struc_6_stg2_a2) {
	int v3; // esi@1
	int v5; // ebx@2
	signed int v6; // eax@2
	int v7; // esi@3
	int v8; // edi@3

	int counter; // [sp+2Ch] [bp+8h]@2

	int tmp;

	v3 = 0;
	struc_6_stg2_a2[0] = 0x2000;
	while (1) {
		v5 = v3;
		v6 = v3 + 1;
		struc_6_stg2_a2[v3 + 1] = struc_6_a1->array14_stage1[v3] >> 2;
		if (v6 / 2 >= 1)
			break;
		LABEL_9: ++v3;
		if (v3 >= 14)
			return;
	}

	/////////////////////////////
	counter = 1;
	while (1) {
		v7 = struc_6_stg2_a2[counter];
		// 4, 4, 4
		v8 = struc_6_stg2_a2[1 + v5 - counter];
		// 8, 8, c
		tmp = (struc_6_a1->array14_stage1[v5] * v8 + (v7 << 15) + 0x4000) >> 15;
		struc_6_stg2_a2[counter] = tmp;
		tmp &= 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000)
			break;

		tmp = (struc_6_a1->array14_stage1[v5] * v7 + (v8 << 15) + 0x4000) >> 15;
		struc_6_stg2_a2[1 + v5 - counter] = tmp;
		tmp &= 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000)
			break;

		++counter;
		if (counter > v6 / 2)
			goto LABEL_9;
	}
	printf("%s should never be here\n", __func__);
#if 0
	struct struc_6 *struc_6_v14 = struc_6_a1;
	int v24, v14 = 0;
	int word_3D9B7C = 1;
	struc_6_stg2_a2[0] = 0x1000;
	int v26 = 0;
	struct struc_6 *struc_6_v29 = struc_6_a1;
	int32_t *array14a = struc_6_stg2_a2;
	int v28 = 14;
	do
	{
		int v15 = v14 + 1;
		*array14a = struc_6_v14->array14_stage1[0] >> 3;
		counter = 1;
		int v30 = v14 + 1;
		int v29 = (v14 + 1) / 2;
		if ( (v14 + 1) / 2 >= 1 )
		{
			int v16 = 1;
			while ( 1 )
			{
				int v17 = v14 - v16;
				int v19 = struc_6_stg2_a2[v17 + 1];

				tmp = (struc_6_v14->array14_stage1[0] * v19 + (struc_6_stg2_a2[v16] << 15) + 0x4000) >> 15;
				struc_6_stg2_a2[v16] = tmp;
				tmp &= 0xFFFF8000;
				if ( tmp && tmp != 0xFFFF8000 )
					struc_6_stg2_a2[v16] = ((tmp <= 0) - 1) - 0x8000;

				struc_6_v14 = struc_6_v29;

				int v22 = (struc_6_stg2_a2[v16] * struc_6_v29->array14_stage1[0] + (v19 << 15) + 0x4000) >> 15;
				struc_6_stg2_a2[v17] = v22;
				v22 &= 0xFFFF8000;
				if ( v22 && v22 != 0xFFFF8000 )
				{
					if ( v22 <= 0 )
						struc_6_stg2_a2[v17] = 0xFFFF8000;
					else
						struc_6_stg2_a2[v17] = 0x7FFF;
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

static void dss_sp_sub_3B9080(int32_t *array72, int32_t *array36, int a3, int a4) {

	int i;

	/* do we actually need this check? we can use just [a3 - i % a3] for both cases */
	if (a3 < 72)
		for (i = 0; i < 72; i++)
			array72[i] = array36[a3 - i % a3];
	else
		for (i = 0; i < 72; i++)
			array72[i] = array36[a3 - i];

	for (i = 0; i < 72; i++) {
		int tmp = a4 * array72[i] >> 11;
		array72[i] = tmp;
		tmp = tmp & 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000)
			array72[i] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;

	};
}

static void dss_sp_normalize(int32_t *array_a1, int normalize_bits, int array_a1_size) {
	int i;

	if (array_a1_size <= 0)
		return;

	if (normalize_bits < 0)
		for (i = 0; i < array_a1_size; i++)
			array_a1[i] = array_a1[i] >> abs(normalize_bits);
	else
		for (i = 0; i < array_a1_size; i++)
			array_a1[i] = array_a1[i] << normalize_bits;
}

static void dss_sp_sub_3B9FB0(int32_t *array72, int32_t *arrayXX) {
	int i;

	for (i = 114; i > 0; i--)
		arrayXX[i + 72] = arrayXX[i];

	for (i = 0; i < 72; i++)
		arrayXX[72 - i] = array72[i];
}

static void dss_sp_shift_sq_sub(const int32_t *array_a1,
		int32_t *array_a2, int32_t *array_a3_dst) {
	int a;

	for (a = 0; a < 72; a++) {
		int i, tmp;

		tmp = array_a3_dst[a] * array_a1[0];

		for (i = 14; i > 0; i--)
			tmp -= array_a2[i] * array_a1[i];

		/* original code overwrite array_a2[1] two times - makes no sense for me. */
		for (i = 14; i > 0; i--)
			array_a2[i] = array_a2[i - 1];

		tmp = (tmp + 4096) >> 13;

		array_a2[1] = tmp;

		array_a3_dst[a] = tmp;
		tmp &= 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000) {
			if (tmp <= 0)
				array_a2[1] = array_a3_dst[a] = 0xFFFF8000;
			else
				array_a2[1] = array_a3_dst[a] = 0x7FFF;
		}
	}
}

static void dss_sp_shift_sq_add(const int32_t *array_a1, int32_t *array_a2,
		int32_t *array_a3_dst) {
	int a;

	for (a = 0; a < 72; a++) {
		int i, tmp;

		array_a2[0] = array_a3_dst[a];

		for (i = 14; i >= 0; i--)
			tmp += array_a2[i] * array_a1[i];

		for (i = 14; i > 0; i--)
			array_a2[i] = array_a2[i - 1];

		tmp = (tmp + 4096) >> 13;

		array_a3_dst[a] = tmp;
		tmp &= 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000)
			array_a3_dst[a] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
	}
}

static void dss_sp_vec_mult(const int32_t *array15_ro_src, int32_t *array15_dst,
		const int32_t *array15_ro_a3) {
	int i;

	array15_dst[0] = array15_ro_src[0];

	for (i = 1; i < 15; i++)
		array15_dst[i] = (array15_ro_src[i] * array15_ro_a3[i] + 0x4000) >> 15;
}

static int dss_sp_get_normalize_bits(int32_t *array_var, int16_t size) {
	unsigned int val;
	int max_val;
	int i;

	val = 1;
	if (size)
		for (i = 0; i < size; i++)
			val |= abs(array_var[i]);

	for (max_val = 0;
			((val & 0xFFFFC000) == 0) | ((val & 0xFFFFC000) == 0xFFFFC000);
			++max_val)
		val *= 2;
	return max_val;
}

static void dss_sp_sf_synthesis(DSS_SP_Context *p, int32_t a0,
		int32_t *array72_a4, int size) {

	int32_t local_rw_array15_v1a[15];
	int32_t local_rw_array15_v39[15];
	int32_t local_rw_array_v41[72];
	int v11, v22, v23, v18, v34, v36, normalize_bits;
	int i, tmp;

	v34 = 0;
	if (size > 0) {
		for (i = 0; i < size; i++)
			v34 += abs(p->g_unc_rw_array72_3D0C44[i]);

		if (v34 > 0xFFFFF)
			v34 = 0xFFFFF;
	}

	normalize_bits = dss_sp_get_normalize_bits(p->g_unc_rw_array72_3D0C44, size);

	dss_sp_normalize(p->g_unc_rw_array72_3D0C44, normalize_bits - 3, size);
	dss_sp_normalize(p->g_unc_rw_array15_3D0420, normalize_bits, 15);
	dss_sp_normalize(p->g_unc_rw_array15_3D045C, normalize_bits, 15);

	v36 = p->g_unc_rw_array15_3D045C[1];

	dss_sp_vec_mult(p->g_unc_rw_array15_stg2_3D08C0, local_rw_array15_v39,
			binary_decreasing_array);
	dss_sp_shift_sq_add(local_rw_array15_v39, p->g_unc_rw_array15_3D0420,
			p->g_unc_rw_array72_3D0C44);

	dss_sp_vec_mult(p->g_unc_rw_array15_stg2_3D08C0, local_rw_array15_v1a,
			dss_sp_unc_decreasing_array);
	dss_sp_shift_sq_sub(local_rw_array15_v1a,
			p->g_unc_rw_array15_3D045C, p->g_unc_rw_array72_3D0C44);

	/* a0 can be negative */
	v11 = a0 >> 1;
	if (v11 >= 0)
		v11 = 0;

	if (size > 1) {
		for (i = size - 1; i > 0; i--) {
			tmp = ((v11 * p->g_unc_rw_array72_3D0C44[i - 1] + (p->g_unc_rw_array72_3D0C44[i] << 15)) + 0x4000)
					>> 15;
			p->g_unc_rw_array72_3D0C44[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000)
				p->g_unc_rw_array72_3D0C44[i] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
		}
	}

	tmp = (v36 * v11 + (p->g_unc_rw_array72_3D0C44[0] << 15) + 16384) >> 15;
	p->g_unc_rw_array72_3D0C44[0] = tmp;
	tmp &= 0xFFFF8000;
	if (tmp && tmp != 0xFFFF8000)
		p->g_unc_rw_array72_3D0C44[0] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;

	dss_sp_normalize(p->g_unc_rw_array72_3D0C44, -normalize_bits, size);
	dss_sp_normalize(p->g_unc_rw_array15_3D0420, -normalize_bits, 15);
	dss_sp_normalize(p->g_unc_rw_array15_3D045C, -normalize_bits, 15);

	v18 = 0;
	if (size > 0)
		for (i = 0; i < size; i++)
			v18 += abs(p->g_unc_rw_array72_3D0C44[i]);

	if (v18 & 0xFFFFFFC0)
		v22 = (v34 << 11) / v18;
	else
		v22 = 1;

	v23 = 409 * v22 >> 15 << 15;
	tmp = (v23 + 32358 * p->dword_3D0498) >> 15;
	local_rw_array_v41[0] = tmp;
	tmp &= 0xFFFF8000;
	if (tmp && tmp != 0xFFFF8000)
		local_rw_array_v41[0] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;

	if (size > 1) {
		for (i = 1; i < size; i++) {
			tmp = (v23 + 32358 * local_rw_array_v41[i - 1]) >> 15;
			local_rw_array_v41[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000)
				local_rw_array_v41[i] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
		}
	}

	p->dword_3D0498 = local_rw_array_v41[size - 1];
	if (size > 0) {
		for (i = 0; i < size; i++) {
			tmp = (p->g_unc_rw_array72_3D0C44[i] * local_rw_array_v41[i]) >> 11;
			array72_a4[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000)
				array72_a4[i] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
		}
	}
}

static void dss_sp_sub_3B98D0(DSS_SP_Context *p, int32_t *array72_a1) {
	int v1;

	signed int v10;

	int v12;
	int i, offset, counter, array_size;

	v1 = 0;

	for (i = 0; i < 6; i++)
		p->g_unc_rw_array288_3D0DC0[i] = p->g_unc_rw_array288_3D0DC0[288 + i];

	for (i = 0; i < 72 * SUBFRAMES; i++)
		p->g_unc_rw_array288_3D0DC0[6 + i] = array72_a1[i];

	offset = 6;
	counter = 0;
	array_size = sizeof(p->g_unc_rw_array288_3D0DC0) / sizeof(int32_t);
	do {
		v10 = 0;
		for (i = 0; i < 6; i++)
			v10 += p->g_unc_rw_array288_3D0DC0[offset--]
					* g_unc_array_3C9288[v1 + i * 11];

		offset += 7;

		v12 = v10 >> 15;
		array72_a1[counter] = v12;
		v12 &= 0xFFFF8000;
		if (v12 && v12 != 0xFFFF8000)
			array72_a1[counter] = (((v12 <= 0) - 1) & 0xFFFE) - 0x7FFF;

		counter++;

		v1 = (v1 + 1) % 11;
		if (!v1)
			offset++;
	} while (offset < array_size);
}

static void dss_sp_32to16bit(int16_t *dst, int32_t *src, int size) {
	int i;

	if (!size)
		return;

	for (i = 0; i < size; i++)
		dst[i] = src[i];
}

static int dss_sp_decode_frame_2(DSS_SP_Context *p, int16_t *abuf_dst, const int8_t *abuff_src) {

	int i, tmp, sf_idx;

	dss_sp_unpack_coeffs(p, (const int16_t *)abuff_src);

	dss_sp_get_codebook_val(p);

	dss_sp_stabilize_coeff(&p->g_unc_rw_array14_stg1_3D0D64,
			p->g_unc_rw_array15_stg2_3D08C0);

////////
	for (sf_idx = 0; sf_idx < SUBFRAMES; sf_idx++) {

		dss_sp_sub_3B9080(p->g_unc_rw_array72_3D0C44, p->g_unc_rw_arrayXX_3D08FC,
				p->fparam.array_20[sf_idx],
				g_unc_array_3C88F8[p->fparam.subframe_something[sf_idx]]);

		dss_sp_add_pulses(p->g_unc_rw_array72_3D0C44, &p->fparam.sf[sf_idx]);

		dss_sp_sub_3B9FB0(p->g_unc_rw_array72_3D0C44, p->g_unc_rw_arrayXX_3D08FC);

		/* swap and copy buffer */
		for (i = 0; i < 72; i++)
			p->g_unc_rw_array72_3D0C44[i] = p->g_unc_rw_arrayXX_3D08FC[72 - i];

		dss_sp_shift_sq_sub(p->g_unc_rw_array15_stg2_3D08C0,
				p->g_unc_rw_array15_3D0BE8, p->g_unc_rw_array72_3D0C44);

		for (i = 0; i < 72; i++) {
			tmp = (((p->dword_3D0DA0 << 13) - p->dword_3D0DA0)
					+ (p->g_unc_rw_array72_3D0C44[i] << 15) + 0x4000) >> 15;
			p->g_unc_rw_array72_3D0C44[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000) {
				p->dword_3D0DA0 = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
				p->g_unc_rw_array72_3D0C44[i] = p->dword_3D0DA0;
			}
		}

		dss_sp_sf_synthesis(p, p->g_unc_rw_array14_stg1_3D0D64.array14_stage1[0],
				&p->working_buffer[sf_idx][0], 72);

	};
////////

	dss_sp_sub_3B98D0(p, &p->working_buffer[0][0]);

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
    dss_sp_decode_frame_2(p, out, buf);

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
