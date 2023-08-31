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

#include <cstdio>
#include <random>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>
#include "math.h"
#include "Param.h"
#include "constant.h"

using namespace std;

Param::Param() {
	/***************************************** user defined design options and parameters *****************************************/
	operationmode = 2;     		// 1: conventionalSequential (Use several multi-bit RRAM as one synapse)
								// 2: conventionalParallel (Use several multi-bit RRAM as one synapse)
	
	memcelltype = 1;        	// 1: cell.memCellType = Type::SRAM
								// 2: cell.memCellType = Type::RRAM
								// 3: cell.memCellType = Type::FeFET
								// 4: cell.memCellType = Type::DCIM
	
	accesstype = 1;         	// 1: cell.accessType = CMOS_access
								// 2: cell.accessType = BJT_access
								// 3: cell.accessType = diode_access
								// 4: cell.accessType = none_access (Crossbar Array)
	
	transistortype = 1;     	// 1: inputParameter.transistorType = conventional
	
	deviceroadmap = 2;      	// 1: inputParameter.deviceRoadmap = HP
								// 2: inputParameter.deviceRoadmap = LSTP
	// Anni update
	globalBusType = false;		// false: X-Y Bus

	globalBufferType = false;    // false: register file
								// true: SRAM
	globalBufferCoreSizeRow = 512;// 512 ;//128;
	globalBufferCoreSizeCol = 512;//512; //128;
	
	tileBufferType = false;      // false: register file
								// true: SRAM
	tileBufferCoreSizeRow = 64;// 128 ;//32;
	tileBufferCoreSizeCol = 64;// 128 ;//32;
	
	// resnet 18 - 256, 64
	// vgg8 - 128 , 32
	// resnet 34 - 512, 64? better

	peBufferType = false;        // false: register file
								// true: SRAM
	
	chipActivation = true;      // false: activation (reLu/sigmoid) inside Tile
								// true: activation outside Tile
						 		
	reLu = true;                // false: sigmoid
								// true: reLu
								
	novelMapping = true;        // false: conventional mapping
								// true: novel mapping
								
	SARADC = false;              // false: MLSA
	                            // true: sar ADC
	currentMode = true;         // false: MLSA use VSA
	                            // true: MLSA use CSA
	
	pipeline = true;            // false: layer-by-layer process --> huge leakage energy in HP
								// true: pipeline process
	speedUpDegree = 8;          // 1 = no speed up --> original speed
								// 2 and more : speed up ratio, the higher, the faster
								// A speed-up degree upper bound: when there is no idle period during each layer --> no need to further fold the system clock
								// This idle period is defined by IFM sizes and data flow, the actual process latency of each layer may be different due to extra peripheries
	
	validated = true;			// false: no calibration factors
								// true: validated by silicon data (wiring area in layout, gate switching activity, post-layout performance drop...)
								
	synchronous = true;			// false: asynchronous
								// true: synchronous, clkFreq will be decided by sensing delay
								
	/*** algorithm weight range, the default wrapper (based on WAGE) has fixed weight range of (-1, 1) ***/
	algoWeightMax = 1;
	algoWeightMin = -1;
	
	/*** conventional hardware design options ***/
	clkFreq = 1e9;                      // Clock frequency
	temp = 300;                         // Temperature (K)
	// technode: 130	 --> wireWidth: 175
	// technode: 90		 --> wireWidth: 110
	// technode: 65      --> wireWidth: 105
	// technode: 45      --> wireWidth: 80
	// technode: 32      --> wireWidth: 56
	// technode: 22      --> wireWidth: 40
	// technode: 14      --> wireWidth: 25
	// technode: 10, 7   --> wireWidth: 18

	Activationtype=true; // true: SRAM, False: RRAM
	Currentsenseamp_fix=true;

	draincaptest=0;
	acutallayout=1;
	
	DCIM_energy_recalculated=1;
	buswidthforce=1;
	
	outputtoggle=0.25;
	newbus=1;
	// VGG8-CIFAR10: 0.18
	// ResNet34-ImageNet: 0.29
	// ResNet18-ImageNet: 0.33



const int 
tgi=5;
const int 
tech_tech=2;
	toggleindex=tgi; 

	

	parallel_weightprecision=4; // how many weight levels are input to the adder tree in one cycle
	speciallayout=1;

	toggle_enforce=0;
	if (toggle_enforce==1) {
	realtime_toggle=0;
	} else {
	realtime_toggle=1;	
	}

const int
file=1;
switch (file){
	case 0: 	
	filename = "BW4096_TCAS_DCIMFinal_ResNET18_Pipielined"; 
	globalBufferCoreSizeRow = 128;
	tileBufferCoreSizeRow  = 64;
	inputtoggle=0.33; 
	globalBufferCoreSizeCol= globalBufferCoreSizeRow;
	tileBufferCoreSizeCol= tileBufferCoreSizeRow;

	break;  
	case 1: 	
	filename="BW4096_TCAS_DCIMFinal_ResNET34_Pipelined";
	globalBufferCoreSizeRow = 256;
	tileBufferCoreSizeRow  = 64;
	inputtoggle=0.29; 
	globalBufferCoreSizeCol= globalBufferCoreSizeRow;
	tileBufferCoreSizeCol= tileBufferCoreSizeRow;
	break;   
	case 2:
	filename = "BW4096_TCAS_DCIMFinal_Vgg_Pipielined";
	globalBufferCoreSizeRow = 128;// 128;
	tileBufferCoreSizeRow  = 32;// 32;
	inputtoggle=0.18; 
	globalBufferCoreSizeCol= globalBufferCoreSizeRow;
	tileBufferCoreSizeCol= tileBufferCoreSizeRow;
	break;   
}

filename = "Add_Values_for_TCAS";


	// resnet 18 - 256, 64
	// vgg8 - 128 , 32
	// resnet 34 - 512, 64? better


//filename="wihtoutdecoder_ResNET18";
// filename = "toggle_final_14_2nm"; 

const int 
tech_mode=2;

	switch (tech_mode){
		case 0: 	
	operationmode = 2;     
	memcelltype = 1;  
	onlymainarray =0; 
	break;  
		case 1: 	
	operationmode = 1;     
	memcelltype = 4;  
	onlymainarray =0; 

	break;   
		case 2: 	
	operationmode = 2;     
	memcelltype = 2;  
	onlymainarray =0; 
	break;   
		case 3: 	
	operationmode = 1;     
	memcelltype = 4;  
	onlymainarray = 1; 
	break;  
		case 4: 	
	operationmode = 2;     
	memcelltype = 2;  
	onlymainarray =0; 
	break;   
		case 5: 	
	operationmode = 2;     
	memcelltype = 2;  
	onlymainarray =1; 
	break;  
		case 6 : 	
	operationmode = 1;     
	memcelltype = 1;  
	onlymainarray =0; 
	break;  
	}


	junctioncap_modify = 1;
	buffernumber =0;
	buffersizeratio = 5;
	sparsitycalc=1;

	const int optimize = 1;

	// 1.4 update
	sizingfactor_MUX= 1; 
	sizingfactor_WLdecoder= 1; 

	newADC =1;
	newSAEncoder  =1;
	newADCarea  =1;
	dumcolshared = 32;

const int 
tech=5;

	tolerance=1;
	repeater_enlarge=0;

	switchmatrixsizeratio_DCIM=0.3; // default

	globalBusDelayTolerance = 0.1;      // to relax bus delay for global H-Tree (chip level: communication among tiles), if tolerance is 0.1, the latency will be relax to (1+0.1)*optimalLatency (trade-off with energy)
	localBusDelayTolerance = 0.1;       // to relax bus delay for global H-Tree (tile level: communication among PEs), if tolerance is 0.1, the latency will be relax to (1+0.1)*optimalLatency (trade-off with energy)
	treeFoldedRatio = 4;                // the H-Tree is assumed to be able to folding in layout (save area)
	maxGlobalBusWidth = 4096 ;           // the max buswidth allowed on chip level (just a upper_bound, the actual bus width is defined according to the auto floorplan)
										// NOTE: Carefully choose this number!!!
										// e.g. when use pipeline with high speedUpDegree, i.e. high throughput, need to increase the global bus width (interface of global buffer) --> guarantee global buffer speed



const int 
tech_tech_b=4;

	switch (tech){
		case 0: technode = 130; break;  
		case 1:technode =  90; break;  
		case 2: technode =  65; break;  
		case 3: technode =  45; break;  
		case 4: technode =  32; break;  
		case 5: technode =  22; 
		switchmatrixsizeratio_DCIM=0.06 ; // 0.02 for DCIM
		// DCIM
		// ss -  FOM highest - 0.06
	
		if (memcelltype == 2) {	
		sizingfactor_MUX=   3400;  
		sizingfactor_MUX_Last=   3400;  
		}
		else   { 		
		sizingfactor_MUX=   110;  
		sizingfactor_MUX_Last=  110;  
		}

		newswitchmatrixsizeratio_ACIM = 14;
		switchmatrixsizeratio_ACIM=0.2;

		// ACIM-SRAM 110, 0.2 
		// ACIM-RRAM 3400, 0.2? 

		break; // 0.02
		case 6: technode =  14; 
		switchmatrixsizeratio_DCIM=0.005; 
		if ((optimize ==1) && (memcelltype == 4)) { buffernumber=0; buffersizeratio= 0;} // (3, 48)
		if ( memcelltype ==4) { 
			repeater_add=5;
			tolerance=0;
			repeater_enlarge=1;
		}
		// DCIM (sc, bn, bs) = (0.005,0,0)

		sizingfactor_MUX=  130;
		sizingfactor_MUX_Last= 130;
		switchmatrixsizeratio_ACIM=0.2;

		if ((optimize ==1) && (memcelltype == 1)) 	{ buffernumber = 0 ; buffersizeratio = 10; }

		break;

		case 7: technode =  10; 
		switchmatrixsizeratio_DCIM=0.02; 
		if ((optimize ==1) && (memcelltype == 4)) { buffernumber =1; buffersizeratio= 18;} 
		if ( memcelltype ==4) { 
			repeater_add=-9; // for VGG, -9 // ResNET18 -9 // ResNET34, -14 
			if (file==2) repeater_add=-9;
			tolerance=0;
			repeater_enlarge=1;
		}
		// DCIM (sc, bn, bs) = (0.007, 0, -) so far best , (0.02, 1, 18)
		
		sizingfactor_MUX=  80;
		sizingfactor_MUX_Last= 80;
		switchmatrixsizeratio_ACIM=0.2;

		if ((optimize ==1) && (memcelltype == 1)) 	{ buffernumber = 0 ; buffersizeratio = 10; }

		break; 

		case 8: technode = 7; 
		switchmatrixsizeratio_DCIM=0.04; 
		if ((optimize ==1) && (memcelltype == 4)) { buffernumber =3; buffersizeratio= 20;} 

		// DCIM (sc, bn, bs) = (0.04, 3, 20)

		sizingfactor_MUX=  60;
		sizingfactor_MUX_Last= 60;
		switchmatrixsizeratio_ACIM=0.2;

		// ACIM 0504 - 40,40,0.06

		if ((optimize ==1) && (memcelltype == 1)) 	{ buffernumber = 0 ; buffersizeratio = 10; }

		break;  
		case 9: technode =  5; 
		switchmatrixsizeratio_DCIM=0.015;  
		// DCIM 0.015, 3, 15?

		if ((optimize ==1) && (memcelltype == 4)) { buffernumber =3; buffersizeratio=15;}
		
		sizingfactor_MUX=  50;
		sizingfactor_MUX_Last=  50; 
		switchmatrixsizeratio_ACIM = 0.06;
		
		// ACIM 0504 - 40,40,0.06

		if ((optimize ==1) && (memcelltype == 1)) 	{ buffernumber = 0 ; buffersizeratio = 10; }

		break;  // 0.002, 24 -> 0.001, 24 increases but TOPS degrades
		case 10: technode =  3; 

		 
		switchmatrixsizeratio_DCIM=0.03 ;
		if ((optimize ==1) && (memcelltype == 4)) { buffernumber =7; buffersizeratio=12;} 
		
		// DCIM 0.03, 7, 12
		sizingfactor_MUX=  25;
		sizingfactor_MUX_Last= 25;
		switchmatrixsizeratio_ACIM = 0.03;
		// ACIM 0504 - 40,40,0.03 (best FOM) 25, 25 , 0.03 ensures TOPS/mm^2 improvement from 3->2 nm

		if ((optimize ==1) && (memcelltype == 1))	{ buffernumber =0 ; buffersizeratio = 10; }

		break; 
		case 11: technode =  2; 
		switchmatrixsizeratio_DCIM=0.03 ; 
		if ((optimize ==1) && (memcelltype == 4)) { buffernumber =7; buffersizeratio = 10;} // 0.004, (7, 14)
		// DCIM buffer optimization record
		// 0.001 (7, 19) -> TOPS, 7.74806, TOPS/W, 182.277, TOPS/mm^2, 296.777
		// 0.002 (7, 19) -> TOPS, 7.745, TOPS/W, 185.402, TOPS/mm^2, 299.916
		// 0.003 (7, 19) -> TOPS, 7.74175, TOPS/W, 187.123, TOPS/mm^2, 301.562
		
		// updated DCIM 0.03, 7, 10

		sizingfactor_MUX=  25;
		sizingfactor_MUX_Last= 25;
		switchmatrixsizeratio_ACIM = 0.1;


		// ACIM 0504 - 25,25,0.05, (1,12)
		// ACIM 0506 - 25,25,0.1, (3,10)
		// ACIM 0506 - 35,35,0.05, (3,8)

		// ACIM-SRAM optimization record
		// MUX (100, 100) Buffer (1,6) -> Highest TOPS: sm 0.02, TOPS, 1.18911, TOPS/W, 118.021, TOPS/mm^2, 302.234, FOM, 35670.1
		// Highest TOPS/W * TOPS/mm^2:sm  0.06 TOPS, 1.18314, TOPS/W, 119.512, TOPS/mm^2, 300.717, FOM, 35939.2
		// MUX (100, 100) Buffer (1,x) x>2 sm 0.02 -> Highest TOPS & FOM x= 2, TOPS, 1.18919, TOPS/W, 127.302, TOPS/mm^2, 303.985, FOM, 38697.9,
		// MUX (100, 100) Buffer 0 -> TOPS, 0.999413, TOPS/W, 134.006, TOPS/mm^2, 256.576, FOM, 34382.8
		// MUX (100, 100) Buffer (1,x) sm 0.06 -> Highest TOPS & FOM x= 1, TOPS, 1.18341, TOPS/W, 131.704, TOPS/mm^2, 302.941, FOM, 39898.4
		// MUX (100, 100) Buffer 1 2> TOPS, 1.2069, TOPS/W, 128.599, TOPS/mm^2, 308.071, FOM, 39617.7

		if ((optimize ==1) && (memcelltype == 1)) 	{ buffernumber = 3 ; buffersizeratio = 10; }

		break; 
		case 12: technode =  1; 
		
		switchmatrixsizeratio_DCIM= 0.024; 
		if ((optimize ==1) && (memcelltype == 4)) { buffernumber =7; buffersizeratio=10;} 
		
		// 0.024, 7, 10

		sizingfactor_MUX=  30;
		sizingfactor_MUX_Last= 30; 
		switchmatrixsizeratio_ACIM = 0.1 ;
	
		// ACIM 0504 - 20,20,0.09, (1,5)


		// ACIM-SRAM optimization record
		// (100,100), buffer 0: Highest FOM sm 0.03  TOPS, 1.07628, TOPS/W, 166.274, TOPS/mm^2, 412.397, FOM, 68570.8
		// Highest TOPS sm 0.02 TOPS, 1.07911, TOPS/W, 165.6, TOPS/mm^2, 413.483, FOM, 68472.9
		// sm 0.03 Mux (30,30), Buf (1,1) has the best FOM TOPS, 1.15149, TOPS/W, 161.182, TOPS/mm^2, 555.026, FOM, 89460.4
		
		if ((optimize ==1) && (memcelltype == 1)) 	{ buffernumber = 3 ; buffersizeratio = 8; } // (100,100), (1,6)



		break;  // around 3 ~ 12
		// case 12:technode =  1; break; 
	} 


	// 1,4 update-2: barrier thickness
	// 1.4 update-3: barrier thickness thickness updated

	switch (technode){
	// needs update
		case 130: 	Metal0 = 175; Metal1 = 175; wireWidth = 175; barrierthickness = 10.0e-9 ; featuresize = wireWidth*1e-9; break;  
		case 90: 	Metal0 = 110; Metal1 = 110; wireWidth=110; barrierthickness = 10.0e-9 ; featuresize = wireWidth*1e-9; break;  
		case 65:	Metal0 = 105; Metal1 = 105; wireWidth=105; barrierthickness = 7.0e-9 ; featuresize = wireWidth*1e-9; break;  
		case 45:	Metal0 = 80; Metal1 = 80; wireWidth=80; barrierthickness = 5.0e-9 ; featuresize = wireWidth*1e-9; break;  
		case 32:	Metal0 = 56; Metal1 = 56; wireWidth=56; barrierthickness = 4.0e-9 ; featuresize = wireWidth*1e-9; break;  
		case 22:	Metal0 = 40; Metal1 = 40; wireWidth=40; barrierthickness = 3.0e-9 ; featuresize = wireWidth*1e-9; break; 
		case 14:	Metal0 = 32; Metal1 = 39; wireWidth=32; barrierthickness = 2.5e-9 ; featuresize = wireWidth*1e-9; break;  
		case 10:	Metal0 = 22; Metal1 = 32; wireWidth=22; barrierthickness = 2.5e-9 ; featuresize = wireWidth*1e-9; break;  
		case 7:		Metal0 = 20; Metal1 = 28.5; wireWidth=20; barrierthickness = 2.5e-9 ; featuresize = wireWidth*1e-9; break;  
		case 5:		Metal0 = 15; Metal1 = 17; wireWidth=18; barrierthickness = 2.0e-9 ; featuresize = wireWidth*1e-9; break;  
		case 3:		Metal0 = 12; Metal1 = 16; wireWidth=16; barrierthickness = 1.5e-9 ; featuresize = wireWidth*1e-9; break; 
		case 2:		Metal0 = 10; Metal1 = 11.5; wireWidth=12; barrierthickness = 0.5e-9 ; featuresize = wireWidth*1e-9; break;  
		case 1:		Metal0 = 8; Metal1 = 10; wireWidth=8; barrierthickness = 0.5e-9 ; featuresize = wireWidth*1e-9; break;  
		case -1:	break;	
		default:	exit(-1); puts("Wire width out of range"); 
	}
	


	numRowSubArray = 256; // 128;               // # of rows in single subArray
	numColSubArray = 256; //144;               // # of columns in single subArray

	if (memcelltype ==4)
	{
	numRowSubArray = 256;               // # of rows in single subArray
	numColSubArray =256;               // # of columns in single subArray
	
	}	

	/*** option to relax subArray layout ***/
	relaxArrayCellHeight = 0;           // relax ArrayCellHeight or not
	relaxArrayCellWidth = 0;            // relax ArrayCellWidth or not
	


	levelOutput = 32;                   // # of levels of the multilevelSenseAmp output, should be in 2^N forms; e.g. 32 levels --> 5-bit ADC
	cellBit = 1;                        // precision of memory device 
	
	/*** parameters for SRAM ***/
	// due the scaling, suggested SRAM cell size above 22nm: 160F^2
	// SRAM cell size at 14nm: 300F^2
	// SRAM cell size at 10nm: 400F^2
	// SRAM cell size at 7nm: 600F^2
	



	if (memcelltype == 4)  numColMuxed = parallel_weightprecision; 
	// 1.4 update: handle the exception for conventionalsequential case

	if ((conventionalSequential == 1) && (memcelltype = 1))
	{
	numColMuxed=numColPerSynapse;
	}
	
	else numColMuxed = 8;                    // How many columns share 1 ADC (for eNVM and FeFET) or parallel SRAM
	
	if (technode>14){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 10;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 28;        // SRAM Cell width in feature size  
	}
	else if (technode==14){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 10.6;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 30.8;        // SRAM Cell width in feature size  
	}
	else if (technode==10){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 12.8;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 31.25;        // SRAM Cell width in feature size  
	}
	else if (technode==7){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 16;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 34.43;        // SRAM Cell width in feature size  
	}
	else if (technode==5){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 19.2;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 43.75;        // SRAM Cell width in feature size  
	}
	else if (technode==3){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 30;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 68.26;        // SRAM Cell width in feature size  
	}
	else if (technode==2){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 42;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 120;// 111.42;        // SRAM Cell width in feature size  
	}
	else if (technode==1){
	widthSRAMCellNMOS = 1;                            
	widthSRAMCellPMOS = 1;
	widthAccessCMOS = 1;
	heightInFeatureSizeSRAM = 80;        // SRAM Cell height in feature size  
	widthInFeatureSizeSRAM = 144;        // SRAM Cell width in feature size  
	}

	minSenseVoltage = 0.1;
	
	/*** parameters for analog synaptic devices ***/
	heightInFeatureSize1T1R = 4;        // 1T1R Cell height in feature size
	widthInFeatureSize1T1R = 12;         // 1T1R Cell width in feature size
	heightInFeatureSizeCrossbar = 2;    // Crossbar Cell height in feature size
	widthInFeatureSizeCrossbar = 2;     // Crossbar Cell width in feature size
	
	resistanceOn = 6e3;               // Ron resistance at Vr in the reported measurement data (need to recalculate below if considering the nonlinearity)
	resistanceOff = 6e3*150;           // Roff resistance at Vr in the reported measurement dat (need to recalculate below if considering the nonlinearity)
	maxConductance = (double) 1/resistanceOn;
	minConductance = (double) 1/resistanceOff;
	
	readVoltage = 0.5;	                // On-chip read voltage for memory cell
	readPulseWidth = 10e-9;             // read pulse width in sec
	accessVoltage = 1.1;                // Gate voltage for the transistor in 1T1R
	resistanceAccess = resistanceOn*IR_DROP_TOLERANCE;            // resistance of access CMOS in 1T1R
	writeVoltage = 2;					// Enable level shifer if writeVoltage > 1.5V
	
	/*** Calibration parameters ***/

	if(validated){
		alpha = 1.44;	// wiring area of level shifter
		beta = 1.4;  	// latency factor of sensing cycle
		gamma = 0.5; 	// switching activity of DFF in shifter-add and accumulator
		delta = 0.15; 	// switching activity of adder 
		epsilon = 0.05; // switching activity of control circuits
		zeta = 1.22; 	// post-layout energy increase
	}		
	
	/***************************************** user defined design options and parameters *****************************************/
	
	
	
	/***************************************** Initialization of parameters NO need to modify *****************************************/
	
	if (memcelltype == 1 || memcelltype == 4 ) {
		cellBit = 1;             // force cellBit = 1 for all SRAM cases
	} 
	
	/*** initialize operationMode as default ***/
	conventionalParallel = 0;
	conventionalSequential = 0;
	BNNparallelMode = 0;                
	BNNsequentialMode = 0;              
	XNORsequentialMode = 0;          
	XNORparallelMode = 0;         
	switch(operationmode) {
		case 6:	    XNORparallelMode = 1;               break;     
		case 5:	    XNORsequentialMode = 1;             break;     
		case 4:	    BNNparallelMode = 1;                break;     
		case 3:	    BNNsequentialMode = 1;              break;     
		case 2:	    conventionalParallel = 1;           break;     
		case 1:	    conventionalSequential = 1;         break;     
		default:	printf("operationmode ERROR\n");	exit(-1);
	}
	
	/*** parallel read ***/
	parallelRead = 0;
	if(conventionalParallel || BNNparallelMode || XNORparallelMode) {
		parallelRead = 1;
	} else {
		parallelRead = 0;
	}

	// 1,4 update-2: barrier thickness

	// wirewidth
	if (wireWidth >= 175) {
		AR = 1.6; 
		Rho = 2.01*1e-8;
	} else if ((110 <= wireWidth) &&  (wireWidth < 175)) {
		AR = 1.6; 
		Rho = 2.20*1e-8;
	} else if ((105 <= wireWidth) &&  (wireWidth < 110)) {
		AR = 1.7; 
		Rho = 2.21*1e-8;
	} else if ((80 <= wireWidth) &&  (wireWidth < 105)){
		AR = 1.7; 
		Rho = 2.37*1e-8;
	} else if ((56 <= wireWidth) &&  (wireWidth < 80)){
		AR = 1.8; 
		Rho = 2.63*1e-8;
	} else if ((40 <= wireWidth) &&  (wireWidth < 56)) {
		AR = 1.9; 
		Rho = 2.97*1e-8;
	} else if ((32 <= wireWidth) &&  (wireWidth < 40)) {
		AR = 2.0; 
		Rho = 3.25*1e-8;
	} else if ((22 <= wireWidth) &&  (wireWidth < 32)){
		AR = 2.00; Rho = 3.95*1e-8;
	} else if ((20 <= wireWidth) &&  (wireWidth < 22)){
		AR = 2.00; Rho = 4.17*1e-8; 
	} else if ((15 <= wireWidth) &&  (wireWidth < 20)){
		AR = 2.00; Rho = 4.98*1e-8; 
	} else if ((12 <= wireWidth) &&  (wireWidth < 15)){
		AR = 2.00; Rho = 5.8*1e-8; 
	} else if ((10 <= wireWidth) &&  (wireWidth < 12)){
		// AR = 3.00; Rho = 6.65*1e-8; 
		AR = 2.00; Rho = 6.61*1e-8; 
	} else if ((8 <= wireWidth) &&  (wireWidth < 10)){
		AR = 3.00; Rho = 7.87*1e-8; 
	} else {
		exit(-1); puts("Wire width out of range"); 
	}

	Rho = Rho * 1 / (1- ( (2*AR*wireWidth + wireWidth)*barrierthickness / (AR*pow(wireWidth,2) ) ));
	

	// Metal0
	if (Metal0 >= 175) {
		AR_Metal0 = 1.6; 
		Rho_Metal0 = 2.01*1e-8;
	} else if ((110 <= Metal0) &&  (Metal0< 175)) {
		AR_Metal0 = 1.6; 
		Rho_Metal0 = 2.20*1e-8;
	} else if ((105 <= Metal0) &&  (Metal0< 110)){
		AR_Metal0 = 1.7; 
		Rho_Metal0 = 2.21*1e-8;
	} else if ((80 <= Metal0) &&  (Metal0< 105)) {
		AR_Metal0 = 1.7; 
		Rho_Metal0 = 2.37*1e-8;
	} else if ((56 <= Metal0) &&  (Metal0< 80)){
		AR_Metal0 = 1.8; 
		Rho_Metal0 = 2.63*1e-8;
	} else if ((40 <= Metal0) &&  (Metal0< 56)) {
		AR_Metal0 = 1.9; 
		Rho_Metal0 = 2.97*1e-8;
	} else if ((32 <= Metal0) &&  (Metal0< 40)) {
		AR_Metal0 = 2.0; 
		Rho_Metal0 = 3.25*1e-8;
	} else if ((22 <= Metal0) &&  (Metal0< 32)){
		AR_Metal0 = 2.00; Rho_Metal0 = 3.95*1e-8;
	} else if ((20 <= Metal0) &&  (Metal0< 22)){
		AR_Metal0 = 2.00; Rho_Metal0 = 4.17*1e-8; 
	} else if ((15 <= Metal0) &&  (Metal0< 20)){
		AR_Metal0 = 2.00; Rho_Metal0 = 4.98*1e-8; 
	} else if ((12 <= Metal0) &&  (Metal0< 15)){
		AR_Metal0 = 2.00; Rho_Metal0 = 5.8*1e-8; 
	} else if ((10 <= Metal0) &&  (Metal0< 12)){
		// AR_Metal0 = 3.00; Rho_Metal0 = 6.65*1e-8; 
		AR_Metal0 = 2.00; Rho_Metal0 = 6.61*1e-8; 
	} else if ((8 <= Metal0) &&  (Metal0< 10)){
		AR_Metal0 = 3.00; Rho_Metal0 = 7.87*1e-8; 
	} else {
		exit(-1); puts("Wire width out of range"); 
	}

	Rho_Metal0 = Rho_Metal0 * 1 / (1- ( (2*AR_Metal0*Metal0 + Metal0)*barrierthickness / (AR_Metal0*pow(Metal0,2) ) ));

	// Metal1
	if (Metal1 >= 175) {
		AR_Metal1 = 1.6; 
		Rho_Metal1 = 2.01*1e-8;
	} else if ((110 <= Metal1) &&  (Metal1 < 175)) {
		AR_Metal1 = 1.6; 
		Rho_Metal1 = 2.20*1e-8;
	} else if ((105 <= Metal1) &&  (Metal1 < 110)) {
		AR_Metal1 = 1.7; 
		Rho_Metal1 = 2.21*1e-8;
	} else if ((80 <= Metal1) &&  (Metal1 <105)) {
		AR_Metal1 = 1.7; 
		Rho_Metal1 = 2.37*1e-8;
	} else if ((56 <= Metal1) &&  (Metal1 < 80)) {
		AR_Metal1 = 1.8; 
		Rho_Metal1 = 2.63*1e-8;
	} else if ((40 <= Metal1) &&  (Metal1 < 56)){
		AR_Metal1 = 1.9; 
		Rho_Metal1 = 2.97*1e-8;
	} else if ((32 <= Metal1) &&  (Metal1 < 40)) {
		AR_Metal1 = 2.0; 
		Rho_Metal1 = 3.25*1e-8;
	} else if ((22 <= Metal1) &&  (Metal1 < 32)){
		AR_Metal1 = 2.00; Rho_Metal1 = 3.95*1e-8;
	} else if ((20 <= Metal1) &&  (Metal1 < 22)){
		AR_Metal1 = 2.00; Rho_Metal1 = 4.17*1e-8; 
	} else if ((15 <= Metal1) &&  (Metal1 < 20)){
		AR_Metal1 = 2.00; Rho_Metal1 = 4.98*1e-8; 
	} else if ((12 <= Metal1) &&  (Metal1 < 15)){
		AR_Metal1 = 2.00; Rho_Metal1 = 5.8*1e-8; 
	} else if ((10 <= Metal1) &&  (Metal1 < 12)){
		// AR_Metal1 = 3.00; Rho_Metal1 = 6.65*1e-8; 
		AR_Metal1 = 2.00; Rho_Metal1 = 6.61*1e-8;
	} else if ((8 <= Metal1) &&  (Metal1 < 10)){
		AR_Metal1 = 3.00; Rho_Metal1 = 7.87*1e-8; 
	} else {
		exit(-1); puts("Wire width out of range"); 
	}

	Rho_Metal1 = Rho_Metal1 * 1 / (1- ( (2*AR_Metal1*Metal1 + Metal1)*barrierthickness / (AR_Metal1*pow(Metal1,2) ) ));


	Metal0_unitwireresis =  Rho_Metal0 / ( Metal0*1e-9 * Metal0*1e-9 * AR_Metal0 );
	Metal1_unitwireresis =  Rho_Metal1 / ( Metal1*1e-9 * Metal1*1e-9 * AR_Metal1 );
	
	

	if (memcelltype == 1 || memcelltype == 4) {
		wireLengthRow = wireWidth * 1e-9 * heightInFeatureSizeSRAM;
		wireLengthCol = wireWidth * 1e-9 * widthInFeatureSizeSRAM;
		// need to handle DCIM Case if wirelengthrow/col is used
	} else {
		if (accesstype == 1) {
			wireLengthRow = wireWidth * 1e-9 * heightInFeatureSize1T1R;
			wireLengthCol = wireWidth * 1e-9 * widthInFeatureSize1T1R;
		} else {
			wireLengthRow = wireWidth * 1e-9 * heightInFeatureSizeCrossbar;
			wireLengthCol = wireWidth * 1e-9 * widthInFeatureSizeCrossbar;
		}
	}

	Rho *= (1+0.00451*abs(temp-300));

	if (wireWidth == -1) {
		unitLengthWireResistance = 1.0;	// Use a small number to prevent numerical error for NeuroSim
		wireResistanceRow = 0;
		wireResistanceCol = 0;
	} else {
		unitLengthWireResistance =  Rho / ( wireWidth*1e-9 * wireWidth*1e-9 * AR );

		wireResistanceRow = unitLengthWireResistance * wireLengthRow;
		wireResistanceCol = unitLengthWireResistance * wireLengthCol;
	}
	/***************************************** Initialization of parameters NO need to modify *****************************************/
}

