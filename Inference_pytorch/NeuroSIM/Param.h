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

#ifndef PARAM_H_
#define PARAM_H_

#include <string>

#include <vector>


using namespace std;

class Param {
public:
	Param();

	int operationmode, operationmodeBack, memcelltype, accesstype, transistortype, deviceroadmap;      		
	
	double heightInFeatureSizeSRAM, widthInFeatureSizeSRAM, widthSRAMCellNMOS, widthSRAMCellPMOS, widthAccessCMOS, minSenseVoltage;
	 
	double heightInFeatureSize1T1R, widthInFeatureSize1T1R, heightInFeatureSizeCrossbar, widthInFeatureSizeCrossbar;
	
	int relaxArrayCellHeight, relaxArrayCellWidth;

	
	// additional parameters to track subarray cell height & width design
	double finalCellHeight=0, finalCellWidth=0;
	double minimumCellHeight=0, minimumCellWidth=0;
	// Anni update
	bool globalBusType, globalBufferType, tileBufferType, peBufferType, chipActivation, reLu, novelMapping, pipeline, SARADC, currentMode, validated, synchronous;
	int globalBufferCoreSizeRow, globalBufferCoreSizeCol, tileBufferCoreSizeRow, tileBufferCoreSizeCol;																								
	
	double clkFreq, featuresize, readNoise, resistanceOn, resistanceOff, maxConductance, minConductance;
	int temp, technode, wireWidth, multipleCells;
	double maxNumLevelLTP, maxNumLevelLTD, readVoltage, readPulseWidth, writeVoltage;
	double accessVoltage, resistanceAccess;
	double nonlinearIV, nonlinearity;
	double writePulseWidth, numWritePulse;
	double globalBusDelayTolerance, localBusDelayTolerance;
	double treeFoldedRatio, maxGlobalBusWidth;
	double algoWeightMax, algoWeightMin;
	
	int neuro, multifunctional, parallelWrite, parallelRead;
	int numlut, numColMuxed, numWriteColMuxed, levelOutput, avgWeightBit, numBitInput;
	int numRowSubArray, numColSubArray;
	int cellBit, synapseBit;
	int speedUpDegree;
	int draincaptest;
	double adderinputcap;
	double adderoutputcap;

	double parallel_weightprecision;
	int XNORparallelMode, XNORsequentialMode, BNNparallelMode, BNNsequentialMode, conventionalParallel, conventionalSequential, DigitalCIM; 
	double numRowPerSynapse, numColPerSynapse;
	double AR, Rho, wireLengthRow, wireLengthCol, unitLengthWireResistance, wireResistanceRow, wireResistanceCol;
	
	double alpha, beta, gamma, delta, epsilon, zeta;
	double acutallayout=0;
	double junctioncap_modify=0;

	double Metal0=0;
	double Metal1=0;
	double AR_Metal0=0;
	double AR_Metal1=0;
	double Rho_Metal0=0;
	double Rho_Metal1=0;
	double Metal0_unitwireresis=0;
	double Metal1_unitwireresis=0;
	
	// repeater
	double repeater_add =0; 
	double tolerance =1; 
	double repeater_enlarge=0;
	// area benchmarking
	
	double subarray_height;
	double subarray_width;
	double subarray_usedarea;
	double addertree_width;
	double wlSwitchMatrix_width ;
	double wlDecoder_width  ;
	double shiftAddInput_width ;
	double sramWriteDriver_height ;
	double precharger_height  ;
	double senseAmp_height ;
	double dff_width  ;
	double Inputdff_width;
	double addertree_delay =0  ;
	double precharge_delay=0;
	double ADC_delay=0;
	double multisense_delay=0;
	double WL_delay=0;
	double multiencoder_delay=0;
	double mux_delay=0;
	double col_delay;
	double mux_res=0;
	double NOR_delay=0;
	double capcell_SRAM=0;
	double capcell_row_SRAM=0;
	double rescell_SRAM=0;
	double row_delay=0;

	double resCellAccess = 0;

	// DCIM part // 
	// DCIM_buffersize 

	double buffernumber=0;
	double buffersizeratio=0; 
	double addedbufferlatency=0;
	double switchmatrixsizeratio_DCIM=0; 

	// DCIM calibration

	double DCIM_energy_recalculated=0;

	int toggleindex=0;
	int toggle_enforce=0;
	double togglerows [6]= {16.0, 32.0, 64.0, 128.0, 192.0, 256.0};
	
	int realtime_toggle =0;
	double repeaterfinnum = 0; 
	double dff_cap;
	double targetdriveres=0;
	double sectionresistance=0;
	double finfetonresistance = 0;
	double finfetswitchmatrixdrive = 0;

	double columncap=0;
	double capNORInput = 0;
	double capNOROutput = 0;
	double DCIMline_R1 = 0;
	double DCIMline_C1 = 0;
	double DCIMline_R2 = 0;
	double DCIMline_C2 = 0;
	double resRow_DCIM = 0;
	double resCol_DCIM = 0;

	double zeroweightcount =0;
	double weightcount =0;

	double sparsitycalc=0;


	// self leakage
	double subarray_selfleakage =0;
	double PE_selfleakage =0;
	double Tile_selfleakage =0; // can later be extended to have values for each lyaer
	double repeater_leakage=0;

	// Activationtype
	double speciallayout=0;
	bool Activationtype;
	bool Currentsenseamp_fix;

	// latency decompose

	double onlymainarray = 0;
	double Switching_ADC =0;
	double DC_ADC_min=0;
	double DC_ADC_max =0;
			

	// energy breakdown extraction
	double totalsubarrayoperations=0;
	double Macrototaldynamic=0;
	double Macrototallatency=0;
	double Macroleakage=0;
	double subarray_leakage;

	double macro_readDynamicEnergyShiftAdd ;
	double macro_readDynamicEnergyAccum;
	double macro_readDynamicEnergyAdder;
	double macro_readDynamicEnergyBuffer;
	double macro_readDynamicSwitchmatrix;
	double macro_readDynamicInterconnect;
	double macro_readDynamicDFFinShiftAdd;
	double macro_readDynamicADC;

	double SRAM_peri_leakage;
	double DCIM_array_leakage;
	double ShiftAdd_leakage;
	double Adder_leakage;
	double Buffer_leakage;
	double Switchmatrix_leakage;

	double PE_bus=0;
	double PE_Addertree=0;
	double PE_buffer=0;
	double PE_total=0;

	double PE_bus_latency=0;
	double PE_Addertree_latency=0;
	double PE_buffer_latency=0;
	double PE_total_latency=0;

	double Tile_htree=0;
	double Tile_addertree=0;
	double Tile_sigmoid=0;
	double Tile_ReLU=0;
	double Tile_buffer=0;
	double Tile_total=0;

	double Tile_htree_latency=0;
	double Tile_addertree_latency=0;
	double Tile_sigmoid_latency=0;
	double Tile_ReLU_latency=0;
	double Tile_buffer_latency=0;
	double Tile_total_latency=0;

	double Chip_buffer=0;
	double Chip_htree=0;
	double Chip_addertree=0;
	double Chip_sigmoid=0;
	double Chip_ReLU=0;
	double Chip_maxpool=0;
	double Chip_total=0;

	double Chip_buffer_latency=0;
	double Chip_htree_latency=0;
	double Chip_addertree_latency=0;
	double Chip_sigmoid_latency=0;
	double Chip_maxpool_latency=0;

	// leakage debug 
	double busleakage;
	double PE_leakage ;
	double Tile_Leakage;

	// area debug
	double areaothers_subarray=0;
	double areaothers_subarray_wlSwitchMatrix =0;
	double areaothers_subarray_slSwitchMatrix =0;
	double areaothers_subarray_mux =0;
	double areaothers_subarray_muxDecoder =0;
	double areaothers_subarray_levelshifter =0;
	double areaothers_subarray_ADC =0;

	double area_LS_biggest =0;
	double area_levelw=0;
	double area_levelh =0;

	double CPP_const=0;
	double numFoldedPMOS=0;
	double numFoldedNMOS=0;
    double NumPFin =0;
	double ADCwidth =0;
	double ADCheight =0;
	double currentSenseAmp_area =0;
	double areaothers_subarray_multilevelSAEncoder=0;
	double areaothers_subarray_shiftAdd=0;
	double PEadder_area=0;

	// 1.4 update: new ADC model
	double newADC;
	double newSAEncoder;
	double	newADCarea;
	double mirrorcap;
	double reference_energy_peri;
	double reference_mux;
	double reference_switchmatrix;

	double globalbuswidth;
	double buswidthforce;
	// debug

	double caprow_debug;
	double numrow_debug;
	double lengthRow;

	// 1.4 update: Final driver sizing for row decoder conventional parallel mode (SRAM, RRAM)
	// multiplied by the driver width
	double sizingfactor_MUX= 1; 
	double sizingfactor_MUX_Last= 1; 
	double sizingfactor_WLdecoder= 1;
	double switchmatrixsizeratio_ACIM = 1; // factor multiplied to target resistance (smaller the value, the better the drivibility)
	double newswitchmatrixsizeratio_ACIM = 1; // factor multiplied to width (larger the value, the better the drivibility)

	// 1.4 update-2: added parameters for buffer insertion
	double unitcap;
	double unitres;
	double drivecapin; 

	// DCIM
	string filename;
	// Array
	double dumcolshared;
	double arraywidthunit;
	double arrayheight;

	// TCAS

	double access_fincount;
	double ADC_area;
	double array_area;
	double levelshifter_area;
	// Areabreakdown

	double peBufferarea;
	double tileBufferarea;
	double globalBufferarea;
	double peBufferarea_total;
	double tileBufferarea_total;
	double globalBufferarea_total;
	
	// 1.4 update-3: barrier thickness
	double barrierthickness= 0; 

	// 1.4 update: new bus 
	double newbus;
	double inputtoggle;
	double outputtoggle;

	// 1.4 update: togglecount for interconnect

	double numcount=0;
	double unitactivity=0;
	double averageTR=0;
};

#endif