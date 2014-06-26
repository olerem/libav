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
    int32_t g_unc_rw_arrayXX_3D08FC[186];
	struct struc_1 struc_1_v96;
	int32_t local_rw_array72_v101[SUBFRAMES][72];
	int32_t g_unc_rw_array15_3D0420[15];
	int32_t g_unc_rw_array15_3D045C[15];
	struct struc_6 g_unc_rw_array14_stg1_3D0D64;
	int32_t g_unc_rw_array15_stg2_3D08C0[15];
	int16_t array14_3D0DA4[14];
	int32_t g_unc_rw_array72_3D0C44[72];
	int dword_3D0498;
	int dword_3D0DA0;
	int32_t g_unc_rw_array15_3D0BE8[15];

    unsigned int word_3D0C26;

} DSS_SP_Context;

static av_cold int dss_sp_decode_init(AVCodecContext *avctx)
{
    DSS_SP_Context *p = avctx->priv_data;
    avctx->channel_layout = AV_CH_LAYOUT_MONO;
    avctx->sample_fmt     = AV_SAMPLE_FMT_S16;
    avctx->channels       = 1;
    avctx->sample_rate    = 12000;

    memset(p->g_unc_rw_arrayXX_3D08FC, 0, 0x2ECu);
    p->word_3D0C26 = 1;

    return 0;
}

static void dss2_unpack_coeffs(DSS_SP_Context *p, struct struc_1 *reconstr_abuff, const int16_t *abuff_swap_a2) {

	int i;
	int subframe_idx;
	uint32_t v43;
	int v46;
	uint32_t v51;
	uint32_t v48;

	reconstr_abuff->array14_stage0[0] = (abuff_swap_a2[0] >> 11) & 0x1F;
	reconstr_abuff->array14_stage0[1] = (abuff_swap_a2[0] >> 6) & 0x1F;
	reconstr_abuff->array14_stage0[2] = (abuff_swap_a2[0] >> 2) & 0xF;
	reconstr_abuff->array14_stage0[3] = ((abuff_swap_a2[1] >> 14) & 3)
			+ 4 * (abuff_swap_a2[0] & 3);

	reconstr_abuff->array14_stage0[4] = (abuff_swap_a2[1] >> 10) & 0xF;
	reconstr_abuff->array14_stage0[5] = (abuff_swap_a2[1] >> 6) & 0xF;
	reconstr_abuff->array14_stage0[6] = (abuff_swap_a2[1] >> 2) & 0xF;
	reconstr_abuff->array14_stage0[7] = ((abuff_swap_a2[2] >> 14) & 3)
			+ 4 * (abuff_swap_a2[1] & 3);

	reconstr_abuff->array14_stage0[8] = (abuff_swap_a2[2] >> 11) & 7;
	reconstr_abuff->array14_stage0[9] = (abuff_swap_a2[2] >> 8) & 7;
	reconstr_abuff->array14_stage0[10] = (abuff_swap_a2[2] >> 5) & 7;
	reconstr_abuff->array14_stage0[11] = (abuff_swap_a2[2] >> 2) & 7;
	reconstr_abuff->array14_stage0[12] = ((abuff_swap_a2[3] >> 15) & 1)
			+ 2 * (abuff_swap_a2[2] & 3);

	reconstr_abuff->array14_stage0[13] = (abuff_swap_a2[3] >> 12) & 7;

	reconstr_abuff->subframe_something[0] = (abuff_swap_a2[3] >> 7) & 0x1F;

	reconstr_abuff->sf[0].combined_pulse_pos =
			  (abuff_swap_a2[5] & 0xff00) >> 8
			| (abuff_swap_a2[4] & 0xffff) << 8
			| (abuff_swap_a2[3] & 0x7f) << 24;

	reconstr_abuff->sf[0].gain = (abuff_swap_a2[5] >> 2) & 0x3F;

	reconstr_abuff->sf[0].pulse_val[0] = ((abuff_swap_a2[6] >> 15) & 1)
			+ 2 * (abuff_swap_a2[5] & 3);
	reconstr_abuff->sf[0].pulse_val[1] = (abuff_swap_a2[6] >> 12) & 7;
	reconstr_abuff->sf[0].pulse_val[2] = (abuff_swap_a2[6] >> 9) & 7;
	reconstr_abuff->sf[0].pulse_val[3] = (abuff_swap_a2[6] >> 6) & 7;
	reconstr_abuff->sf[0].pulse_val[4] = (abuff_swap_a2[6] >> 3) & 7;
	reconstr_abuff->sf[0].pulse_val[5] = abuff_swap_a2[6] & 7;
	reconstr_abuff->sf[0].pulse_val[6] = (abuff_swap_a2[7] >> 13) & 7;

	reconstr_abuff->subframe_something[1] = (abuff_swap_a2[7] >> 8) & 0x1F;

	reconstr_abuff->sf[1].combined_pulse_pos =
			  (abuff_swap_a2[9] & 0xfe00) >> 9
			| (abuff_swap_a2[8] & 0xffff) << 7
			| (abuff_swap_a2[7] & 0xff) << 23;

	reconstr_abuff->sf[1].gain = (abuff_swap_a2[9] >> 3) & 0x3F;

	reconstr_abuff->sf[1].pulse_val[0] = abuff_swap_a2[9] & 7;
	reconstr_abuff->sf[1].pulse_val[1] = (abuff_swap_a2[10] >> 13) & 7;
	reconstr_abuff->sf[1].pulse_val[2] = (abuff_swap_a2[10] >> 10) & 7;
	reconstr_abuff->sf[1].pulse_val[3] = (abuff_swap_a2[10] >> 7) & 7;
	reconstr_abuff->sf[1].pulse_val[4] = (abuff_swap_a2[10] >> 4) & 7;
	reconstr_abuff->sf[1].pulse_val[5] = (abuff_swap_a2[10] >> 1) & 7;
	reconstr_abuff->sf[1].pulse_val[6] = ((abuff_swap_a2[11] >> 14) & 3)
			+ 4 * (abuff_swap_a2[10] & 1);

	reconstr_abuff->subframe_something[2] = (abuff_swap_a2[11] >> 9) & 0x1F;

	reconstr_abuff->sf[2].combined_pulse_pos =
			  (abuff_swap_a2[13] & 0xfc00) >> 10
			| (abuff_swap_a2[12] & 0xffff) << 6
			| (abuff_swap_a2[11] & 0x1ff) << 22;

	reconstr_abuff->sf[2].gain = (abuff_swap_a2[13] >> 4) & 0x3F;

	reconstr_abuff->sf[2].pulse_val[0] = (abuff_swap_a2[13] >> 1) & 7;
	reconstr_abuff->sf[2].pulse_val[1] = ((abuff_swap_a2[14] >> 14) & 3)
			+ 4 * (abuff_swap_a2[14] & 1);
	reconstr_abuff->sf[2].pulse_val[2] = (abuff_swap_a2[14] >> 11) & 7;
	reconstr_abuff->sf[2].pulse_val[3] = (abuff_swap_a2[14] >> 8) & 7;
	reconstr_abuff->sf[2].pulse_val[4] = (abuff_swap_a2[14] >> 5) & 7;
	reconstr_abuff->sf[2].pulse_val[5] = (abuff_swap_a2[14] >> 2) & 7;
	reconstr_abuff->sf[2].pulse_val[6] = ((abuff_swap_a2[15] >> 15) & 1)
			+ 2 * (abuff_swap_a2[14] & 3);

	reconstr_abuff->subframe_something[3] = (abuff_swap_a2[15] >> 10) & 0x1F;

	reconstr_abuff->sf[3].combined_pulse_pos =
			  (abuff_swap_a2[17] & 0xf800) >> 11
			| (abuff_swap_a2[16] & 0xffff) << 5
			| (abuff_swap_a2[15] & 0x3ff) << 21;

	reconstr_abuff->sf[3].gain = (abuff_swap_a2[17] >> 5) & 0x3F;

	reconstr_abuff->sf[3].pulse_val[0] = (abuff_swap_a2[17] >> 2) & 7;
	reconstr_abuff->sf[3].pulse_val[1] = ((abuff_swap_a2[18] >> 15) & 1)
			+ 2 * (abuff_swap_a2[17] & 3);
	reconstr_abuff->sf[3].pulse_val[2] = (abuff_swap_a2[18] >> 12) & 7;
	reconstr_abuff->sf[3].pulse_val[3] = (abuff_swap_a2[18] >> 9) & 7;
	reconstr_abuff->sf[3].pulse_val[4] = (abuff_swap_a2[18] >> 6) & 7;
	reconstr_abuff->sf[3].pulse_val[5] = (abuff_swap_a2[18] >> 3) & 7;
	reconstr_abuff->sf[3].pulse_val[6] = abuff_swap_a2[18] & 7;

////////////////////////////////////////////////////////////////////
	for (subframe_idx = 0; subframe_idx < 4; subframe_idx++) {
		unsigned int C72_binomials[PULSE_MAX] = { 72, 2556, 59640, 1028790,
				13991544, 156238908, 1473109704, 3379081753 };
		unsigned int combined_pulse_pos =
				reconstr_abuff->sf[subframe_idx].combined_pulse_pos;
		int index = 6;

		if (combined_pulse_pos < C72_binomials[PULSE_MAX - 1]) {
			if (p->word_3D0C26 != 0)
				goto LABEL_22;
		} else
				p->word_3D0C26 = 0;

		/* why do we need this? */
		reconstr_abuff->sf[subframe_idx].pulse_pos[6] = 0;

		//////////////////
		for (i = 71; i >= 0; i--) {
			if (C72_binomials[index] <= combined_pulse_pos) {
				combined_pulse_pos -= C72_binomials[index];

				reconstr_abuff->sf[subframe_idx].pulse_pos[(index ^ 7) - 1] = i;

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

		if (p->word_3D0C26) {
			int pulse, pulse_idx;
			LABEL_22: pulse = PULSE_MAX - 1;
			pulse_idx = 71; //GRID_SIZE
			combined_pulse_pos =
					reconstr_abuff->sf[subframe_idx].combined_pulse_pos;

			/* this part seems to be close to g723.1 gen_fcb_excitation() RATE_6300 */
			/* TODO: 7 is what? size of subframe? */
			for (i = 0; i < 7; i++) {
				for (;
						combined_pulse_pos
								< dss2_combinatorial_table[pulse][pulse_idx];
						--pulse_idx)
					;
				combined_pulse_pos -=
						dss2_combinatorial_table[pulse][pulse_idx];
				pulse--;
				reconstr_abuff->sf[subframe_idx].pulse_pos[i] = pulse_idx;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////
	v43 = (abuff_swap_a2[19] & 0xffff) << 8;

	v46 = (v43 | ((abuff_swap_a2[20] & 0xff00) >> 8)) / 151;


	// TODO, is filed_1e part of array_20?
	reconstr_abuff->filed_1e =
			(v43 | ((abuff_swap_a2[20] & 0xff00) >> 8)) % 151 + 36;
	for (i = 0; i < 3; i++) {
		int v47 = v46;
		v46 /= 48;
		reconstr_abuff->array_20[i] = v47 - 48 * v46;
	}
////////////////////////////////////////////////////////////////////////
	v48 = reconstr_abuff->filed_1e;
	for (i = 0; i < 3; i++) {
		if (v48 > 162) {
			reconstr_abuff->array_20[i] += 139;
		} else {
			v51 = v48 - 23;
			if (v51 < 36)
				v51 = 36;
			reconstr_abuff->array_20[i] += v51;
		}
		v48 = reconstr_abuff->array_20[i];

	}

}

/* create stage 1 array14_stage0 based on stage0 and some kind of pulse table */
static void dss2_sub_3B8740(int32_t *array14_stage1, const struct struc_1 *a2) {
	int i;

	for (i = 0; i < 14; i++)
		array14_stage1[i] = g_unc_array_3C84F0[i][a2->array14_stage0[i]];
}

static void dss2_sub_3B8410(struct struc_6 *struc_6_a1,
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
}

/* this function will get pointer to one of 4 subframes */
static void dss2_add_pulses(int32_t *array72_a1, const struct dss2_subframe *sf) {
	int i;

	//looks like "output[sf->pulse_pos[i]] += g_gains[sf->gain] * g_pulse_val[sf->pulse_val[i]] + 0x4000 >> 15;"
	for (i = 0; i < 7; i++)
		array72_a1[sf->pulse_pos[i]] += (dss2_fixed_cb_gain[sf->gain]
				* dss2_pulse_val[sf->pulse_val[i]] + 0x4000) >> 15;

}

static void dss2_sub_3B9080(int32_t *array72, int32_t *array36, int a3, int a4) {

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

static void dss2_normalize(int32_t *array_a1, int normalize_bits, int array_a1_size) {
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

static void dss2_sub_3B9FB0(int32_t *array72, int32_t *arrayXX) {
	int i;

	for (i = 0; i < 114; i++)
		arrayXX[114 - i] = arrayXX[186 - i];

	for (i = 0; i < 72; i++)
		arrayXX[72 - i] = array72[i];
}

static void dss2_shift_sq_sub(const int32_t *array_a1,
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

static void dss2_shift_sq_add(const int32_t *array_a1, int32_t *array_a2,
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

static void dss2_vec_mult(const int32_t *array15_ro_src, int32_t *array15_dst,
		const int32_t *array15_ro_a3) {
	int i;

	array15_dst[0] = array15_ro_src[0];

	/* TODO: pseudocode and my result are too different. May be my version is wrong */
	for (i = 1; i < 15; i++)
		array15_dst[i] = (array15_ro_src[i] * array15_ro_a3[i] + 0x4000) >> 15;
}

static int dss2_get_normalize_bits(int32_t *array_var, int16_t size) {
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

static void dss2_sub_3B80F0(DSS_SP_Context *p, int32_t a0, int32_t *array15_a1, int32_t *array72_a3,
		int32_t *array72_a4, int size) {

	int32_t local_rw_array15_v1a[15];
	int32_t local_rw_array15_v39[15];
	int32_t local_rw_array_v41[72];
	int v11, v22, v23, v18, v34, v36, normalize_bits;
	int i, tmp;

	v34 = 0;
	if (size > 0) {
		for (i = 0; i < size; i++)
			v34 += abs(array72_a3[i]);

		if (v34 > 0xFFFFF)
			v34 = 0xFFFFF;
	}

	normalize_bits = dss2_get_normalize_bits(array72_a3, size);

	dss2_normalize(array72_a3, normalize_bits - 3, size);
	dss2_normalize(p->g_unc_rw_array15_3D0420, normalize_bits, 15);
	dss2_normalize(p->g_unc_rw_array15_3D045C, normalize_bits, 15);

	v36 = p->g_unc_rw_array15_3D045C[1];

	dss2_vec_mult(array15_a1, local_rw_array15_v39, g_unc_ro_array37_3C845C);
	dss2_shift_sq_add(local_rw_array15_v39, p->g_unc_rw_array15_3D0420,
			array72_a3);

	dss2_vec_mult(array15_a1, local_rw_array15_v1a, g_unc_ro_array15_3C8420);
	dss2_shift_sq_sub(local_rw_array15_v1a,
			p->g_unc_rw_array15_3D045C, array72_a3);

	/* a0 can be negative */
	v11 = a0 >> 1;
	if (v11 >= 0)
		v11 = 0;

	if (size > 1) {
		for (i = size - 1; i > 0; i--) {
			tmp = ((v11 * array72_a3[i - 1] + (array72_a3[i] << 15)) + 0x4000)
					>> 15;
			array72_a3[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000)
				array72_a3[i] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
		}
	}

	tmp = (v36 * v11 + (array72_a3[0] << 15) + 16384) >> 15;
	array72_a3[0] = tmp;
	tmp &= 0xFFFF8000;
	if (tmp && tmp != 0xFFFF8000)
		array72_a3[0] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;

	dss2_normalize(array72_a3, -normalize_bits, size);
	dss2_normalize(p->g_unc_rw_array15_3D0420, -normalize_bits, 15);
	dss2_normalize(p->g_unc_rw_array15_3D045C, -normalize_bits, 15);

	v18 = 0;
	if (size > 0)
		for (i = 0; i < size; i++)
			v18 += abs(array72_a3[i]);

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
			tmp = (array72_a3[i] * local_rw_array_v41[i]) >> 11;
			array72_a4[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000)
				array72_a4[i] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
		}
	}
}

static void dss2_sub_3B98D0(DSS_SP_Context *p, int32_t *array72_a1) {
	int v1;

	signed int v10;

	int v12;
	int i, offset, counter;

	v1 = 0;

	for (i = 0; i < 6; i++)
		p->g_unc_rw_array288_3D0DC0[i] = p->g_unc_rw_array288_3D0DC0[288 + i];

	for (i = 0; i < 72; i++)
		p->g_unc_rw_array288_3D0DC0[6 + i] = array72_a1[i];

	offset = 6;
	counter = 0;
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
	} while (offset < sizeof(p->g_unc_rw_array288_3D0DC0) / sizeof(int32_t));
}

static void dss2_32to16bit(int16_t *dst, int32_t *src, int size) {
	int i;

	if (!size)
		return;

	for (i = 0; i < size; i++)
		dst[i] = src[i];
}

static int dss2_2_sub_3B8790(DSS_SP_Context *p, int16_t *abuf_dst, const int8_t *abuff_src) {

	int i, tmp, sf_idx;

	dss2_unpack_coeffs(p, &p->struc_1_v96, (const int16_t *)abuff_src);

	dss2_sub_3B8740(p->g_unc_rw_array14_stg1_3D0D64.array14_stage1, &p->struc_1_v96);

	dss2_sub_3B8410(&p->g_unc_rw_array14_stg1_3D0D64,
			p->g_unc_rw_array15_stg2_3D08C0);

////////
	for (sf_idx = 0; sf_idx < SUBFRAMES; sf_idx++) {

		dss2_sub_3B9080(p->g_unc_rw_array72_3D0C44, p->g_unc_rw_arrayXX_3D08FC,
				p->struc_1_v96.filed_1e,
				g_unc_array_3C88F8[p->struc_1_v96.subframe_something[sf_idx]]);

		dss2_add_pulses(p->g_unc_rw_array72_3D0C44, &p->struc_1_v96.sf[sf_idx]);

		dss2_sub_3B9FB0(p->g_unc_rw_array72_3D0C44, p->g_unc_rw_arrayXX_3D08FC);

		/* swap and copy buffer */
		for (i = 0; i < 72; i++)
			p->g_unc_rw_array72_3D0C44[i] = p->g_unc_rw_arrayXX_3D08FC[72 - i];

		dss2_shift_sq_sub(p->g_unc_rw_array15_stg2_3D08C0,
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

		dss2_sub_3B80F0(p, p->g_unc_rw_array14_stg1_3D0D64.array14_stage1[0],
				p->g_unc_rw_array15_stg2_3D08C0, p->g_unc_rw_array72_3D0C44,
				&p->local_rw_array72_v101[sf_idx][0], 72);

	};
////////

	dss2_sub_3B98D0(p, &p->local_rw_array72_v101[0][0]);

	dss2_32to16bit(abuf_dst,
					&p->local_rw_array72_v101[0][0], 264);
	memcpy(&p->array14_3D0DA4, p->struc_1_v96.array14_stage0, 28u);
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
    dss2_2_sub_3B8790(p, out, buf);

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
