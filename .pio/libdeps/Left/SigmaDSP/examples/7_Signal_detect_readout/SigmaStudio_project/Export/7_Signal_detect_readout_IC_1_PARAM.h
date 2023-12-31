/*
 *
 *
 * Created:        Wednesday, February 19, 2020 9:14:02 AM
 * Description:    7_Signal_detect_readout:IC 1 parameter RAM definitions.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2020 Analog Devices, Inc. All rights reserved.
 */
#ifndef __7_SIGNAL_DETECT_READOUT_IC_1_PARAM_H__
#define __7_SIGNAL_DETECT_READOUT_IC_1_PARAM_H__


/* Module Noise_filter - Nth Order Filter*/
#define MOD_NOISE_FILTER_COUNT                         5
#define MOD_NOISE_FILTER_DEVICE                        "IC1"
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_0B_1_ADDR 0
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_0B_1_FIXPT 0x007F68B3
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_0B_1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.995382689587065)
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_0B_1_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1B_1_ADDR 1
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1B_1_FIXPT 0xFF012E9A
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1B_1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1.99076537917413)
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1B_1_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2B_1_ADDR 2
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2B_1_FIXPT 0x007F68B3
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2B_1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.995382689587065)
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2B_1_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1A_1_ADDR 3
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1A_1_FIXPT 0x00FED0B3
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1A_1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.99074405950505)
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_1A_1_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2A_1_ADDR 4
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2A_1_FIXPT 0xFF812DE7
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2A_1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.990786698843211)
#define MOD_NOISE_FILTER_ALG0_STAGE1_NTHORDERDOUBLE21_2A_1_TYPE SIGMASTUDIOTYPE_FIXPOINT

/* Module Signal_detection_right - Signal Detection*/
#define MOD_SIGNAL_DETECTION_RIGHT_COUNT               3
#define MOD_SIGNAL_DETECTION_RIGHT_DEVICE              "IC1"
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_TIMECONSTANTSIGNALDETECTALG2_ADDR 5
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_TIMECONSTANTSIGNALDETECTALG2_FIXPT 0x007FFE1B
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_TIMECONSTANTSIGNALDETECTALG2_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.999942239533362)
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_TIMECONSTANTSIGNALDETECTALG2_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_COUNTERDOWNSIGNALDETECTALG2_ADDR 6
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_COUNTERDOWNSIGNALDETECTALG2_FIXPT 0x00000346
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_COUNTERDOWNSIGNALDETECTALG2_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0001)
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_COUNTERDOWNSIGNALDETECTALG2_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_THRESHOLDSIGNALDETECTALG2_ADDR 7
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_THRESHOLDSIGNALDETECTALG2_FIXPT 0x000020C4
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_THRESHOLDSIGNALDETECTALG2_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.001)
#define MOD_SIGNAL_DETECTION_RIGHT_ALG0_THRESHOLDSIGNALDETECTALG2_TYPE SIGMASTUDIOTYPE_FIXPOINT

/* Module Signal_detection_left - Signal Detection*/
#define MOD_SIGNAL_DETECTION_LEFT_COUNT                3
#define MOD_SIGNAL_DETECTION_LEFT_DEVICE               "IC1"
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_TIMECONSTANTSIGNALDETECTALG1_ADDR 8
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_TIMECONSTANTSIGNALDETECTALG1_FIXPT 0x007FFE1B
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_TIMECONSTANTSIGNALDETECTALG1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.999942239533362)
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_TIMECONSTANTSIGNALDETECTALG1_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_COUNTERDOWNSIGNALDETECTALG1_ADDR 9
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_COUNTERDOWNSIGNALDETECTALG1_FIXPT 0x00000346
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_COUNTERDOWNSIGNALDETECTALG1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0001)
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_COUNTERDOWNSIGNALDETECTALG1_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_THRESHOLDSIGNALDETECTALG1_ADDR 10
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_THRESHOLDSIGNALDETECTALG1_FIXPT 0x000020C4
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_THRESHOLDSIGNALDETECTALG1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.001)
#define MOD_SIGNAL_DETECTION_LEFT_ALG0_THRESHOLDSIGNALDETECTALG1_TYPE SIGMASTUDIOTYPE_FIXPOINT

/* Module Signal_detection_readout_left - DSP Readback*/
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_COUNT        2
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_DEVICE       "IC1"
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL0_ADDR 2074
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL0_VALUES SIGMASTUDIOTYPE_SPECIAL(0x00EE)
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL0_TYPE SIGMASTUDIOTYPE_SPECIAL
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL0_READBACK_ADDR 0
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL1_ADDR 2074
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL1_VALUE SIGMASTUDIOTYPE_5_19_CONVERT(0)
#define MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL1_TYPE SIGMASTUDIOTYPE_5_19

/* Module Signal_detection_readout_right - DSP Readback*/
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_COUNT       2
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_DEVICE      "IC1"
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL0_ADDR 2074
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL0_VALUES SIGMASTUDIOTYPE_SPECIAL(0x00E2)
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL0_TYPE SIGMASTUDIOTYPE_SPECIAL
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL0_READBACK_ADDR 0
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL1_ADDR 2074
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL1_VALUE SIGMASTUDIOTYPE_5_19_CONVERT(0)
#define MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL1_TYPE SIGMASTUDIOTYPE_5_19

#endif
