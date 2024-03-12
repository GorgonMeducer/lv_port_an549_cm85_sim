/*
 * Copyright (c) 2017-2024 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "perf_counter.h"
#include "platform.h"


#if (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
#include <arm_mve.h>
#endif


#include "arm_math.h"
#include "arm_math_f16.h"


#define REF_FIR_COEF        32
#define REF_FIR_SAMPLE      256

#define REF_MAT_DIM         32

#define REF_CMPLX_MUL_SIZE  256


__attribute__ ((aligned(16)))
float32_t       matA[(64 * 64)];
__attribute__ ((aligned(16)))
float32_t       matB[(64 * 64)];
__attribute__ ((aligned(16)))
float32_t       matD[(64 * 64) + 8] = { 0 };


__attribute__ ((aligned(16)))
float32_t       vecA[512];
__attribute__ ((aligned(16)))
float32_t       vecB[512];
__attribute__ ((aligned(16)))
float32_t       vecD[512];


__attribute__ ((aligned(16)))
float32_t       firInput[512], *pInput;

__attribute__ ((aligned(16)))
float32_t       firStateSCALAR[512 + 512 + 8] = { 0 };

__attribute__ ((aligned(16)))
float32_t       firDst[512 + 8] = { 0 };

__attribute__ ((aligned(16)))
q7_t            firCoeffs[512];


void test_mat_mult_q7(int numRowsA, int numColsA, int numColsB)
{
    int             numRowsB = numColsA;
    arm_matrix_instance_q7 SrcA, SrcB;
    arm_matrix_instance_q7 DstMatSclr;
    int             alignOffs = 0;
    q7_t           *pmatA = (q7_t *) matA;
    q7_t           *pmatB = (q7_t *) matB;
    q7_t           *pmatD = (q7_t *) matD + alignOffs;

    SrcA.numRows = numRowsA;
    SrcA.numCols = numColsA;
    SrcA.pData = pmatA;
    SrcB.numRows = numRowsB;
    SrcB.numCols = numColsB;
    SrcB.pData = pmatB;
    DstMatSclr.numRows = numRowsA;
    DstMatSclr.numCols = numColsB;
    DstMatSclr.pData = pmatD;

    q7_t            state[SrcB.numRows * SrcB.numCols];

    int64_t        tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_mat_mult_q7(&SrcA, &SrcB, &DstMatSclr, state);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_mat_mult_q15(int numRowsA, int numColsA, int numColsB)
{
    int             numRowsB = numColsA;
    arm_matrix_instance_q15 SrcA, SrcB;
    arm_matrix_instance_q15 DstMatSclr;
    int             alignOffs = 0;
    q15_t          *pmatA = (q15_t *) matA;
    q15_t          *pmatB = (q15_t *) matB;
    q15_t          *pmatD = (q15_t *) matD + alignOffs;

    SrcA.numRows = numRowsA;
    SrcA.numCols = numColsA;
    SrcA.pData = pmatA;
    SrcB.numRows = numRowsB;
    SrcB.numCols = numColsB;
    SrcB.pData = pmatB;
    DstMatSclr.numRows = numRowsA;
    DstMatSclr.numCols = numColsB;
    DstMatSclr.pData = pmatD;

    //q15_t           state[SrcB.numRows * SrcB.numCols];
    q15_t *state = malloc(sizeof(q15_t) * SrcB.numRows * SrcB.numCols);
    assert(NULL != state);

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {
    
        arm_mat_mult_q15(&SrcA, &SrcB, &DstMatSclr, state);
    
    }

    free(state);

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_mat_mult_q31(int numRowsA, int numColsA, int numColsB)
{
    int             numRowsB = numColsA;
    arm_matrix_instance_q31 SrcA, SrcB;
    arm_matrix_instance_q31 DstMatSclr;
    int             alignOffs = 0;
    q31_t          *pmatA = (q31_t *) matA;
    q31_t          *pmatB = (q31_t *) matB;
    q31_t          *pmatD = (q31_t *) matD + alignOffs;

    SrcA.numRows = numRowsA;
    SrcA.numCols = numColsA;
    SrcA.pData = pmatA;
    SrcB.numRows = numRowsB;
    SrcB.numCols = numColsB;
    SrcB.pData = pmatB;
    DstMatSclr.numRows = numRowsA;
    DstMatSclr.numCols = numColsB;
    DstMatSclr.pData = pmatD;


    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_mat_mult_q31(&SrcA, &SrcB, &DstMatSclr);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_mat_mult_f16(int numRowsA, int numColsA, int numColsB)
{
    int             numRowsB = numColsA;
    arm_matrix_instance_f16 SrcA, SrcB;
    arm_matrix_instance_f16 DstMatSclr;

    int             alignOffs = 0;
    float16_t      *pmatA = (float16_t *) matA;
    float16_t      *pmatB = (float16_t *) matB;
    float16_t      *pmatD = (float16_t *) matD + alignOffs;

    SrcA.numRows = numRowsA;
    SrcA.numCols = numColsA;
    SrcA.pData = pmatA;
    SrcB.numRows = numRowsB;
    SrcB.numCols = numColsB;
    SrcB.pData = pmatB;
    DstMatSclr.numRows = numRowsA;
    DstMatSclr.numCols = numColsB;
    DstMatSclr.pData = pmatD;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_mat_mult_f16(&SrcA, &SrcB, &DstMatSclr);
    
    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_mat_mult_f32(int numRowsA, int numColsA, int numColsB)
{
    int             numRowsB = numColsA;
    arm_matrix_instance_f32 SrcA, SrcB;
    arm_matrix_instance_f32 DstMatSclr;

    int             alignOffs = 0;
    float32_t      *pmatA = (float32_t *) matA;
    float32_t      *pmatB = (float32_t *) matB;
    float32_t      *pmatD = (float32_t *) matD + alignOffs;

    SrcA.numRows = numRowsA;
    SrcA.numCols = numColsA;
    SrcA.pData = pmatA;
    SrcB.numRows = numRowsB;
    SrcB.numCols = numColsB;
    SrcB.pData = pmatB;
    DstMatSclr.numRows = numRowsA;
    DstMatSclr.numCols = numColsB;
    DstMatSclr.pData = pmatD;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_mat_mult_f32(&SrcA, &SrcB, &DstMatSclr);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}





void test_fir_q7(int numTaps, int blockSize)
{

    q7_t           *pCoeffs = (q7_t *) firCoeffs;
    q7_t           *pInput;
    int             alignOffs = 0;
    arm_fir_instance_q7 SSCALAR;
    q7_t           *pfirStSCALAR;
    q7_t           *pfirDst;


    pfirStSCALAR = (q7_t *) firStateSCALAR + alignOffs;;

    pInput = (q7_t *) firInput;
    pfirDst = (q7_t *) firDst + alignOffs;

    arm_fir_init_q7(&SSCALAR, numTaps, pCoeffs, pfirStSCALAR, blockSize);


    q7_t           *p = &pCoeffs[numTaps];
    for (int i = 0; i < 16 - (numTaps & 0xf); i++)
        *p++ = 0;;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_fir_q7(&SSCALAR, pInput, pfirDst, blockSize);
    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_fir_q15(int numTaps, int blockSize)
{

    q15_t          *pCoeffs = (q15_t *) firCoeffs;
    q15_t          *pInput;
    int             alignOffs = 0;
    arm_fir_instance_q15 SSCALAR;
    q15_t          *pfirStSCALAR;
    q15_t          *pfirDst;



    pfirStSCALAR = (q15_t *) firStateSCALAR + alignOffs;;

    pInput = (q15_t *) firInput;
    pfirDst = (q15_t *) firDst + alignOffs;

    arm_fir_init_q15(&SSCALAR, numTaps, pCoeffs, pfirStSCALAR, blockSize);


    q15_t          *p = &pCoeffs[numTaps];
    for (int i = 0; i < 16 - (numTaps & 0xf); i++)
        *p++ = 0;;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_fir_q15(&SSCALAR, pInput, pfirDst, blockSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_fir_q31(int numTaps, int blockSize)
{

    q31_t          *pCoeffs = (q31_t *) firCoeffs;
    q31_t          *pInput;
    int             alignOffs = 0;
    arm_fir_instance_q31 SSCALAR;
    q31_t          *pfirStSCALAR;
    q31_t          *pfirDst;

    pfirStSCALAR = (q31_t *) firStateSCALAR + alignOffs;;

    pInput = (q31_t *) firInput;
    pfirDst = (q31_t *) firDst + alignOffs;

    arm_fir_init_q31(&SSCALAR, numTaps, pCoeffs, pfirStSCALAR, blockSize);


    q31_t          *p = &pCoeffs[numTaps];
    for (int i = 0; i < 16 - (numTaps & 0xf); i++)
        *p++ = 0;;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_fir_q31(&SSCALAR, pInput, pfirDst, blockSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_fir_f16(int numTaps, int blockSize)
{

    float16_t      *pCoeffs = (float16_t *) firCoeffs;
    float16_t      *pInput;
    int             alignOffs = 0;
    arm_fir_instance_f16 SSCALAR;
    float16_t      *pfirStSCALAR;
    float16_t      *pfirDst;



    pfirStSCALAR = (float16_t *) firStateSCALAR + alignOffs;;

    pInput = (float16_t *) firInput;
    pfirDst = (float16_t *) firDst + alignOffs;

    arm_fir_init_f16(&SSCALAR, numTaps, pCoeffs, pfirStSCALAR, blockSize);


    float16_t      *p = &pCoeffs[numTaps];
    for (int i = 0; i < 16 - (numTaps & 0xf); i++)
        *p++ = 0;;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_fir_f16(&SSCALAR, pInput, pfirDst, blockSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_fir_f32(int numTaps, int blockSize)
{

    float32_t      *pCoeffs = (float32_t *) firCoeffs;
    float32_t      *pInput;
    int             alignOffs = 0;
    arm_fir_instance_f32 SSCALAR;
    float32_t      *pfirStSCALAR;
    float32_t      *pfirDst;



    pfirStSCALAR = (float32_t *) firStateSCALAR + alignOffs;;

    pInput = (float32_t *) firInput;
    pfirDst = (float32_t *) firDst + alignOffs;

    arm_fir_init_f32(&SSCALAR, numTaps, pCoeffs, pfirStSCALAR, blockSize);


    float32_t      *p = &pCoeffs[numTaps];
    for (int i = 0; i < 16 - (numTaps & 0xf); i++)
        *p++ = 0;;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_fir_f32(&SSCALAR, pInput, pfirDst, blockSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}




void test_vec_cmplx_mult_q15(int vectorSize)
{
    int             alignOffs = 0;
    q15_t          *pA = (q15_t *) vecA;
    q15_t          *pB = (q15_t *) vecB;
    q15_t          *pD = (q15_t *) vecD + alignOffs;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_cmplx_mult_cmplx_q15(pA, pB, pD, vectorSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_vec_cmplx_mult_q31(int vectorSize)
{
    int             alignOffs = 0;
    q31_t          *pA = (q31_t *) vecA;
    q31_t          *pB = (q31_t *) vecB;
    q31_t          *pD = (q31_t *) vecD + alignOffs;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_cmplx_mult_cmplx_q31(pA, pB, pD, vectorSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}


void test_vec_cmplx_mult_f16(int vectorSize)
{
    int             alignOffs = 0;
    float16_t      *pA = (float16_t *) vecA;
    float16_t      *pB = (float16_t *) vecB;
    float16_t      *pD = (float16_t *) vecD + alignOffs;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_cmplx_mult_cmplx_f16(pA, pB, pD, vectorSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_vec_cmplx_mult_f32(int vectorSize)
{
    int             alignOffs = 0;
    float32_t      *pA = (float32_t *) vecA;
    float32_t      *pB = (float32_t *) vecB;
    float32_t      *pD = (float32_t *) vecD + alignOffs;

    int64_t tick = get_system_ticks();
    __cycleof__("", {tick = __cycle_count__;}) {

        arm_cmplx_mult_cmplx_f32(pA, pB, pD, vectorSize);

    }

    printf("%s, %lld\n", __FUNCTION__, tick);
    fflush(&__stdout);
}

void test_entry(void)
{
    printf("\r\nTest, Cycles\r\n");
    
    test_vec_cmplx_mult_q15(REF_CMPLX_MUL_SIZE);
    test_vec_cmplx_mult_q31(REF_CMPLX_MUL_SIZE);
    test_vec_cmplx_mult_f16(REF_CMPLX_MUL_SIZE);
    test_vec_cmplx_mult_f32(REF_CMPLX_MUL_SIZE);

    test_fir_q7(REF_FIR_COEF, REF_FIR_SAMPLE);
    test_fir_q15(REF_FIR_COEF, REF_FIR_SAMPLE);
    test_fir_q31(REF_FIR_COEF, REF_FIR_SAMPLE);
    test_fir_f16(REF_FIR_COEF, REF_FIR_SAMPLE);
    test_fir_f32(REF_FIR_COEF, REF_FIR_SAMPLE);

    test_mat_mult_q7(REF_MAT_DIM, REF_MAT_DIM, REF_MAT_DIM);
    test_mat_mult_q15(REF_MAT_DIM, REF_MAT_DIM, REF_MAT_DIM);
    test_mat_mult_q31(REF_MAT_DIM, REF_MAT_DIM, REF_MAT_DIM);
    test_mat_mult_f16(REF_MAT_DIM, REF_MAT_DIM, REF_MAT_DIM);
    test_mat_mult_f32(REF_MAT_DIM, REF_MAT_DIM, REF_MAT_DIM);
    
    printf("\r\n End of Test \r\n");

}
