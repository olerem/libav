/*
 * dss2_decoder
 *
 *  Created on: 03.06.2014
 *      Autor: lex
 */

#include <stdio.h>
#include <stdint.h>
#include "dss.h"
#define __int8 int8_t
#define __int16 int16_t

unsigned int word_3D1264;
unsigned int word_3D1266;
unsigned int word_3D041C;
unsigned int word_3D0C26;
unsigned int word_3D9B7C;
int dword_3D0498;
unsigned int flip;
int32_t g_unc_rw_array15_3D0420[15];
int32_t g_unc_rw_array15_3D045C[15];
struct struc_6 g_unc_rw_array14_stg1_3D0D64;
struct struc_8 g_unc_rw_array15_stg2_3D08C0;
int16_t array14_3D0DA4[14];
int32_t g_unc_rw_array72_3D0C44[72];
int32_t g_unc_rw_arrayXX_3D08FC[186];
int word_3D9B7E;
int dword_3D0DA0;
struct struc_8 g_unc_rw_array15_3D0BE8;
int32_t g_unc_rw_array288_3D0DC0[288 + 6];
int32_t g_unc_rw_array_3D04A0[264];

void dss2_byte_swap(__int8 *abuff_swap, __int8 **abuff_src, int dec_flag) {
	uint8_t *abuff_tmp;
	int size; // si@1
	int i;

	word_3D1264 = 1;
	abuff_tmp = *abuff_src;
	size = DSS_CBUF_SIZE;
	if (flip) {
		unsigned int tmp = abuff_tmp[0];
#if 0
		if (dec_flag & 2 && !(tmp & 0x80) )
		{
			size = 5;
			word_3D1264 = 0;
		}
#endif
		abuff_tmp++;

		for (i = 0; i < size - 1; i++) {
			abuff_swap[0] = tmp;
			abuff_swap[1] = abuff_tmp[0];
			tmp = abuff_tmp[1];
			abuff_tmp += 2;
			abuff_swap += 2;
		}

		word_3D1266 = tmp;
		abuff_swap[0] = 0;
		abuff_swap[1] = abuff_tmp[0];
		*abuff_src = abuff_tmp;
		flip ^= 1;
	} else {
#if 0
		if ( dec_flag & 2 && !(*(abuff_tmp - 1) & 0x80) ) // Never executed
		{
			size = 5;
			word_3D1264 = 0;
		}
#endif
		abuff_swap[0] = abuff_tmp[2];
		abuff_swap[1] = word_3D1266;
		abuff_tmp++;
		abuff_swap += 2;

		for (i = 0; i < size - 2; i++) {
			abuff_swap[0] = abuff_tmp[3];
			abuff_swap[1] = abuff_tmp[0];
			abuff_swap += 2;
			abuff_tmp += 2;
		}

		abuff_swap[1] = abuff_tmp[0];
		*abuff_src = abuff_tmp + 2;
		flip ^= 1;
	}
}

void dss2_unpack_coeffs(struct struc_1 *reconstr_abuff, int16_t *abuff_swap_a2) {
	int16_t *abuff_swap_ptr; // eax@2
	int v12; // edx@2
	int i;

	if (word_3D041C) {

		word_3D1264 = reconstr_abuff->field_0 = (abuff_swap_ptr[0] >> 15) & 1;

		reconstr_abuff->array14_stage0[0] = (abuff_swap_ptr[0] >> 10) & 0x1F;
		reconstr_abuff->array14_stage0[1] = (abuff_swap_ptr[0] >> 5) & 0x1F;
		reconstr_abuff->array14_stage0[2] = (abuff_swap_ptr[0] >> 1) & 0xF;
		reconstr_abuff->array14_stage0[3] = ((abuff_swap_ptr[1] >> 13) & 7)
				+ 8 * (abuff_swap_ptr[0] & 1);

		reconstr_abuff->array14_stage0[4] = (abuff_swap_ptr[1] >> 9) & 0xF;
		reconstr_abuff->array14_stage0[5] = (abuff_swap_ptr[1] >> 5) & 0xF;
		reconstr_abuff->array14_stage0[6] = (abuff_swap_ptr[1] >> 1) & 0xF;
		reconstr_abuff->array14_stage0[7] = ((abuff_swap_ptr[2] >> 13) & 7)
				+ 8 * (abuff_swap_ptr[1] & 1);

		reconstr_abuff->array14_stage0[8] = (abuff_swap_ptr[2] >> 10) & 7;
		reconstr_abuff->array14_stage0[9] = (abuff_swap_ptr[2] >> 7) & 7;
		reconstr_abuff->array14_stage0[10] = (abuff_swap_ptr[2] >> 4) & 7;
		reconstr_abuff->array14_stage0[11] = (abuff_swap_ptr[2] >> 1) & 7;
		reconstr_abuff->array14_stage0[12] = ((abuff_swap_ptr[3] >> 14) & 3)
				+ 4 * (abuff_swap_ptr[2] & 1);

		v12 = (abuff_swap_ptr[3] >> 12) & 3;
	} else {
		word_3D1264 = 1;

		reconstr_abuff->array14_stage0[0] = (abuff_swap_ptr[0] >> 11) & 0x1F;
		reconstr_abuff->array14_stage0[1] = (abuff_swap_ptr[0] >> 6) & 0x1F;
		reconstr_abuff->array14_stage0[2] = (abuff_swap_ptr[0] >> 2) & 0xF;
		reconstr_abuff->array14_stage0[3] = ((abuff_swap_ptr[1] >> 14) & 3)
				+ 4 * (abuff_swap_ptr[0] & 3);

		reconstr_abuff->array14_stage0[4] = (abuff_swap_ptr[1] >> 10) & 0xF;
		reconstr_abuff->array14_stage0[5] = (abuff_swap_ptr[1] >> 6) & 0xF;
		reconstr_abuff->array14_stage0[6] = (abuff_swap_ptr[1] >> 2) & 0xF;
		reconstr_abuff->array14_stage0[7] = ((abuff_swap_ptr[2] >> 14) & 3)
				+ 4 * (abuff_swap_ptr[1] & 3);

		reconstr_abuff->array14_stage0[8] = (abuff_swap_ptr[2] >> 11) & 7;
		reconstr_abuff->array14_stage0[9] = (abuff_swap_ptr[2] >> 8) & 7;
		reconstr_abuff->array14_stage0[10] = (abuff_swap_ptr[2] >> 5) & 7;
		reconstr_abuff->array14_stage0[11] = (abuff_swap_ptr[2] >> 2) & 7;
		reconstr_abuff->array14_stage0[12] = ((abuff_swap_ptr[3] >> 15) & 1)
				+ 2 * (abuff_swap_ptr[2] & 3);

		v12 = (abuff_swap_ptr[3] >> 12) & 7;
	}

	reconstr_abuff->array14_stage0[13] = v12;

	if (word_3D041C && !word_3D1264) {
		reconstr_abuff->subframe_something[0] = (abuff_swap_ptr[3] >> 7) & 0x1F;
		reconstr_abuff->subframe_something[1] = (abuff_swap_ptr[3] >> 2) & 0x1F;
		reconstr_abuff->subframe_something[2] = (abuff_swap_ptr[4] >> 13) & 7;
		reconstr_abuff->subframe_something[3] = (abuff_swap_ptr[4] >> 8) & 0x1F;
		return;
	}

	reconstr_abuff->subframe_something[0] = (abuff_swap_ptr[3] >> 7) & 0x1F;

	// instead of "*((uint8_t *)abuff_swap_ptr + 11)" can be "(abuff_swap_ptr[5] >> 8) & 0xf"
	reconstr_abuff->sf[0].combined_pulse_pos =
			*((uint8_t *) abuff_swap_ptr + 11)
					+ ((abuff_swap_ptr[4] + ((abuff_swap_ptr[3] & 0x7F) << 16))
							<< 8);
	reconstr_abuff->sf[0].gain = (abuff_swap_ptr[5] >> 2) & 0x3F;

	reconstr_abuff->sf[0].pulse_val[0] = ((abuff_swap_ptr[6] >> 15) & 1)
			+ 2 * (abuff_swap_ptr[5] & 3);
	reconstr_abuff->sf[0].pulse_val[1] = (abuff_swap_ptr[6] >> 12) & 7;
	reconstr_abuff->sf[0].pulse_val[2] = (abuff_swap_ptr[6] >> 9) & 7;
	reconstr_abuff->sf[0].pulse_val[3] = (abuff_swap_ptr[6] >> 6) & 7;
	reconstr_abuff->sf[0].pulse_val[4] = (abuff_swap_ptr[6] >> 3) & 7;
	reconstr_abuff->sf[0].pulse_val[5] = abuff_swap_ptr[6] & 7;
	reconstr_abuff->sf[0].pulse_val[6] = (abuff_swap_ptr[7] >> 13) & 7;

	reconstr_abuff->subframe_something[1] = (abuff_swap_ptr[7] >> 8) & 0x1F;

	reconstr_abuff->sf[1].combined_pulse_pos = ((abuff_swap_ptr[9] >> 9) & 0x7F)
			+ ((abuff_swap_ptr[8] + (abuff_swap_ptr[7] << 16)) << 7);
	reconstr_abuff->sf[1].gain = (abuff_swap_ptr[9] >> 3) & 0x3F;

	reconstr_abuff->sf[1].pulse_val[0] = abuff_swap_ptr[9] & 7;
	reconstr_abuff->sf[1].pulse_val[1] = (abuff_swap_ptr[10] >> 13) & 7;
	reconstr_abuff->sf[1].pulse_val[2] = (abuff_swap_ptr[10] >> 10) & 7;
	reconstr_abuff->sf[1].pulse_val[3] = (abuff_swap_ptr[10] >> 7) & 7;
	reconstr_abuff->sf[1].pulse_val[4] = (abuff_swap_ptr[10] >> 4) & 7;
	reconstr_abuff->sf[1].pulse_val[5] = (abuff_swap_ptr[10] >> 1) & 7;
	reconstr_abuff->sf[1].pulse_val[6] = ((abuff_swap_ptr[11] >> 14) & 3)
			+ 4 * (abuff_swap_ptr[10] & 1);

	reconstr_abuff->subframe_something[2] = (abuff_swap_ptr[11] >> 9) & 0x1F;

	reconstr_abuff->sf[2].combined_pulse_pos =
			((abuff_swap_ptr[13] >> 10) & 0x3F)
					+ ((abuff_swap_ptr[12]
							+ ((abuff_swap_ptr[11] & 0x1FF) << 16)) << 6);

	reconstr_abuff->sf[2].gain = (abuff_swap_ptr[13] >> 4) & 0x3F;

	reconstr_abuff->sf[2].pulse_val[0] = (abuff_swap_ptr[13] >> 1) & 7;
	reconstr_abuff->sf[2].pulse_val[1] = ((abuff_swap_ptr[14] >> 14) & 3)
			+ 4 * (abuff_swap_ptr[14] & 1);
	reconstr_abuff->sf[2].pulse_val[2] = (abuff_swap_ptr[14] >> 11) & 7;
	reconstr_abuff->sf[2].pulse_val[3] = (abuff_swap_ptr[14] >> 8) & 7;
	reconstr_abuff->sf[2].pulse_val[4] = (abuff_swap_ptr[14] >> 5) & 7;
	reconstr_abuff->sf[2].pulse_val[5] = (abuff_swap_ptr[14] >> 2) & 7;
	reconstr_abuff->sf[2].pulse_val[6] = ((abuff_swap_ptr[15] >> 15) & 1)
			+ 2 * (abuff_swap_ptr[14] & 3);

	reconstr_abuff->subframe_something[3] = (abuff_swap_ptr[15] >> 10) & 0x1F;

	reconstr_abuff->sf[3].combined_pulse_pos = ((abuff_swap_ptr[17] >> 11)
			& 0x1F)
			+ 32 * (abuff_swap_ptr[16] + ((abuff_swap_ptr[15] & 0x3FF) << 16));
	reconstr_abuff->sf[3].gain = (abuff_swap_ptr[17] >> 5) & 0x3F;

	reconstr_abuff->sf[3].pulse_val[0] = (abuff_swap_ptr[17] >> 2) & 7;
	reconstr_abuff->sf[3].pulse_val[1] = ((abuff_swap_ptr[18] >> 15) & 1)
			+ 2 * (abuff_swap_ptr[17] & 3);
	reconstr_abuff->sf[3].pulse_val[2] = (abuff_swap_ptr[18] >> 12) & 7;
	reconstr_abuff->sf[3].pulse_val[3] = (abuff_swap_ptr[18] >> 9) & 7;
	reconstr_abuff->sf[3].pulse_val[4] = (abuff_swap_ptr[18] >> 6) & 7;
	reconstr_abuff->sf[3].pulse_val[5] = (abuff_swap_ptr[18] >> 3) & 7;
	reconstr_abuff->sf[3].pulse_val[6] = abuff_swap_ptr[18] & 7;

////////////////////////////////////////////////////////////////////
	int subframe_idx;
	for (subframe_idx = 0; subframe_idx < 4; subframe_idx++) {
		unsigned int C72_binomials[PULSE_MAX] = { 72, 2556, 59640, 1028790,
				13991544, 156238908, 1473109704, 3379081753 };
		unsigned int combined_pulse_pos =
				reconstr_abuff->sf[subframe_idx].combined_pulse_pos;
		int index = 6;

		if (combined_pulse_pos < C72_binomials[PULSE_MAX - 1])
			if (word_3D0C26 != 0)
				goto LABEL_22;
			else
				word_3D0C26 = 0;

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

		if (word_3D0C26) {
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
	int16_t v43;
	int v46;

	v43 = abuff_swap_ptr[19];

	v46 = ((v43 << 8) + *((int8_t *) abuff_swap_ptr + 0x29)) / 151;
	// TODO, is filed_1e part of array_20?
	reconstr_abuff->filed_1e =
			((v43 << 8) + *((int8_t *) abuff_swap_ptr + 0x29)) % 151 + 36;
	for (i = 0; i < 3; i++) {
		int v47 = v46;
		v46 /= 48;
		reconstr_abuff->array_20[i] = v47 - 48 * v46;
	}
////////////////////////////////////////////////////////////////////////
	int16_t v51;
	int16_t v48;

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
void dss2_sub_3B8740(int32_t *array14_stage1, const struct struc_1 *a2) {
	int i;

	for (i = 0; i < 14; i++)
		array14_stage1[i] = g_unc_array_3C84F0[i][a2->array14_stage0[i]];

	if (word_3D041C)
		array14_stage1[13] = g_unc_array_3C8870[a2->array14_stage0[13]];
}

void dss2_sub_3B8410(struct struc_6 *struc_6_a1,
		struct struc_8 *struc_6_stg2_a2) {
	struct struc_8 *struc_6_v2; // ecx@1
	int v3; // esi@1
	int v5; // ebx@2
	signed int v6; // eax@2
	int v7; // esi@3
	int v8; // edi@3
	int v9; // edx@3
	int v10; // edx@3
	int v11; // edx@5
	int v12; // edx@5
	struct struc_6 *struc_6_v14; // ebx@11
	int v14; // ebp@11
	int v15; // esi@12
	signed int v16; // esi@13
	int v17; // ebp@15
	int v18; // eax@15
	int v19; // edi@15
	int v20; // edx@15
	int v21; // edx@15
	int v22; // edx@18
	int v23; // edx@18
	int v24; // zf@25
	int v25; // [sp+10h] [bp-14h]@1
	int v26; // [sp+10h] [bp-14h]@11
	struct struc_6 *struc_6_v29; // [sp+14h] [bp-10h]@11
	signed int v28; // [sp+18h] [bp-Ch]@11
	int v29; // [sp+1Ch] [bp-8h]@12
	int v30; // [sp+20h] [bp-4h]@12
	int32_t *array14a; // [sp+28h] [bp+4h]@11
	int counter; // [sp+2Ch] [bp+8h]@2
	signed int struc_6_a2b; // [sp+2Ch] [bp+8h]@12
	int tmp;

	struc_6_v2 = struc_6_stg2_a2;
	v3 = 0;
	word_3D9B7C = 0;
	struc_6_stg2_a2->field_0 = 0x2000u;
	while (1) {
		v5 = v3;
		v6 = v3 + 1;
		struc_6_v2->array14_stage2[v3] = struc_6_a1->array14_stage1[v3] >> 2;
		if (v6 / 2 >= 1)
			break;
		LABEL_9: ++v3;
		if (v3 >= 14)
			return;
	}

	/////////////////////////////
	counter = 1;
	while (1) {
		v7 = struc_6_v2->array14_stage2[counter - 1];
		// 4, 4, 4
		v8 = struc_6_v2->array14_stage2[v5 - counter];
		// 8, 8, c
		tmp = (struc_6_a1->array14_stage1[v5] * v8 + (v7 << 15) + 0x4000) >> 15;
		struc_6_v2->array14_stage2[counter - 1] = tmp;
		tmp &= 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000)
			break;

		tmp = (struc_6_a1->array14_stage1[v5] * v7 + (v8 << 15) + 0x4000) >> 15;
		struc_6_v2->array14_stage2[v5 - counter] = tmp;
		tmp &= 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000)
			break;

		++counter;
		if (counter > v6 / 2)
			goto LABEL_9;
	}
	//////////////////////////////////
	// looks like sort of cleanup part. In case "tmp != 0xFFFF8000" check will fail.
	// or may be not used at all. With my test file it was never triggered
#if 0
	struc_6_v14 = struc_6_a1;
	v14 = 0;
	word_3D9B7C = 1;
	struc_6_v2->gain = 0x1000;
	v26 = 0;
	struc_6_v29 = struc_6_a1;
	array14a = struc_6_v2->array14_stage2;
	v28 = 14;
	do
	{
		v15 = v14 + 1;
		*array14a = struc_6_v14->array14_stage1[0] >> 3;
		counter = 1;
		v30 = v14 + 1;
		v29 = (v14 + 1) / 2;
		if ( (v14 + 1) / 2 >= 1 )
		{
			v16 = 1;
			while ( 1 )
			{
				v17 = v14 - v16;
				v18 = struc_6_v2->array14_stage2[v16 - 1];
				v19 = struc_6_v2->array14_stage2[v17];
				v20 = (v19 * struc_6_v14->array14_stage1[0] + (struc_6_v2->array14_stage2[v16 - 1] << 15) + 0x4000) >> 15;
				struc_6_v2->array14_stage2[v16 - 1] = v20;
				v21 = v20 & 0xFFFF8000;
				if ( v21 && v21 != 0xFFFF8000 )
				struc_6_v2->array14_stage2[v16 - 1] = ((v21 <= 0) - 1) - 0x8000;
				struc_6_v14 = struc_6_v29;
				v22 = (v18 * struc_6_v29->array14_stage1[0] + (v19 << 15) + 0x4000) >> 15;
				struc_6_v2->array14_stage2[v17] = v22;
				v23 = v22 & 0xFFFF8000;
				if ( v23 && v23 != 0xFFFF8000 )
				{
					if ( v23 <= 0 )
					struc_6_v2->array14_stage2[v17] = 0xFFFF8000;
					else
					struc_6_v2->array14_stage2[v17] = 0x7FFF;
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
void dss2_add_pulses(int32_t *array72_a1, const struct dss2_subframe *sf) {
	int i;

	//looks like "output[sf->pulse_pos[i]] += g_gains[sf->gain] * g_pulse_val[sf->pulse_val[i]] + 0x4000 >> 15;"
	for (i = 0; i < 7; i++)
		array72_a1[sf->pulse_pos[i]] += (dss2_fixed_cb_gain[sf->gain]
				* dss2_pulse_val[sf->pulse_val[i]] + 0x4000) >> 15;

}

void dss2_sub_3B9080(int32_t *array72, int32_t *array36, int a3, int a4) {

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

void dss2_normalize(int32_t *array_a1, int normalize_bits, int array_a1_size) {
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

void dss2_sub_3B9FB0(int32_t *array72, int32_t *arrayXX) {
	int i;

	for (i = 0; i < 114; i++)
		arrayXX[114 - i] = arrayXX[186 - i];

	for (i = 0; i < 72; i++)
		arrayXX[72 - i] = array72[i];
}

void dss2_shift_sq_sub(const int32_t *array_a1, int32_t *array_a2,
		int32_t *array_a3_dst) {
	int a, shift;

	shift = 13 - word_3D9B7C;

	for (a = 0; a < 72; a++) {
		int i, tmp;

		tmp = array_a3_dst[a] * array_a1[0];

		for (i = 14; i > 0; i--)
			tmp -= array_a2[i] * array_a1[i];

		/* original code overwrite array_a2[1] two times - makes no sense for me. */
		for (i = 14; i > 1; i--)
			array_a2[i] = array_a2[i - 1];

		tmp = (tmp + 4096) >> shift;

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

void dss2_shift_sq_add(const int32_t *array_a1, int32_t *array_a2,
		int32_t *array_a3_dst) {
	int a, shift;

	shift = 13 - word_3D9B7C;
	for (a = 0; a < 72; a++) {
		int i, tmp;

		array_a2[0] = array_a3_dst[a];

		for (i = 14; i >= 0; i--)
			tmp += array_a2[i] * array_a1[i];

		for (i = 14; i > 0; i--)
			array_a2[i] = array_a2[i - 1];

		tmp = (tmp + 4096) >> shift;

		array_a3_dst[a] = tmp;
		tmp &= 0xFFFF8000;
		if (tmp && tmp != 0xFFFF8000)
			array_a3_dst[a] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
	}
}

void dss2_vec_mult(const int32_t *array15_ro_src, int32_t *array15_dst,
		const int32_t *array15_ro_a3) {
	int i;

	array15_dst[0] = array15_ro_src[0];

	/* TODO: pseudocode and my result are too different. May be my version is wrong */
	for (i = 1; i < 14; i++)
		array15_dst[i] = (array15_ro_src[i] * array15_ro_a3[i] + 0x4000) >> 15;
}

int dss2_get_normalize_bits(int32_t *array_var, int16_t size) {
	unsigned int val;
	int max_val;
	int i;

	val = 1;
	if (size)
		for (i = 0; i < size; i++)
			val |= abs(array_var[i]);

	for (max_val = 0;
			(val & 0xFFFFC000) == 0 | (val & 0xFFFFC000) == 0xFFFFC000;
			++max_val)
		val *= 2;
	return max_val;
}

void dss2_sub_3B80F0(int32_t a0, int32_t *array15_a1, int32_t *array72_a3,
		int32_t *array72_a4, int size) {

	int32_t local_rw_array15_v1a[15];
	int32_t local_rw_array15_v39[15];
	int32_t local_rw_array_v41[73];
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
	dss2_normalize(g_unc_rw_array15_3D0420, normalize_bits, 15);
	dss2_normalize(g_unc_rw_array15_3D045C, normalize_bits, 15);

	v36 = g_unc_rw_array15_3D045C[1];

	dss2_vec_mult(array15_a1, local_rw_array15_v39, g_unc_ro_array37_3C845C);
	dss2_shift_sq_add(local_rw_array15_v39, g_unc_rw_array15_3D0420,
			array72_a3);

	dss2_vec_mult(array15_a1, local_rw_array15_v1a, g_unc_ro_array15_3C8420);
	dss2_shift_sq_sub(local_rw_array15_v1a, g_unc_rw_array15_3D045C,
			array72_a3);

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
	dss2_normalize(g_unc_rw_array15_3D0420, -normalize_bits, 15);
	dss2_normalize(g_unc_rw_array15_3D045C, -normalize_bits, 15);

	v18 = 0;
	if (size > 0)
		for (i = 0; i < size; i++)
			v18 += abs(array72_a3[i]);

	if (v18 & 0xFFFFFFC0)
		v22 = (v34 << 11) / v18;
	else
		v22 = 1;

	v23 = 409 * v22 >> 15 << 15;
	tmp = (v23 + 32358 * dword_3D0498) >> 15;
	local_rw_array_v41[0] = tmp;
	tmp &= 0xFFFF8000;
	if (tmp && tmp != 0xFFFF8000)
		local_rw_array_v41[0] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;

	if (size > 1) {
		for (i = 1; i < size - 1; i++) {
			tmp = (v23 + 32358 * local_rw_array_v41[i - 1]) >> 15;
			local_rw_array_v41[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000)
				local_rw_array_v41[i] = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
		}
	}

	dword_3D0498 = local_rw_array_v41[size];
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

int16_t dss2_sub_3BA000(int16_t *a1) {
	int16_t tmp;

	tmp = *a1 + ((*a1 << 6) + *a1) * 8 + 0x103;
	*a1 = tmp;
	return tmp;
}

void dss2_sub_3B98D0(int32_t *array72_a1) {
	int v1;

	signed int v10;

	int v12;
	int i, offset, counter;

	v1 = 0;

	for (i = 0; i < 6; i++)
		g_unc_rw_array288_3D0DC0[i] = g_unc_rw_array288_3D0DC0[288 + i];

	for (i = 0; i < 72; i++)
		g_unc_rw_array288_3D0DC0[6 + i] = array72_a1[i];

	offset = 6;
	counter = 0;
	do {
		v10 = 0;
		for (i = 0; i < 6; i++)
			v10 += g_unc_rw_array288_3D0DC0[offset--]
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
	} while (offset < sizeof(g_unc_rw_array288_3D0DC0) / sizeof(int32_t));
}

static void dss2_32to16bit(int16_t *dst, int32_t *src, int size) {
	int i;

	if (!size)
		return;

	for (i = 0; i < size; i++)
		dst[i] = src[i];
}

int dss2_2_sub_3B8790(int8_t *abuff_swap, int *some_ptr_a2, int *dec_flag,
		int **abuff, int param_a5, struct struc_4 *a6, int *a7, int a8,
		float *a9, signed int *a10, unsigned int a11) {
	__int8 *v13; // edx@3
	struct struc_1 *v14; // ecx@3

	int *v36; // edx@33
	__int16 v37; // bp@33
	int v38; // eax@33

	signed int *v40; // esi@35
	int *v41; // eax@37
	double v42; // st7@38
	signed int v43; // eax@49
	__int16 v44; // ax@50

	struct struc_1 struc_1_v45; // [sp-C4h] [bp-620h]@16
	struct struc_1 *struc_1_v46; // [sp-C0h] [bp-61Ch]@12
	int v48; // [sp-24h] [bp-580h]@50
	int v49; // [sp-20h] [bp-57Ch]@52
	int v50; // [sp-18h] [bp-574h]@3
	int v51; // [sp-14h] [bp-570h]@3
	int v52; // [sp-10h] [bp-56Ch]@3
	int *v53; // [sp-Ch] [bp-568h]@50
	void *v54; // [sp-8h] [bp-564h]@5
	void *abuff_swap_v94; // [sp-4h] [bp-560h]@5
	int64_t v56; // [sp+10h] [bp-54Ch]@14

	struct struc_1 struc_1_v96; // [sp+1Ch] [bp-540h]@5
	int32_t local_rw_array72_v101[SUBFRAMES][72];
	int i, tmp, sf_idx;

	if (*dec_flag & 0x10) {
		dss2_32to16bit(&some_ptr_a2[132 * param_a5], g_unc_rw_array_3D04A0,
				264);
		*dec_flag &= ~0x10;
		return 0;
	}

	memcpy(&v50, &a6, 24u);

	dss2_byte_swap(abuff_swap, abuff, dec_flag);

	if (*dec_flag & 0x1) {
		g_unc_clean_array_3B9060();
		*dec_flag &= ~0x1;
		word_3D0C26 = 1;
	}

	abuff_swap_v94 = abuff_swap;
	v54 = &struc_1_v96;
	dss2_unpack_coeffs(v14, v13);

	if (word_3D041C) {
		if (word_3D1264)
			g_unc_rw_array14_stg1_3D0D64.field_38 = 0;
		else {
			if (g_unc_rw_array14_stg1_3D0D64.field_38++ <= 60) {
				if (g_unc_rw_array14_stg1_3D0D64.field_38 == 1) {
					memcpy(struc_1_v96.array14_stage0, &array14_3D0DA4,
							sizeof(struc_1_v96.array14_stage0));
				} else
					g_unc_rw_array14_stg1_3D0D64.field_38 = 60;
			}
		}
	}

	memcpy(&struc_1_v46, &struc_1_v96, 192u);
	dss2_sub_3B8740(&g_unc_rw_array14_stg1_3D0D64, struc_1_v46);

	if (g_unc_rw_array14_stg1_3D0D64.field_38 == 1)
		word_3D9B7E = struc_1_v96.array14_stage0[0];

	dss2_sub_3B8410(&g_unc_rw_array14_stg1_3D0D64,
			&g_unc_rw_array15_stg2_3D08C0);

////////
	for (sf_idx = 0; sf_idx < SUBFRAMES; sf_idx++) {
		if (word_3D1264) {
			dss2_sub_3B9080(g_unc_rw_array72_3D0C44, g_unc_rw_arrayXX_3D08FC,
					struc_1_v96.filed_1e,
					g_unc_array_3C88F8[struc_1_v96.subframe_something[sf_idx]]);

			abuff_swap_v94 = sf_idx;

			dss2_add_pulses(g_unc_rw_array72_3D0C44, &struc_1_v96.sf[sf_idx]);
		} else {
			for (i = 0; i < 72; i++)
				g_unc_rw_array72_3D0C44[i] =
						(g_unc_array_3C8938[struc_1_v96.subframe_something[sf_idx]]
								* dss2_sub_3BA000(&word_3D9B7E)) >> 14;

		}
		dss2_sub_3B9FB0(g_unc_rw_array72_3D0C44, g_unc_rw_arrayXX_3D08FC);

		/* swap and copy buffer */
		for (i = 0; i < 72; i++)
			g_unc_rw_array72_3D0C44[i] = g_unc_rw_arrayXX_3D08FC[71 - i];

		/* TODO: find what happens with g_unc_rw_array15_3D0BE8 */
		dss2_shift_sq_sub(&g_unc_rw_array15_stg2_3D08C0,
				&g_unc_rw_array15_3D0BE8, g_unc_rw_array72_3D0C44);

		for (i = 0; i < 72; i++) {
			tmp = (((dword_3D0DA0 << 13) - dword_3D0DA0)
					+ (g_unc_rw_array72_3D0C44[i] << 15) + 0x4000) >> 15;
			g_unc_rw_array72_3D0C44[i] = tmp;
			tmp &= 0xFFFF8000;
			if (tmp && tmp != 0xFFFF8000) {
				dword_3D0DA0 = (((tmp <= 0) - 1) & 0xFFFE) - 0x7FFF;
				g_unc_rw_array72_3D0C44[i] = dword_3D0DA0;
			}
		}

		if (*dec_flag & 0x20000)
			dss2_sub_3B80F0(g_unc_rw_array14_stg1_3D0D64.array14_stage1[0],
					&g_unc_rw_array15_stg2_3D08C0, g_unc_rw_array72_3D0C44,
					&local_rw_array72_v101[sf_idx][0], 72);
		else
			memcpy(&local_rw_array72_v101[sf_idx][0], g_unc_rw_array72_3D0C44,
					72 * sizeof(int32_t));

	};
////////
	int v39;
	if (!(*dec_flag & 0x40000))
		dss2_sub_3B98D0(local_rw_array72_v101);
	v36 = dec_flag;
	v37 = 0;
	v38 = *dec_flag;
	if (!(*dec_flag & 0x20)) {
		if (!(*dec_flag & 0x40000)) {
			v39 = a11;
			v40 = a10;
			if (!a11 && *a10 != (v38 & 0xFF00)) {
				v41 = a7;
				switch ((*a10 >> 11) & 0x1F) {
				case 0:
					v39 = *a7;
					v56 = v39;
					v42 = (double) v39 * 24.0;
					goto LABEL_47;
				case 1:
					v56 = (unsigned int) *a7;
					v42 = (double) v56 * 30.0;
					goto LABEL_47;
				case 2:
					v39 = *a7;
					v56 = v39;
					v42 = (double) v39 * 31.92;
					goto LABEL_47;
				case 3:
					v56 = (unsigned int) *a7;
					v42 = (double) v56 * 36.0;
					goto LABEL_47;
				case 4:
					v39 = *a7;
					v56 = v39;
					v42 = (double) v39 * 48.0;
					goto LABEL_47;
				case 0x11:
					v56 = (unsigned int) *a7;
					v42 = (double) v56 * 19.200001;
					goto LABEL_47;
				case 0x12:
					v39 = *a7;
					v56 = v39;
					v42 = (double) v39 * 18.0;
					goto LABEL_47;
				case 0x13:
					v56 = (unsigned int) *a7;
					v42 = (double) v56 * 15.84;
					goto LABEL_47;
				case 0x14:
					v39 = *a7;
					v56 = v39;
					v42 = (double) v39 * 12.0;
					LABEL_47: *a9 = v42 + *a9;
					break;
				default:
					break;
				}
				v36 = dec_flag;
				*v41 = 0;
				*v40 = *dec_flag & 0xFF00;
			}
			v43 = *v40;
			if (v43 & 0x80) {
				memcpy(&v50, &a6, 24u);
				v48 = (v43 >> 11) & 7;
				v44 = g_unc_sub_3B9CA0(v48, local_rw_array72_v101, v36, v50,
						v51, v52, v53, v54, abuff_swap_v94);
			} else {
				if (!(v43 & 0x38))
					goto LABEL_54;
				v49 = (v43 >> 11) & 7;
				memcpy(&v50, &a6, 24u);
				v44 = g_unc_sub_3B9990(v49, (int) local_rw_array72_v101, v50,
						v51, v52, (int) v53, (int) v54,
						(signed int *) abuff_swap_v94);
			}
			v37 = v44;
			if (v44) {
				LABEL_55: memcpy(&array14_3D0DA4, struc_1_v96.array14_stage0,
						28u);
				return v37;
			}
			LABEL_54: dss2_32to16bit(&some_ptr_a2[132 * param_a5],
					local_rw_array72_v101, 264);
			goto LABEL_55;
		}
		goto LABEL_58;
	}

	if (v38 & 0x40000) {
		LABEL_58: dss2_32to16bit(&some_ptr_a2[144 * param_a5],
				local_rw_array72_v101, 288);
	} else
		dss2_32to16bit(&some_ptr_a2[132 * param_a5], local_rw_array72_v101,
				264);

	memcpy(&array14_3D0DA4, struc_1_v96.array14_stage0, 28u);
	return 0;
}

int main(void) {
	int8_t *abuff_swap;
	int *some_ptr_a2;
	int *dec_flag;
	int *abuff;
	int param_a5;
	struct struc_4 *a6;
	int *a7;
	int a8;
	float *a9;
	signed int *a10;
	unsigned int a11;

	dss2_2_sub_3B8790(abuff_swap, some_ptr_a2, dec_flag, &abuff, param_a5, a6,
			a7, a8, a9, a10, a11);
	return 0;
}
