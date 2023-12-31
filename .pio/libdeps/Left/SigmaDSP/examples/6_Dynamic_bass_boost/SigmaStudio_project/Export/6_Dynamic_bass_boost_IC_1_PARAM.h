/*
 *
 *
 * Created:        Sunday, February 16, 2020 12:13:46 PM
 * Description:    6_Dynamic_bass_boost:IC 1 parameter RAM definitions.
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
#ifndef __6_DYNAMIC_BASS_BOOST_IC_1_PARAM_H__
#define __6_DYNAMIC_BASS_BOOST_IC_1_PARAM_H__


/* Module Dynamic_bass - Dynamic Enhancement*/
#define MOD_DYNAMIC_BASS_COUNT                         9
#define MOD_DYNAMIC_BASS_DEVICE                        "IC1"
#define MOD_DYNAMIC_BASS_ALG0_FREQUENCY_ADDR           0
#define MOD_DYNAMIC_BASS_ALG0_FREQUENCY_FIXPT          0x0001015B
#define MOD_DYNAMIC_BASS_ALG0_FREQUENCY_VALUE          SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00785396144863598)
#define MOD_DYNAMIC_BASS_ALG0_FREQUENCY_TYPE           SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_B0_ADDR                  1
#define MOD_DYNAMIC_BASS_ALG0_B0_FIXPT                 0x000001FF
#define MOD_DYNAMIC_BASS_ALG0_B0_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(6.10060775445649E-05)
#define MOD_DYNAMIC_BASS_ALG0_B0_TYPE                  SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_B1_ADDR                  2
#define MOD_DYNAMIC_BASS_ALG0_B1_FIXPT                 0x000003FF
#define MOD_DYNAMIC_BASS_ALG0_B1_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00012201215508913)
#define MOD_DYNAMIC_BASS_ALG0_B1_TYPE                  SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_B2_ADDR                  3
#define MOD_DYNAMIC_BASS_ALG0_B2_FIXPT                 0x000001FF
#define MOD_DYNAMIC_BASS_ALG0_B2_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(6.10060775445649E-05)
#define MOD_DYNAMIC_BASS_ALG0_B2_TYPE                  SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_A1_ADDR                  4
#define MOD_DYNAMIC_BASS_ALG0_A1_FIXPT                 0x00FD27FF
#define MOD_DYNAMIC_BASS_ALG0_A1_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.97778320249124)
#define MOD_DYNAMIC_BASS_ALG0_A1_TYPE                  SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_A2_ADDR                  5
#define MOD_DYNAMIC_BASS_ALG0_A2_FIXPT                 0xFF82D001
#define MOD_DYNAMIC_BASS_ALG0_A2_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.978027226801421)
#define MOD_DYNAMIC_BASS_ALG0_A2_TYPE                  SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_TIMECONSTANT_ADDR        6
#define MOD_DYNAMIC_BASS_ALG0_TIMECONSTANT_FIXPT       0x000006C7
#define MOD_DYNAMIC_BASS_ALG0_TIMECONSTANT_VALUE       SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.000206906044701749)
#define MOD_DYNAMIC_BASS_ALG0_TIMECONSTANT_TYPE        SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_THRESHOLD_ADDR           7
#define MOD_DYNAMIC_BASS_ALG0_THRESHOLD_FIXPT          0x00C51C4F
#define MOD_DYNAMIC_BASS_ALG0_THRESHOLD_VALUE          SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.53992652605949)
#define MOD_DYNAMIC_BASS_ALG0_THRESHOLD_TYPE           SIGMASTUDIOTYPE_FIXPOINT
#define MOD_DYNAMIC_BASS_ALG0_LOWLEVELBOOST_ADDR       8
#define MOD_DYNAMIC_BASS_ALG0_LOWLEVELBOOST_FIXPT      0x00800000
#define MOD_DYNAMIC_BASS_ALG0_LOWLEVELBOOST_VALUE      SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_DYNAMIC_BASS_ALG0_LOWLEVELBOOST_TYPE       SIGMASTUDIOTYPE_FIXPOINT

#endif
