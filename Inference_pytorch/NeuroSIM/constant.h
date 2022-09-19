/*******************************************************************************
* Copyright (c) 2015-2017
* School of Electrical, Computer and Energy Engineering, Arizona State University
* PI: Prof. Shimeng Yu
* All rights reserved.
* 
* This source code is part of NeuroSim - a device-circuit-algorithm framework to benchmark 
* neuro-inspired architectures with synaptic devices(e.g., SRAM and emerging non-volatile memory). 
* Copyright of the model is maintained by the developers, and the model is distributed under 
* the terms of the Creative Commons Attribution-NonCommercial 4.0 International Public License 
* http://creativecommons.org/licenses/by-nc/4.0/legalcode.
* The source code is free and you can redistribute and/or modify it
* by providing that the following conditions are met:
* 
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
* 
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* Developer list: 
*   Pai-Yu Chen     Email: pchen72 at asu dot edu 
*                    
*   Xiaochen Peng   Email: xpeng15 at asu dot edu
********************************************************************************/

// separate cases based on technology node

#ifndef CONSTANT_H_
#define CONSTANT_H_

#define INV     0
#define NOR     1
#define NAND    2

#define NMOS    0
#define PMOS    1

#define MAX_NMOS_SIZE   100
#define MIN_NMOS_SIZE   2   //1.5

#define MAX_TRANSISTOR_HEIGHT_FINFET  34
#define MAX_TRANSISTOR_HEIGHT        28 //28
// New codes for constant - modified 220916 
// 10nm and beyond: constant scaling no more applies -> use separate values for each tech node 
// TSMC + IRDS version
#define MAX_TRANSISTOR_HEIGHT_10nm 36 //TSMC
#define MAX_TRANSISTOR_HEIGHT_7nm 42.8574 //TSMC
#define MAX_TRANSISTOR_HEIGHT_5nm 45 //TSMC
#define MAX_TRANSISTOR_HEIGHT_3nm  53.3 //IRDS
#define MAX_TRANSISTOR_HEIGHT_2nm  69 //IRDS
#define MAX_TRANSISTOR_HEIGHT_1nm  88 //IRDS


/*
//IMEC Version
#define MAX_TRANSISTOR_HEIGHT_5nm 36.4
#define MAX_TRANSISTOR_HEIGHT_3nm 42
#define MAX_TRANSISTOR_HEIGHT_2nm 40
*/

// y dimension has to be changed as well - to do (220916)
#define MIN_GAP_BET_SAME_TYPE_DIFFS    1.6  //1.5

// metal pitching scaling has also reached limit, and the constant multiplying method no longer applies
#define MIN_GAP_BET_P_AND_N_DIFFS 3.5 // 3.5 //2
#define MIN_POLY_EXT_DIFF       1.0 // Minimum poly extension beyond diffusion region
#define MIN_GAP_BET_FIELD_POLY 1.6 // Field poly means the poly above the field oxide (outside the active region)

#define MIN_GAP_BET_P_AND_N_DIFFS_14nm  9.1
#define OUTER_HEIGHT_REGION_14nm  13.7
#define MIN_GAP_BET_P_AND_N_DIFFS_10nm 9.6
#define OUTER_HEIGHT_REGION_10nm 14.4
#define MIN_GAP_BET_P_AND_N_DIFFS_7nm 11.4
#define OUTER_HEIGHT_REGION_7nm 17.1
#define MIN_GAP_BET_P_AND_N_DIFFS_5nm 11.2
#define OUTER_HEIGHT_REGION_5nm 16.8
#define MIN_GAP_BET_P_AND_N_DIFFS_3nm 14
#define OUTER_HEIGHT_REGION_3nm 21
#define MIN_GAP_BET_P_AND_N_DIFFS_2nm 16
#define OUTER_HEIGHT_REGION_2nm 24
#define MIN_GAP_BET_P_AND_N_DIFFS_1nm  32 // projection 
#define OUTER_HEIGHT_REGION_1nm 48

#define POLY_WIDTH                     1.0
#define POLY_WIDTH_FINFET              1.4
#define MIN_GAP_BET_GATE_POLY          2.8  //1.5
#define MIN_GAP_BET_GATE_POLY_FINFET   3.9

// New codes for constant - modified 220916
// 10, 7nm nodes -> replace the original values (due to the disagreement with the acutal trend) : MIN_GAP_BET_GATE_POLY+POLY_WIDTH corresponds to CPP (_FINFET is added for the FinFET case)
// 5nm and beyond-> update the values following predictions (CPP scaling has been slowing down, and gate length scaling has also slowed down followingly)

#define CPP_10nm  6.4 // IMEC
#define POLY_WIDTH_10nm  2 // IRDS
#define CPP_7nm 8 // IMEC
#define POLY_WIDTH_7nm 2.57 // IRDS
#define CPP_5nm 48/5 // IMEC
#define POLY_WIDTH_5nm 4 // IRDS
#define CPP_3nm 35/3 // IMEC
#define POLY_WIDTH_3nm 6 // IRDS
#define CPP_2nm 42/2 // IMEC
#define POLY_WIDTH_2nm 7 // IRDS
#define CPP_1nm 40/1 // IMEC
#define POLY_WIDTH_1nm 7 // IRDS

#define MIN_GAP_BET_CONTACT_POLY       0.7  //0.75
#define CONTACT_SIZE                   1.3  //1
#define MIN_WIDTH_POWER_RAIL           3.4  //2


#define M2_PITCH                       3.2
#define M3_PITCH                       2.8

#define AVG_RATIO_LEAK_2INPUT_NAND 0.48
#define AVG_RATIO_LEAK_3INPUT_NAND 0.31
#define AVG_RATIO_LEAK_2INPUT_NOR  0.95
#define AVG_RATIO_LEAK_3INPUT_NOR  0.62

#define W_SENSE_P       7.5
#define W_SENSE_N       3.75
#define W_SENSE_ISO     12.5
#define W_SENSE_EN      5.0
#define W_SENSE_MUX     9.0

#define IR_DROP_TOLERANCE           0.25
#define LINEAR_REGION_RATIO         0.20

#define HEIGHT_WIDTH_RATIO_LIMIT    5

#define RATIO_READ_THRESHOLD_VS_VOLTAGE 0.2

#define ROW_MODE    0   // Connect to rows
#define COL_MODE    1   // Connect to columns



#endif /* CONSTANT_H_ */
