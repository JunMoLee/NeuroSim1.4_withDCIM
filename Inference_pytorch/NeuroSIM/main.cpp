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
#include "constant.h"
#include "formula.h"
#include "Param.h"
#include "Tile.h"
#include "Chip.h"
#include "ProcessingUnit.h"
#include "SubArray.h"
#include "Definition.h"

using namespace std;

vector<vector<double> > getNetStructure(const string &inputfile);

int main(int argc, char * argv[]) {   

    auto start = chrono::high_resolution_clock::now();
    
    gen.seed(0);
    
    vector<vector<double> > netStructure;
    netStructure = getNetStructure(argv[1]);
    
    // define weight/input/memory precision from wrapper
    param->synapseBit = atoi(argv[2]);              // precision of synapse weight
    param->numBitInput = atoi(argv[3]);             // precision of input neural activation

    if (param->cellBit > param->synapseBit) {
        cout << "ERROR!: Memory precision is even higher than synapse precision, please modify 'cellBit' in Param.cpp!" << endl;
        param->cellBit = param->synapseBit;
    }
    
    /*** initialize operationMode as default ***/
    param->DigitalCIM =0;
    param->conventionalParallel = 0;
    param->conventionalSequential = 0;
    param->BNNparallelMode = 0;                // parallel BNN
    param->BNNsequentialMode = 0;              // sequential BNN
    param->XNORsequentialMode = 0;           // Use several multi-bit RRAM as one synapse
    param->XNORparallelMode = 0;         // Use several multi-bit RRAM as one synapse
    switch(param->operationmode) {
        case 7:     param->DigitalCIM = 1;               break; 
        case 6:     param->XNORparallelMode = 1;               break;     
        case 5:     param->XNORsequentialMode = 1;             break;     
        case 4:     param->BNNparallelMode = 1;                break;     
        case 3:     param->BNNsequentialMode = 1;              break;    
        case 2:     param->conventionalParallel = 1;           break;     
        case 1:     param->conventionalSequential = 1;         break;    
        case -1:    break;
        default:    exit(-1);
    }
    
    if (param->XNORparallelMode || param->XNORsequentialMode) {
        param->numRowPerSynapse = 2;
    } else {
        param->numRowPerSynapse = 1;
    }
    if (param->BNNparallelMode) {
        param->numColPerSynapse = 2;
    } else if (param->XNORparallelMode || param->XNORsequentialMode || param->BNNsequentialMode) {
        param->numColPerSynapse = 1;
    } else {
        param->numColPerSynapse = ceil((double)param->synapseBit/(double)param->cellBit); 
    }
    
    double maxPESizeNM, maxTileSizeCM, numPENM;
    vector<int> markNM;
    vector<int> pipelineSpeedUp;
    markNM = ChipDesignInitialize(inputParameter, tech, cell, false, netStructure, &maxPESizeNM, &maxTileSizeCM, &numPENM);
    pipelineSpeedUp = ChipDesignInitialize(inputParameter, tech, cell, true, netStructure, &maxPESizeNM, &maxTileSizeCM, &numPENM);
    
    double desiredNumTileNM, desiredPESizeNM, desiredNumTileCM, desiredTileSizeCM, desiredPESizeCM;
    int numTileRow, numTileCol;
    
    vector<vector<double> > numTileEachLayer;
    vector<vector<double> > utilizationEachLayer;
    vector<vector<double> > speedUpEachLayer;
    vector<vector<double> > tileLocaEachLayer;
    
    numTileEachLayer = ChipFloorPlan(true, false, false, netStructure, markNM, 
                    maxPESizeNM, maxTileSizeCM, numPENM, pipelineSpeedUp,
                    &desiredNumTileNM, &desiredPESizeNM, &desiredNumTileCM, &desiredTileSizeCM, &desiredPESizeCM, &numTileRow, &numTileCol);    
    
    utilizationEachLayer = ChipFloorPlan(false, true, false, netStructure, markNM, 
                    maxPESizeNM, maxTileSizeCM, numPENM, pipelineSpeedUp,
                    &desiredNumTileNM, &desiredPESizeNM, &desiredNumTileCM, &desiredTileSizeCM, &desiredPESizeCM, &numTileRow, &numTileCol);
    
    speedUpEachLayer = ChipFloorPlan(false, false, true, netStructure, markNM,
                    maxPESizeNM, maxTileSizeCM, numPENM, pipelineSpeedUp,
                    &desiredNumTileNM, &desiredPESizeNM, &desiredNumTileCM, &desiredTileSizeCM, &desiredPESizeCM, &numTileRow, &numTileCol);
                    
    tileLocaEachLayer = ChipFloorPlan(false, false, false, netStructure, markNM,
                    maxPESizeNM, maxTileSizeCM, numPENM, pipelineSpeedUp,
                    &desiredNumTileNM, &desiredPESizeNM, &desiredNumTileCM, &desiredTileSizeCM, &desiredPESizeCM, &numTileRow, &numTileCol);
    

    cout << "------------------------------ FloorPlan --------------------------------" <<  endl;
    cout << endl;
    cout << "Tile and PE size are optimized to maximize memory utilization ( = memory mapped by synapse / total memory on chip)" << endl;
    cout << endl;
    if (!param->novelMapping) {
        cout << "Desired Conventional Mapped Tile Storage Size: " << desiredTileSizeCM << "x" << desiredTileSizeCM << endl;
        cout << "Desired Conventional PE Storage Size: " << desiredPESizeCM << "x" << desiredPESizeCM << endl;
    } else {
        cout << "Desired Conventional Mapped Tile Storage Size: " << desiredTileSizeCM << "x" << desiredTileSizeCM << endl;
        cout << "Desired Conventional PE Storage Size: " << desiredPESizeCM << "x" << desiredPESizeCM << endl;
        cout << "Desired Novel Mapped Tile Storage Size: " << numPENM << "x" << desiredPESizeNM << "x" << desiredPESizeNM << endl;
    }
    cout << "User-defined SubArray Size: " << param->numRowSubArray << "x" << param->numColSubArray << endl;
    cout << endl;
    cout << "----------------- # of tile used for each layer -----------------" <<  endl;
    double totalNumTile = 0;
    for (int i=0; i<netStructure.size(); i++) {
        cout << "layer" << i+1 << ": " << numTileEachLayer[0][i] * numTileEachLayer[1][i] << endl;
        totalNumTile += numTileEachLayer[0][i] * numTileEachLayer[1][i];
    }
    cout << endl;

    cout << "----------------- Speed-up of each layer ------------------" <<  endl;
    for (int i=0; i<netStructure.size(); i++) {
        cout << "layer" << i+1 << ": " << speedUpEachLayer[0][i] * speedUpEachLayer[1][i] << endl;
    }
    cout << endl;
    
    cout << "----------------- Utilization of each layer ------------------" <<  endl;
    double realMappedMemory = 0;
    for (int i=0; i<netStructure.size(); i++) {
        cout << "layer" << i+1 << ": " << utilizationEachLayer[i][0] << endl;
        realMappedMemory += numTileEachLayer[0][i] * numTileEachLayer[1][i] * utilizationEachLayer[i][0];
    }
    cout << "Memory Utilization of Whole Chip: " << realMappedMemory/totalNumTile*100 << " % " << endl;
    cout << endl;
    cout << "---------------------------- FloorPlan Done ------------------------------" <<  endl;
    cout << endl;
    cout << endl;
    cout << endl;
        double numComputation = 0;
    for (int i=0; i<netStructure.size(); i++) {
        numComputation += 2*(netStructure[i][0] * netStructure[i][1] * netStructure[i][2] * netStructure[i][3] * netStructure[i][4] * netStructure[i][5]);
    }

    
    ChipInitialize(inputParameter, tech, cell, netStructure, markNM, numTileEachLayer,
                    numPENM, desiredNumTileNM, desiredPESizeNM, desiredNumTileCM, desiredTileSizeCM, desiredPESizeCM, numTileRow, numTileCol);
            
    double chipHeight, chipWidth, chipArea, chipAreaIC, chipAreaADC, chipAreaAccum, chipAreaOther, chipAreaArray;
    double CMTileheight = 0;
    double CMTilewidth = 0;
    double NMTileheight = 0;
    double NMTilewidth = 0;
    vector<double> chipAreaResults;
    
    chipAreaResults = ChipCalculateArea(inputParameter, tech, cell, desiredNumTileNM, numPENM, desiredPESizeNM, desiredNumTileCM, desiredTileSizeCM, desiredPESizeCM, numTileRow, 
                    &chipHeight, &chipWidth, &CMTileheight, &CMTilewidth, &NMTileheight, &NMTilewidth);     
    
    chipArea = chipAreaResults[0];
    chipAreaIC = chipAreaResults[1];
    chipAreaADC = chipAreaResults[2];
    chipAreaAccum = chipAreaResults[3];
    chipAreaOther = chipAreaResults[4];
    chipAreaArray = chipAreaResults[5];

    double clkPeriod = 0;
    double layerclkPeriod = 0;
    
    double chipReadLatency = 0;
    double chipReadDynamicEnergy = 0;
    double chipLeakageEnergy = 0;
    double chipLeakage = 0;
    double chipbufferLatency = 0;
    double chipbufferReadDynamicEnergy = 0;
    double chipicLatency = 0;
    double chipicReadDynamicEnergy = 0;
    
    double chipLatencyADC = 0;
    double chipLatencyAccum = 0;
    double chipLatencyOther = 0;
    double chipEnergyADC = 0;
    double chipEnergyAccum = 0;
    double chipEnergyOther = 0;
    
    double layerReadLatency = 0;
    double layerReadDynamicEnergy = 0;
    double tileLeakage = 0;
    // Anni update: leakage of SRAM when partial subarray in use
    double tileLeakageSRAMInUse = 0;
    double layerbufferLatency = 0;
    double layerbufferDynamicEnergy = 0;
    double layericLatency = 0;
    double layericDynamicEnergy = 0;
    
    double coreLatencyADC = 0;
    double coreLatencyAccum = 0;
    double coreLatencyOther = 0;
    double coreEnergyADC = 0;
    double coreEnergyAccum = 0;
    double coreEnergyOther = 0;
    
    if (param->synchronous){
        // calculate clkFreq
        for (int i=0; i<netStructure.size(); i++) {     
            // Anni update: add &tileLeakageSRAMInUse
            ChipCalculatePerformance(inputParameter, tech, cell, i, argv[2*i+4], argv[2*i+4], argv[2*i+5], netStructure[i][6],
                        netStructure, markNM, numTileEachLayer, utilizationEachLayer, speedUpEachLayer, tileLocaEachLayer,
                        numPENM, desiredPESizeNM, desiredTileSizeCM, desiredPESizeCM, CMTileheight, CMTilewidth, NMTileheight, NMTilewidth,
                        &layerReadLatency, &layerReadDynamicEnergy, &tileLeakage, &tileLeakageSRAMInUse, &layerbufferLatency, &layerbufferDynamicEnergy, &layericLatency, &layericDynamicEnergy,
                        &coreLatencyADC, &coreLatencyAccum, &coreLatencyOther, &coreEnergyADC, &coreEnergyAccum, &coreEnergyOther, true, &layerclkPeriod);
            if(clkPeriod < layerclkPeriod){
                clkPeriod = layerclkPeriod;
            }           
        }       
        cout<<"clkPeriod: "<<clkPeriod<<endl;
        if(param->clkFreq > 1/clkPeriod){
            param->clkFreq = 1/clkPeriod;
        }
    }

    cout << "-------------------------------------- Hardware Performance --------------------------------------" <<  endl;  
    if (! param->pipeline) {
        // layer-by-layer process
        // show the detailed hardware performance for each layer
        for (int i=0; i<netStructure.size(); i++) {
            cout << "-------------------- Estimation of Layer " << i+1 << " ----------------------" << endl;

            // Anni update: add &tileLeakageSRAMInUse
            ChipCalculatePerformance(inputParameter, tech, cell, i, argv[2*i+4], argv[2*i+4], argv[2*i+5], netStructure[i][6],
                        netStructure, markNM, numTileEachLayer, utilizationEachLayer, speedUpEachLayer, tileLocaEachLayer,
                        numPENM, desiredPESizeNM, desiredTileSizeCM, desiredPESizeCM, CMTileheight, CMTilewidth, NMTileheight, NMTilewidth,
                        &layerReadLatency, &layerReadDynamicEnergy, &tileLeakage, &tileLeakageSRAMInUse, &layerbufferLatency, &layerbufferDynamicEnergy, &layericLatency, &layericDynamicEnergy,
                        &coreLatencyADC, &coreLatencyAccum, &coreLatencyOther, &coreEnergyADC, &coreEnergyAccum, &coreEnergyOther, false, &layerclkPeriod);
            if (param->synchronous) {
                layerReadLatency *= clkPeriod;
                layerbufferLatency *= clkPeriod;
                layericLatency *= clkPeriod;
                coreLatencyADC *= clkPeriod;
                coreLatencyAccum *= clkPeriod;
                coreLatencyOther *= clkPeriod;
            }
            
            double numTileOtherLayer = 0;
            double layerLeakageEnergy = 0;      
            for (int j=0; j<netStructure.size(); j++) {
                if (j != i) {
                    numTileOtherLayer += numTileEachLayer[0][j] * numTileEachLayer[1][j];
                }
            }
            // Anni update: other layer tiles and partial this layer tiles are in leakage
            layerLeakageEnergy = (numTileOtherLayer * tileLeakage + numTileEachLayer[0][i] * numTileEachLayer[1][i] * tileLeakageSRAMInUse) * layerReadLatency;
            
            cout << "layer" << i+1 << "'s readLatency is: " << layerReadLatency*1e9 << "ns" << endl;
            cout << "layer" << i+1 << "'s readDynamicEnergy is: " << layerReadDynamicEnergy*1e12 << "pJ" << endl;
            cout << "layer" << i+1 << "'s leakagePower is: " << numTileEachLayer[0][i] * numTileEachLayer[1][i] * tileLeakage*1e6 << "uW" << endl;
            cout << "layer" << i+1 << "'s leakageEnergy is: " << layerLeakageEnergy*1e12 << "pJ" << endl;
            cout << "layer" << i+1 << "'s buffer latency is: " << layerbufferLatency*1e9 << "ns" << endl;
            cout << "layer" << i+1 << "'s buffer readDynamicEnergy is: " << layerbufferDynamicEnergy*1e12 << "pJ" << endl;
            cout << "layer" << i+1 << "'s ic latency is: " << layericLatency*1e9 << "ns" << endl;
            cout << "layer" << i+1 << "'s ic readDynamicEnergy is: " << layericDynamicEnergy*1e12 << "pJ" << endl;
            
            
            cout << endl;
            cout << "************************ Breakdown of Latency and Dynamic Energy *************************" << endl;
            cout << endl;
            cout << "----------- ADC (or S/As and precharger for SRAM) readLatency is : " << coreLatencyADC*1e9 << "ns" << endl;
            cout << "----------- Accumulation Circuits (subarray level: adders, shiftAdds; PE/Tile/Global level: accumulation units) readLatency is : " << coreLatencyAccum*1e9 << "ns" << endl;
            cout << "----------- Other Peripheries (e.g. decoders, mux, switchmatrix, buffers, IC, pooling and activation units) readLatency is : " << coreLatencyOther*1e9 << "ns" << endl;
            cout << "----------- ADC (or S/As and precharger for SRAM) readDynamicEnergy is : " << coreEnergyADC*1e12 << "pJ" << endl;
            cout << "----------- Accumulation Circuits (subarray level: adders, shiftAdds; PE/Tile/Global level: accumulation units) readDynamicEnergy is : " << coreEnergyAccum*1e12 << "pJ" << endl;
            cout << "----------- Other Peripheries (e.g. decoders, mux, switchmatrix, buffers, IC, pooling and activation units) readDynamicEnergy is : " << coreEnergyOther*1e12 << "pJ" << endl;
            cout << endl;
            cout << "************************ Breakdown of Latency and Dynamic Energy *************************" << endl;
            cout << endl;
            
            chipReadLatency += layerReadLatency;
            chipReadDynamicEnergy += layerReadDynamicEnergy;
            chipLeakageEnergy += layerLeakageEnergy;
            chipLeakage += tileLeakage*numTileEachLayer[0][i] * numTileEachLayer[1][i];
            chipbufferLatency += layerbufferLatency;
            chipbufferReadDynamicEnergy += layerbufferDynamicEnergy;
            chipicLatency += layericLatency;
            chipicReadDynamicEnergy += layericDynamicEnergy;
            
            chipLatencyADC += coreLatencyADC;
            chipLatencyAccum += coreLatencyAccum;
            chipLatencyOther += coreLatencyOther;
            chipEnergyADC += coreEnergyADC;
            chipEnergyAccum += coreEnergyAccum;
            chipEnergyOther += coreEnergyOther;
        }
    } else {
        // pipeline system
        // firstly define system clock
        double systemClock = 0;
        int maxlayer=0;
        
        vector<double> readLatencyPerLayer;
        vector<double> readDynamicEnergyPerLayer;
        vector<double> leakagePowerPerLayer;
        vector<double> bufferLatencyPerLayer;
        vector<double> bufferEnergyPerLayer;
        vector<double> icLatencyPerLayer;
        vector<double> icEnergyPerLayer;
        
        vector<double> coreLatencyADCPerLayer;
        vector<double> coreEnergyADCPerLayer;
        vector<double> coreLatencyAccumPerLayer;
        vector<double> coreEnergyAccumPerLayer;
        vector<double> coreLatencyOtherPerLayer;
        vector<double> coreEnergyOtherPerLayer;
        
        for (int i=0; i<netStructure.size(); i++) {
            // Anni update: add &tileLeakageSRAMInUse
            ChipCalculatePerformance(inputParameter, tech, cell, i, argv[2*i+4], argv[2*i+4], argv[2*i+5], netStructure[i][6],
                        netStructure, markNM, numTileEachLayer, utilizationEachLayer, speedUpEachLayer, tileLocaEachLayer,
                        numPENM, desiredPESizeNM, desiredTileSizeCM, desiredPESizeCM, CMTileheight, CMTilewidth, NMTileheight, NMTilewidth,
                        &layerReadLatency, &layerReadDynamicEnergy, &tileLeakage, &tileLeakageSRAMInUse, &layerbufferLatency, &layerbufferDynamicEnergy, &layericLatency, &layericDynamicEnergy,
                        &coreLatencyADC, &coreLatencyAccum, &coreLatencyOther, &coreEnergyADC, &coreEnergyAccum, &coreEnergyOther, false, &layerclkPeriod);
            if (param->synchronous) {
                layerReadLatency *= clkPeriod;
                layerbufferLatency *= clkPeriod;
                layericLatency *= clkPeriod;
                coreLatencyADC *= clkPeriod;
                coreLatencyAccum *= clkPeriod;
                coreLatencyOther *= clkPeriod;
            }           
            
            if( layerReadLatency>systemClock) {
                maxlayer=i;
                systemClock = layerReadLatency;
                chipbufferLatency = layerbufferLatency;
                chipicLatency  = layericLatency;
                chipLatencyADC  = coreLatencyADC;
                chipLatencyAccum  = coreLatencyAccum;
                chipLatencyOther  = coreLatencyOther;

            }
            maxlayer = maxlayer+1;
            
            readLatencyPerLayer.push_back(layerReadLatency);
            readDynamicEnergyPerLayer.push_back(layerReadDynamicEnergy);
            // Anni update: average leakage power considering read latency
            leakagePowerPerLayer.push_back(numTileEachLayer[0][i] * numTileEachLayer[1][i] * (tileLeakage * (systemClock-readLatencyPerLayer[i]) + tileLeakageSRAMInUse * readLatencyPerLayer[i]) / systemClock);
            bufferLatencyPerLayer.push_back(layerbufferLatency);
            bufferEnergyPerLayer.push_back(layerbufferDynamicEnergy);
            icLatencyPerLayer.push_back(layericLatency);
            icEnergyPerLayer.push_back(layericDynamicEnergy);
            
            coreLatencyADCPerLayer.push_back(coreLatencyADC);
            coreEnergyADCPerLayer.push_back(coreEnergyADC);
            coreLatencyAccumPerLayer.push_back(coreLatencyAccum);
            coreEnergyAccumPerLayer.push_back(coreEnergyAccum);
            coreLatencyOtherPerLayer.push_back(coreLatencyOther);
            coreEnergyOtherPerLayer.push_back(coreEnergyOther);

        }
        
        for (int i=0; i<netStructure.size(); i++) {
            
            cout << "-------------------- Estimation of Layer " << i+1 << " ----------------------" << endl;

            cout << "layer" << i+1 << "'s readLatency is: " << readLatencyPerLayer[i]*1e9 << "ns" << endl;
            cout << "layer" << i+1 << "'s readDynamicEnergy is: " << readDynamicEnergyPerLayer[i]*1e12 << "pJ" << endl;
            cout << "layer" << i+1 << "'s leakagePower is: " << leakagePowerPerLayer[i]*1e6 << "uW" << endl;
            // Anni update: average leakage power considering read latency
            cout << "layer" << i+1 << "'s leakageEnergy is: " << leakagePowerPerLayer[i] * systemClock *1e12 << "pJ" << endl;
            cout << "layer" << i+1 << "'s buffer latency is: " << bufferLatencyPerLayer[i]*1e9 << "ns" << endl;
            cout << "layer" << i+1 << "'s buffer readDynamicEnergy is: " << bufferEnergyPerLayer[i]*1e12 << "pJ" << endl;
            cout << "layer" << i+1 << "'s ic latency is: " << icLatencyPerLayer[i]*1e9 << "ns" << endl;
            cout << "layer" << i+1 << "'s ic readDynamicEnergy is: " << icEnergyPerLayer[i]*1e12 << "pJ" << endl;
            cout << "layer" << i+1 << "'s accum latency is: " << coreLatencyAccumPerLayer[i]*1e9 << "ns" << endl;
            cout << "layer" << i+1 << "'s LatencyOther is: " << coreLatencyOtherPerLayer[i]*1e9 << "ns" << endl;
            
            cout << endl;
            cout << "************************ Breakdown of Latency and Dynamic Energy *************************" << endl;
            cout << endl;
            cout << "----------- ADC (or S/As and precharger for SRAM) readLatency is : " << coreLatencyADCPerLayer[i]*1e9 << "ns" << endl;
            cout << "----------- Accumulation Circuits (subarray level: adders, shiftAdds; PE/Tile/Global level: accumulation units) readLatency is : " << coreLatencyAccumPerLayer[i]*1e9 << "ns" << endl;
            cout << "----------- Other Peripheries (e.g. decoders, mux, switchmatrix, buffers, IC, pooling and activation units) readLatency is : " << coreLatencyOtherPerLayer[i]*1e9 << "ns" << endl;
            cout << "----------- ADC (or S/As and precharger for SRAM) readDynamicEnergy is : " << coreEnergyADCPerLayer[i]*1e12 << "pJ" << endl;
            cout << "----------- Accumulation Circuits (subarray level: adders, shiftAdds; PE/Tile/Global level: accumulation units) readDynamicEnergy is : " << coreEnergyAccumPerLayer[i]*1e12 << "pJ" << endl;
            cout << "----------- Other Peripheries (e.g. decoders, mux, switchmatrix, buffers, IC, pooling and activation units) readDynamicEnergy is : " << coreEnergyOtherPerLayer[i]*1e12 << "pJ" << endl;
            cout << endl;
            cout << "************************ Breakdown of Latency and Dynamic Energy *************************" << endl;
            cout << endl;
            /*
            chipReadLatency = systemClock;
            chipReadDynamicEnergy += readDynamicEnergyPerLayer[i];
            chipLeakageEnergy += leakagePowerPerLayer[i] * (systemClock-readLatencyPerLayer[i]);
            chipLeakage += leakagePowerPerLayer[i];
            chipbufferLatency = bufferLatencyPerLayer[maxlayer]; // modified
            chipbufferReadDynamicEnergy += bufferEnergyPerLayer[i];
            chipicLatency = icLatencyPerLayer[maxlayer]; // modified
            chipicReadDynamicEnergy += icEnergyPerLayer[i];
            
            chipLatencyADC = coreLatencyADCPerLayer[maxlayer]; // modified
            chipLatencyAccum = coreLatencyAccumPerLayer[maxlayer]; // modified
            chipLatencyOther = coreLatencyOtherPerLayer[maxlayer]; // modified
            chipEnergyADC += coreEnergyADCPerLayer[i];
            chipEnergyAccum += coreEnergyAccumPerLayer[i];
            chipEnergyOther += coreEnergyOtherPerLayer[i];
            */
            chipReadLatency = systemClock;
            chipReadDynamicEnergy += readDynamicEnergyPerLayer[i];
            // Anni update: average leakage power considering read latency
            chipLeakageEnergy += leakagePowerPerLayer[i] * systemClock;
            chipLeakage += leakagePowerPerLayer[i];
            //chipbufferLatency = MAX(chipbufferLatency, bufferLatencyPerLayer[i]);
            chipbufferReadDynamicEnergy += bufferEnergyPerLayer[i];
            //chipicLatency = MAX(chipicLatency, icLatencyPerLayer[i]);
            chipicReadDynamicEnergy += icEnergyPerLayer[i];
            
            //chipLatencyADC = MAX(chipLatencyADC, coreLatencyADCPerLayer[i]);
            //chipLatencyAccum = MAX(chipLatencyAccum, coreLatencyAccumPerLayer[i]);
            //chipLatencyOther = MAX(chipLatencyOther, coreLatencyOtherPerLayer[i]);
            chipEnergyADC += coreEnergyADCPerLayer[i];
            chipEnergyAccum += coreEnergyAccumPerLayer[i];
            chipEnergyOther += coreEnergyOtherPerLayer[i];      
        
        }
        
    }
    
    cout << "------------------------------ Summary --------------------------------" <<  endl;
    cout << endl;
    cout << "ChipArea : " << chipArea*1e12 << "um^2" << endl;
    cout << "Chip total CIM array : " << chipAreaArray*1e12 << "um^2" << endl;
    cout << "Total IC Area on chip (Global and Tile/PE local): " << chipAreaIC*1e12 << "um^2" << endl;
    cout << "Total ADC (or S/As and precharger for SRAM) Area on chip : " << chipAreaADC*1e12 << "um^2" << endl;
    cout << "Total Accumulation Circuits (subarray level: adders, shiftAdds; PE/Tile/Global level: accumulation units) on chip : " << chipAreaAccum*1e12 << "um^2" << endl;
    cout << "Other Peripheries (e.g. decoders, mux, switchmatrix, buffers, pooling and activation units) : " << chipAreaOther*1e12 << "um^2" << endl;
    cout << endl;
    if (! param->pipeline) {
        if (param->synchronous) cout << "Chip clock period is: " << clkPeriod*1e9 << "ns" <<endl;
        cout << "Chip layer-by-layer readLatency (per image) is: " << chipReadLatency*1e9 << "ns" << endl;
        cout << "Chip total readDynamicEnergy is: " << chipReadDynamicEnergy*1e12 << "pJ" << endl;
        cout << "Chip total leakage Energy is: " << chipLeakageEnergy*1e12 << "pJ" << endl;
        cout << "Chip total leakage Power is: " << chipLeakage*1e6 << "uW" << endl;
        cout << "Chip buffer readLatency is: " << chipbufferLatency*1e9 << "ns" << endl;
        cout << "Chip buffer readDynamicEnergy is: " << chipbufferReadDynamicEnergy*1e12 << "pJ" << endl;
        cout << "Chip ic readLatency is: " << chipicLatency*1e9 << "ns" << endl;
        cout << "Chip ic readDynamicEnergy is: " << chipicReadDynamicEnergy*1e12 << "pJ" << endl;
    } else {
        if (param->synchronous) cout << "Chip clock period is: " << clkPeriod*1e9 << "ns" <<endl;
        cout << "Chip pipeline-system-clock-cycle (per image) is: " << chipReadLatency*1e9 << "ns" << endl;
        cout << "Chip pipeline-system readDynamicEnergy (per image) is: " << chipReadDynamicEnergy*1e12 << "pJ" << endl;
        cout << "Chip pipeline-system leakage Energy (per image) is: " << chipLeakageEnergy*1e12 << "pJ" << endl;
        cout << "Chip pipeline-system leakage Power (per image) is: " << chipLeakage*1e6 << "uW" << endl;
        cout << "Chip pipeline-system buffer readLatency (per image) is: " << chipbufferLatency*1e9 << "ns" << endl;
        cout << "Chip pipeline-system buffer readDynamicEnergy (per image) is: " << chipbufferReadDynamicEnergy*1e12 << "pJ" << endl;
        cout << "Chip pipeline-system ic readLatency (per image) is: " << chipicLatency*1e9 << "ns" << endl;
        cout << "Chip pipeline-system ic readDynamicEnergy (per image) is: " << chipicReadDynamicEnergy*1e12 << "pJ" << endl;
        cout << "Chip pipeline-system Accum latency (per image) is: " << chipLatencyAccum*1e9 << "ns" << endl;
        cout << "Chip pipeline-system Other latency (per image) is: " << chipLatencyOther*1e9 << "ns" << endl;
    }
    
    cout << endl;
    cout << "************************ Breakdown of Latency and Dynamic Energy *************************" << endl;
    cout << endl;
    cout << "----------- ADC (or S/As and precharger for SRAM) readLatency is : " << chipLatencyADC*1e9 << "ns" << endl;
    cout << "----------- Accumulation Circuits (subarray level: adders, shiftAdds; PE/Tile/Global level: accumulation units) readLatency is : " << chipLatencyAccum*1e9 << "ns" << endl;
    cout << "----------- Other Peripheries (e.g. decoders, mux, switchmatrix, buffers, IC, pooling and activation units) readLatency is : " << chipLatencyOther*1e9 << "ns" << endl;
    cout << "----------- ADC (or S/As and precharger for SRAM) readDynamicEnergy is : " << chipEnergyADC*1e12 << "pJ" << endl;
    cout << "----------- Accumulation Circuits (subarray level: adders, shiftAdds; PE/Tile/Global level: accumulation units) readDynamicEnergy is : " << chipEnergyAccum*1e12 << "pJ" << endl;
    cout << "----------- Other Peripheries (e.g. decoders, mux, switchmatrix, buffers, IC, pooling and activation units) readDynamicEnergy is : " << chipEnergyOther*1e12 << "pJ" << endl;
    cout << endl;
    cout << "************************ Breakdown of Latency and Dynamic Energy *************************" << endl;
    cout << endl;
    
    cout << endl;
    cout << "----------------------------- Performance -------------------------------" << endl;
    if (! param->pipeline) {
        if(param->validated){
            cout << "Energy Efficiency TOPS/W (Layer-by-Layer Process): " << numComputation/(chipReadDynamicEnergy*1e12+chipLeakageEnergy*1e12) << endl;    // post-layout energy increase, zeta = 1.23 by default
        }else{
            cout << "Energy Efficiency TOPS/W (Layer-by-Layer Process): " << numComputation/(chipReadDynamicEnergy*1e12+chipLeakageEnergy*1e12) << endl;
        }
        cout << "Throughput TOPS (Layer-by-Layer Process): " << numComputation/(chipReadLatency*1e12) << endl;
        cout << "Throughput FPS (Layer-by-Layer Process): " << 1/(chipReadLatency) << endl;
        cout << "Compute efficiency TOPS/mm^2 (Layer-by-Layer Process): " << numComputation/(chipReadLatency*1e12)/(chipArea*1e6) << endl;
        cout << "Subarray cell initial width: " << cell.widthInFeatureSize << endl;
        cout << "Subarray cell initial height: " <<  cell.heightInFeatureSize<< endl;
        cout << "Subarray cell minimum used width for design: " << param->minimumCellWidth << endl;
        cout << "Subarray cell minimum used height for design: " << param->minimumCellHeight << endl;
        cout << "Subarray cell final width for design: " << param->finalCellWidth << endl;
        cout << "Subarray cell final height for design: " << param->finalCellHeight << endl;
    } else {
        if(param->validated){
            cout << "Energy Efficiency TOPS/W (Pipelined Process): " << numComputation/(chipReadDynamicEnergy*1e12+chipLeakageEnergy*1e12) << endl; // post-layout energy increase, zeta = 1.23 by default
        }else{
            cout << "Energy Efficiency TOPS/W (Pipelined Process): " << numComputation/(chipReadDynamicEnergy*1e12+chipLeakageEnergy*1e12) << endl;
        }
        cout << "Throughput TOPS (Pipelined Process): " << numComputation/(chipReadLatency*1e12) << endl;
        cout << "Throughput FPS (Pipelined Process): " << 1/(chipReadLatency) << endl;
        cout << "Compute efficiency TOPS/mm^2 (Pipelined Process): " << numComputation/(chipReadLatency*1e12)/(chipArea*1e6) << endl;
        cout << "Subarray cell initial width: " << cell.widthInFeatureSize << endl;
        cout << "Subarray cell initial height: " <<  cell.heightInFeatureSize << endl;
        cout << "Subarray cell minimum used width for design: " << param->minimumCellWidth << endl;
        cout << "Subarray cell minimum used height for design: " << param->minimumCellHeight << endl;
        cout << "Subarray cell final width for design: " << param->finalCellWidth << endl;
        cout << "Subarray cell final height for design: " << param->finalCellHeight << endl;
        cout << "sparsity" << param->zeroweightcount/param->weightcount <<endl;
    }
    cout << "-------------------------------------- Hardware Performance Done --------------------------------------" <<  endl;
    cout << endl;
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(stop-start);
    cout << "------------------------------ Simulation Performance --------------------------------" <<  endl;
    cout << "Total Run-time of NeuroSim: " << duration.count() << " seconds" << endl;
    cout << "------------------------------ Simulation Performance --------------------------------" <<  endl;
    
    // string date="230504_Resnet18";

    // string date= "DenseNET_Pipielined";
    // string date= "ResNET18_Pipielined";
    string date= param->filename;
    // string date="junmo";
    fstream read;
    // string date="230504_Macro0.8";

    read.open("/home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch/NeuroSIM/Data_TechnologyUpdate/"+date+"_overall_metrics.csv",fstream::app);     
    read<<param->technode<<", "<< param->toggleindex << ", " << chipArea*1e12<<", "<<chipAreaArray*1e12<<", "<<chipAreaIC*1e12<<", ";
    read<<chipAreaADC*1e12<<", "<<chipAreaAccum*1e12<<", "<<chipAreaOther*1e12<<", ";
    read<<clkPeriod*1e9<<", ";
    read<<chipReadLatency*1e9<<", ";
    read<<chipReadDynamicEnergy*1e12<<", ";
    read<<chipLeakageEnergy*1e12<<", ";
    read<<chipLeakage*1e6<<", ";
    read<<chipbufferLatency*1e9<<", ";
    read<<chipbufferReadDynamicEnergy*1e12<<", ";
    read<<chipicLatency*1e9 <<", ";
    read<<chipicReadDynamicEnergy*1e12 <<", ";
    read<<chipLatencyADC*1e9<<", ";
    read<<chipLatencyAccum*1e9 <<", ";
    read<<chipLatencyOther*1e9 <<", ";
    read<<chipEnergyADC*1e12 <<", ";
    read<<chipEnergyAccum*1e12<<", ";
    read<<chipEnergyOther*1e12<<", ";
    read<<numComputation/(chipReadDynamicEnergy*1e12+chipLeakageEnergy*1e12)<<", ";
    read<<numComputation/(chipReadLatency*1e12)<<", ";
    read<<1/(chipReadLatency) <<", ";
    read<<numComputation/(chipReadLatency*1e12)/(chipArea*1e6) <<", ";

    read<<cell.widthInFeatureSize<<", ";
    read<<cell.heightInFeatureSize<<", ";
    read<<param->finalCellWidth <<", ";
    read<<param->finalCellHeight<<", ";
    read<<numComputation<<", ";
    read<<param->globalBufferCoreSizeRow<<", ";
    read<<param->tileBufferCoreSizeRow<<", ";
    read <<param->zeroweightcount/param->weightcount <<", ";
    read<<"TR"<<", ";
    read<<param->averageTR<<", ";
    read<<param->maxGlobalBusWidth <<", ";
    read<<param->globalBusType <<", ";
    read<<desiredNumTileCM<<", ";
    read<<param->globalbuswidth<<", ";

    read<<endl;


    fstream read2;
    read2.open("/home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch/NeuroSIM/Data_TechnologyUpdate/"+date+"_subarraylevel.csv",fstream::app); 
    read2<<"technode"<<", "<<param->technode<<", "<< param->toggleindex <<", ";
    read2<<"subarrayheight"<<", "<<param->subarray_height<<", ";
    read2<<"subarraywidth"<<", "<<param->subarray_width<<", ";
    read2<<"subarray_usedarea"<<", "<< param->subarray_usedarea<<", ";
    read2<<"addertree_width"<<", "<< param->addertree_width <<", ";
    read2<<"wlSwitchMatrix_width"<<", "<< param->wlSwitchMatrix_width  <<", ";
    read2<<"InputDFF_width" << ", " << param->Inputdff_width  <<", ";
    read2<<"DFF_width" << ", " <<param->dff_width <<", ";
    read2<<"wlDecoder_width "<<", "<<   param->wlDecoder_width  <<", ";
    read2<<"shiftAddInput_height"<<", "<<       param->shiftAddInput_width <<", ";
    read2<<"sramWriteDriver_height"<<", "<<     param->sramWriteDriver_height<<", ";
    read2<<"precharger_height"<<", "<<  param->precharger_height <<", ";
    read2<<"senseAmp_height"<<", "<<        param->senseAmp_height  <<", ";
    read2<<"addertree_delay"<<", "<<        param->addertree_delay   <<", ";
    read2<<"precharge_delay"<<", "<<param->precharge_delay<<", ";
    read2<<"col_delay"<<", "<<param->col_delay<<", ";
    read2<<"ADC_delay"<<", "<<param->ADC_delay<<", ";
    read2<<"multisense_delay"<<", "<<param->multisense_delay<<", ";
    read2<<"WL_delay"<<", "<<param->WL_delay<<", ";
    read2<<"encoder_delay"<<", "<<param->multiencoder_delay<<", ";
    read2<<"mux_delay"<<", "<<param->mux_delay<<", ";
    read2<<"NOR_delay"<<", "<<param->NOR_delay<<", ";
    read2<<"row_delay"<<", "<<param->row_delay<<", ";
    read2<<"mux_res"<<", "<<param->mux_res<<", ";
    read2<<"capcell_SRAM"<<", "<<param->capcell_SRAM<<", ";
    read2<<"capcell_row_SRAM"<<", "<<param->capcell_row_SRAM<<", ";
    read2<<"rescell_SRAM"<<", "<<param->rescell_SRAM<<", ";
    
    read2<<"peBufferarea"<<", "<< param->peBufferarea_total<<", ";
    read2<<"tileBufferarea"<<", "<< param->tileBufferarea_total<<", ";
    read2<<"chipBufferarea"<<", "<< param->globalBufferarea_total<<", ";

    read2<<endl;
    
    fstream read3;
    read3.open("/home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch/NeuroSIM/Data_TechnologyUpdate/"+date+"_Hierarchical breakdown.csv",fstream::app);
    read3<<"technode"<<", "<<param->technode<<", ";
    read3<<"toggleindex" <<", "<< param->toggleindex <<", ";
    read3<<"subarray_selfleakage "<<", "<<param->subarray_selfleakage<<", ";
    read3<<"PE_selfleakage "<<", "<<param->PE_selfleakage<<", ";
    read3<<"Tile_selfleakage"<<", "<<param->Tile_selfleakage<<", ";
    read3<<"totalsubarrayoperations "<<", "<<param->totalsubarrayoperations<<", ";
    read3<<"Macrototaldynamic "<<", "<<param->Macrototaldynamic<<", ";
    read3<<"PE_bus "<<", "<<param->PE_bus<<", ";
    read3<<"PE_Addertree "<<", "<<param->PE_Addertree<<", ";
    read3<<"PE_buffer "<<", "<<param->PE_buffer<<", ";
    read3<<"PE_total "<<", "<<param->PE_total<<", ";
    read3<< "PE_leakage "<<", "<<param->PE_leakage<<", ";
    read3<< "Tile_Leakage "<<", "<<param->Tile_Leakage<<", "; 
    read3<<"Tile_htree "<<", "<<param->Tile_htree<<", ";
    read3<<"Tile_addertree "<<", "<<param->Tile_addertree<<", ";
    read3<<"Tile_sigmoid "<<", "<<param->Tile_sigmoid<<", ";
    read3<<"Tile_ReLU "<<", "<<param->Tile_ReLU<<", ";
    read3<<"Tile_buffer "<<", "<<param->Tile_buffer<<", ";
    read3<<"Tile_total "<<", "<<param->Tile_total<<", ";
    read3<<"Chip_buffer "<<", "<<param->Chip_buffer<<", ";
    read3<<"Chip_htree "<<", "<<param->Chip_htree<<", ";
    read3<<"Chip_addertree "<<", "<<param->Chip_addertree<<", ";
    read3<<"Chip_sigmoid "<<", "<<param->Chip_sigmoid<<", ";
    read3<<"Chip_ReLU "<<", "<<param->Chip_ReLU<<", ";
    read3<<"Chip_maxpool "<<", "<<param->Chip_maxpool<<", ";
    read3<<"Chip_total "<<", "<<param->Chip_total<<", ";
    read3<<"Total dynamic "<<", "<<chipReadDynamicEnergy<<", ";
    read3<<"Total leakage "<<", "<<chipLeakageEnergy<<", ";


    read3<<endl;

    fstream read4;

    double TOPS_W= 2*256*64/( param->Macrototaldynamic*4/param->totalsubarrayoperations)*1/pow(10,12) ;
    double TOPS_mm= 2*256*64/ param->Macrototallatency/( param->subarray_height *param->subarray_width) /pow(10,6)/pow(10,12);
    read4.open("/home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch/NeuroSIM/Data_TechnologyUpdate/"+date+"_Macrobreakdown.csv",fstream::app);
    read4<<"technode"<<", "<<param->technode<<", "<<"toggleindex"<<", "<<param->toggleindex<<", ";
    read4<<"mem cell type"<< ", "<< param->memcelltype<<", ";
    read4<<"buffernumber"<< ", "<< param->buffernumber<<", ";
    read4<<"toggle index"<< ", "<< param->toggleindex<<", ";
    read4<< "Totalsubarray operations"<<", "<<param->totalsubarrayoperations<< ", ";
    read4<< "Average macro dynamic energy"<<", "<<param->Macrototaldynamic*4/param->totalsubarrayoperations<< ", ";
    read4<< "1 Macro delay"<<", "<<param->Macrototallatency<< ", ";
    read4<< "1 Macro area (mm^2)"<<", "<<( param->subarray_height *param->subarray_width) * pow(10,6)<< ", ";
    read4<< "TOPS"<<", "<<2*256*64/ param->Macrototallatency*1/pow(10,12) << ", ";
    read4<< "TOPS/W"<<", "<<TOPS_W << ", ";
    read4<< "TOPS/mm^2"<<", "<<TOPS_mm  << ", ";
    read4<< "FOM"<<", "<<TOPS_W * TOPS_mm << ", ";
    

    read4<<"Total macro leakage"<<", "<<param->subarray_leakage<< ", ";
    read4<<"Total DCIM (NOR+Latch) array leakage"<<", "<<param->DCIM_array_leakage << ", ";
    read4<<"SRAM_peri_leakage"<<", "<<param->SRAM_peri_leakage<< ", ";
    read4<<"ShiftAdd_leakage"<<", "<<param->ShiftAdd_leakage<< ", ";
    read4<<"Adder_leakage"<<", "<<param->Adder_leakage<< ", ";
    read4<<"Buffer_leakage"<<", "<<param->Buffer_leakage<< ", ";
    read4<<"Switchmatrix_leakage"<<", "<<param->Switchmatrix_leakage<< ", ";
    read4<<"Repeater_leakage"<<", "<<param-> repeater_leakage<< ", ";
    read4<<"Bus_leakage"<<", "<<param-> busleakage<< ", ";
    
    
    read4<<"readDynamicEnergyBuffer"<<", "<<param->macro_readDynamicEnergyBuffer/param->totalsubarrayoperations<<", ";
    read4<<"readDynamicEnergyShiftAdd"<<", "<<param->macro_readDynamicEnergyShiftAdd/param->totalsubarrayoperations << ", ";
    read4<<"readDynamicEnergyAccum"<<", "<<param->macro_readDynamicEnergyAccum/param->totalsubarrayoperations << ", ";
    read4<<"readDynamicEnergySwitchmatrix"<<", "<<param->macro_readDynamicSwitchmatrix/param->totalsubarrayoperations << ", ";
    read4<<"readDynamicEnergyAdder"<<", "<<param->macro_readDynamicEnergyAdder/param->totalsubarrayoperations << ", ";
    read4<<"readDynamicEnergyInterconnect"<<", "<<param->macro_readDynamicInterconnect/param->totalsubarrayoperations << ", ";
    read4<<"readDynamicEnergyADC"<<", "<<param->macro_readDynamicADC/param->totalsubarrayoperations << ", ";
    read4<<"readDynamicEnergyDFFinShiftAdd"<<", "<<param->macro_readDynamicDFFinShiftAdd/param->totalsubarrayoperations << ", ";
    read4<<"DCIM_energy_recalculated"<<", "<< param->DCIM_energy_recalculated<< ", ";
    read4<<"DC_ADC_min"<<", "<< param->DC_ADC_min<< ", ";
    read4<<"DC_ADC_max"<<", "<< param->DC_ADC_max<< ", ";
    read4<<"Switching_ADC"<<", "<< param->Switching_ADC<< ", ";
        
        
        
    read4<<"NOR_delay"<<", "<<param -> NOR_delay << ", ";
    read4<<"addertree_delay"<<", "<<param -> addertree_delay << ", ";
    read4<<"addedbufferlatency"<<", "<<param -> addedbufferlatency << ", ";
    read4<<"encoder_delay"<<", "<<param->multiencoder_delay<<", ";
    read4<<"WL_delay"<<", "<<param->WL_delay<<", ";
    read4<<"mux_delay"<<", "<<param->mux_delay<<", ";
    read4<<"row_delay"<<", "<<param->row_delay<<", ";
    read4<<"precharge_delay"<<", "<<param->precharge_delay<<", ";
    read4<<"col_delay"<<", "<<param->col_delay<<", ";
    read4<<"ADC_delay"<<", "<<param->ADC_delay<<", ";
    read4<<"multisense_delay"<<", "<<param->multisense_delay<<", ";
    read4<<"clock_period"<< ", "<<clkPeriod<<", ";

    read4<< "param->capNORInput"<<", "<<param-> capNORInput<< ", ";
    read4<< "param->capNOROutput"<<", "<<param-> capNOROutput<< ", ";
    read4<< "param->DCIMline_R2"<<", "<<param->DCIMline_R2<< ", ";
    read4<< "param->DCIMline_R1"<<", "<<param->DCIMline_R1<< ", ";
    read4<< "param->DCIMline_C2"<<", "<<param->DCIMline_C2<< ", ";
    read4<< "param->DCIMline_C1"<<", "<<param->DCIMline_C1<< ", ";
    read4<< "param->resRow_DCIM"<<", "<<param-> resRow_DCIM<< ", ";
    read4<< "param->resCol_DCIM"<<", "<<param-> resCol_DCIM<< ", ";
    read4<< "param->Metal0_unitwireresis"<<", "<<param->Metal0_unitwireresis<< ", ";
    read4<< "param->Metal1_unitwireresis"<<", "<<param->Metal1_unitwireresis<< ", ";
    read4<< "columncap"<<", "<<param->columncap<<", ";
    read4<< "rowunitcap"<<", "<<param-> unitcap<<", ";
    read4<< "caprow_debug"<<", "<< param->caprow_debug<<", ";
    read4<< "numrow_debug"<<", "<< param->numrow_debug<<", ";
    read4<< "reference_energy_peri"<<", "<< param->reference_energy_peri<<", ";
    read4<< "resaccess"<<", "<< param->resCellAccess<<", ";


    read4<< "param->lengthRow"<<", "<< param->lengthRow<<", ";
    read4<< "TOPS"<<", "<<2*256*64/ param->Macrototallatency*1/pow(10,12) << ", ";
    read4<< "TOPS/W"<<", "<<TOPS_W << ", ";
    read4<< "TOPS/mm^2"<<", "<<TOPS_mm << ", ";
    read4<< "FOM"<<", "<<TOPS_W * TOPS_mm << ", ";
    read4<<"buffersizeratio"<<", "<<param->buffersizeratio << ", ";
    read4<<"switchsizeratio_DCIM"<<", "<<param->switchmatrixsizeratio_DCIM<<", ";
    read4<<"sectionresistance"<<", "<<param->sectionresistance<< ", ";
    read4<<"switchmatrix_finnum"<<", "<<param->finfetswitchmatrixdrive<< ", ";
    read4<<"finfetonresistance"<<", "<<param->finfetonresistance<< ", ";
    read4<<"repeaterfinnum"<<", "<<param->repeaterfinnum<< ", ";
    read4<<"repeaterdriveres"<<", "<<param->targetdriveres<< ", ";
    read4<<"sizingfactor_MUX"<<", "<<param->sizingfactor_MUX<<", ";
    read4<<"sizingfactor_MUX_Last"<<", "<<param->sizingfactor_MUX_Last<<", ";
    read4<<"switchmatrixsizeratio_ACIM"<<", "<<param->switchmatrixsizeratio_ACIM<<", ";
    read4<<"newswitchmatrixsizeratio_ACIM"<<", "<<param->newswitchmatrixsizeratio_ACIM<<", ";
    read4<<"capratio"<<", "<<param->mirrorcap/ (param->columncap + (param->mirrorcap/(param->dumcolshared - 1))) <<". ";
    read4<< "Power Density (W/mm2)"<<", "<<param->Macrototaldynamic*4/param->Macrototallatency / (( param->subarray_height *param->subarray_width) * pow(10,6)) << ", ";

    read4<< "Access Fincount"<< param->access_fincount <<", ";
    read4<< "ADC Area" << param->ADC_area <<", ";
    read4<< "levelshifter_area" << param->levelshifter_area<<", ";
    read4<< "array_area" << param->array_area<<", ";

    read4<< endl;

    fstream read5;
    read5.open("/home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch/NeuroSIM/Data_TechnologyUpdate/"+date+"_caplist.csv",fstream::app);
    read5<<"technode"<<", "<<param->technode<<", ";
    read5<<param->adderinputcap <<", ";
    read5<<param->adderoutputcap <<", ";
    read5<<tech.vdd <<", ";
    read5<<param->dff_cap<<", ";
    read5<< endl;

    fstream read6;
    read6.open("/home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch/NeuroSIM/Data_TechnologyUpdate/"+date+"_areadebug.csv",fstream::app);
    read6<<"technode"<<", "<<param->technode<<", ";
    read6 << "wlSwitchMatrix" << param->areaothers_subarray_wlSwitchMatrix << ", ";
    read6 << "slSwitchMatrix" << param->areaothers_subarray_slSwitchMatrix << ", ";
    read6 << "mux" << param->areaothers_subarray_mux << ", ";
    read6 << "muxDecoder" << param->areaothers_subarray_muxDecoder << ", ";
    read6 << "levelshifter" << param->areaothers_subarray_levelshifter << ", ";
    read6 << "ADC" << param->areaothers_subarray_ADC<< ", ";
    read6 << "area_LS_biggest" << param->area_LS_biggest << ", ";
    read6 << "levelw" << param->area_levelw <<", ";
    read6 << "levelh" << param->area_levelh <<", ";
    read6 << "CPP_const" << param->CPP_const <<", ";
    read6 << "numFoldedPMOS" << param->numFoldedPMOS <<", ";
    read6 << "numFoldedNMOS" << param->numFoldedNMOS <<", ";
    read6 << "NumPFin" << param->NumPFin <<", ";
    read6 << "ADCwidth" << param->ADCwidth <<", ";
    read6 << "ADCheight" << param->ADCheight <<", ";
    read6 << "currentSenseAmp_area " << param->currentSenseAmp_area <<", ";
    read6 << "multilevelSAEncoder_area " << param->areaothers_subarray_multilevelSAEncoder <<", ";
    read6 << "subarray_shiftAdd_area " << param->areaothers_subarray_shiftAdd<<", ";
    read6 << "subarray_PEadder_area " << param->PEadder_area <<", ";
    read6 << endl;

    return 0;
}

vector<vector<double> > getNetStructure(const string &inputfile) {
    ifstream infile(inputfile.c_str());      
    string inputline;
    string inputval;
    
    int ROWin=0, COLin=0;      
    if (!infile.good()) {        
        cerr << "Error: the input file cannot be opened!" << endl;
        exit(1);
    }else{
        while (getline(infile, inputline, '\n')) {       
            ROWin++;                                
        }
        infile.clear();
        infile.seekg(0, ios::beg);      
        if (getline(infile, inputline, '\n')) {        
            istringstream iss (inputline);      
            while (getline(iss, inputval, ',')) {       
                COLin++;
            }
        }   
    }
    infile.clear();
    infile.seekg(0, ios::beg);          

    vector<vector<double> > netStructure;               
    for (int row=0; row<ROWin; row++) { 
        vector<double> netStructurerow;
        getline(infile, inputline, '\n');             
        istringstream iss;
        iss.str(inputline);
        for (int col=0; col<COLin; col++) {       
            while(getline(iss, inputval, ',')){ 
                istringstream fs;
                fs.str(inputval);
                double f=0;
                fs >> f;                
                netStructurerow.push_back(f);           
            }           
        }       
        netStructure.push_back(netStructurerow);
    }
    infile.close();
    
    return netStructure;
    netStructure.clear();
}   

