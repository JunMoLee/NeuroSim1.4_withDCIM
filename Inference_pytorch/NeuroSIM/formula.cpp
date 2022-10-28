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
*
*   Rui Liu         Email: rliu51 at asu dot edu
********************************************************************************/

#include <cmath>
#include <cstdlib>
#include <iostream>
#include "constant.h"
#include "formula.h"

using namespace std;

/* Beyond 22 nm technology, the value capIdealGate is the sum of capIdealGate and capOverlap and capFringe */
double CalculateGateCap(double width, Technology tech) {
    double widthEff = 0;
    if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) {
        widthEff = width;
    } else if (tech.featureSize >= 7 * 1e-9) { // adding GAA, 5nm cases (Junmo)
        width *= tech.PitchFin/(2 * tech.featureSize);
        widthEff = ceil(width/tech.PitchFin)*(2*tech.heightFin + tech.widthFin);
    } else if (tech.featureSize >= 3 * 1e-9) {
        width *= tech.effective_width/(2 * tech.featureSize);
        widthEff = width;
    } 
    else{
        width *= tech.effective_width/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
        widthEff = width;
    }
    return (tech.capIdealGate + tech.capOverlap + tech.capFringe) * widthEff   // 3 * tech.capFringe
           + tech.phyGateLength * tech.capPolywire;
}

double CalculateGateArea(     // Calculate layout area and width of logic gate given fixed layout height
    int gateType, int numInput,
    double widthNMOS, double widthPMOS,
    double heightTransistorRegion, Technology tech,
    double *height, double *width) {

    if (tech.featureSize <= 14 * 1e-9) {  // finfet  or GAA

    

    if (tech.featureSize > 2 * 1e-9) {
        widthNMOS *= tech.PitchFin/(2 * tech.featureSize); // for GAA, number of nanosheets is calculated when divided by (2 * tech.featureSize)
        widthPMOS *= tech.PitchFin/(2 * tech.featureSize); // for GAA, number of nanosheets is calculated when divided by (2 * tech.featureSize) 
    }

    else if (tech.featureSize <= 2 * 1e-9) // GAA case
    {
    widthNMOS *= 1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
    widthPMOS *= 1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
    }

    heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_FINFET/MAX_TRANSISTOR_HEIGHT);
    // account for new cell height technology trend
    if (tech.featureSize == 10 * 1e-9)
    heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_10nm /MAX_TRANSISTOR_HEIGHT_FINFET);
    else if (tech.featureSize == 7 * 1e-9)
    heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_7nm /MAX_TRANSISTOR_HEIGHT_FINFET);
    else if (tech.featureSize == 5 * 1e-9)
    heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_5nm /MAX_TRANSISTOR_HEIGHT_FINFET);
    else if (tech.featureSize == 3 * 1e-9)
    heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_3nm /MAX_TRANSISTOR_HEIGHT_FINFET);
    else if (tech.featureSize == 2 * 1e-9)
    heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_2nm /MAX_TRANSISTOR_HEIGHT_FINFET);
    else if (tech.featureSize == 1 * 1e-9)
    heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_1nm /MAX_TRANSISTOR_HEIGHT_FINFET);
    else
    heightTransistorRegion *= 1;

    }
    
    double  ratio = widthPMOS / (widthPMOS + widthNMOS);
    double maxWidthPMOS=0, maxWidthNMOS=0;
    int maxNumPFin=0, maxNumNFin=0; /* Max number of fins for the specified cell height */

    // add GAA
    int NumPSheet=0;
    int NumNSheet=0;
    double maxNumPSheet=0;
    double maxNumNSheet=0;
    double unitWidthRegionP, unitWidthRegionN;
    double widthRegionP, widthRegionN;
    double heightRegionP, heightRegionN;
    int numFoldedPMOS = 1, numFoldedNMOS = 1;

    // add CPP paramter
    double CPP = POLY_WIDTH + MIN_GAP_BET_GATE_POLY;
    double CPP_advanced= POLY_WIDTH_FINFET + MIN_GAP_BET_GATE_POLY_FINFET;

    // account for new cell height technology trend
    if (tech.featureSize == 10 * 1e-9)
    CPP_advanced = CPP_10nm;
    else if (tech.featureSize == 7 * 1e-9)
    CPP_advanced =  CPP_7nm;
    else if (tech.featureSize == 5 * 1e-9)
    CPP_advanced =  CPP_5nm;
    else if (tech.featureSize == 3 * 1e-9)
    CPP_advanced =  CPP_3nm;
    else if (tech.featureSize == 2 * 1e-9)
    CPP_advanced =  CPP_2nm;
    else if (tech.featureSize == 1 * 1e-9)
    CPP_advanced = CPP_1nm;
    else
    heightTransistorRegion *= 1;
    

    if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) { // Bulk

        
        if (ratio == 0) {   /* no PMOS */
            maxWidthPMOS = 0;
            maxWidthNMOS = heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize;
        } else if (ratio == 1) {    /* no NMOS */
            maxWidthPMOS = heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize;
            maxWidthNMOS = 0;
        } else {
            maxWidthPMOS = ratio * (heightTransistorRegion - MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize);
            maxWidthNMOS = maxWidthPMOS / ratio * (1 - ratio);
        }

        if (widthPMOS > 0) {
            if (widthPMOS <= maxWidthPMOS) { /* No folding */
                unitWidthRegionP = 2 * (CPP) * tech.featureSize;
                heightRegionP = widthPMOS;
            } else {    /* Folding */
                numFoldedPMOS = (int)(ceil(widthPMOS / maxWidthPMOS));
                unitWidthRegionP = (numFoldedPMOS + 1) * (CPP) * tech.featureSize;
                heightRegionP = maxWidthPMOS;
            }
        } else {
            unitWidthRegionP = 0;
            heightRegionP = 0;
        }

        if (widthNMOS > 0) {
            if (widthNMOS <= maxWidthNMOS) { /* No folding */
                unitWidthRegionN = 2 * (CPP) * tech.featureSize;
                heightRegionN = widthNMOS;
            } else {    /* Folding */
                numFoldedNMOS = (int)(ceil(widthNMOS / maxWidthNMOS));
                unitWidthRegionN = (numFoldedNMOS + 1) * (CPP) * tech.featureSize;
                heightRegionN = maxWidthNMOS;
            }
        } else {
            unitWidthRegionN = 0;
            heightRegionN = 0;
        }
        
    } else { //FinFET, GAA, or beyond

        /* to be deleted */
        /*
        if (ratio == 0) {   
            maxNumPFin = 0;
            maxNumNFin = (int)(floor((heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
        } else if (ratio == 1) {    
            maxNumPFin = (int)(floor((heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
            maxNumNFin = 0;
        } else {
            maxNumPFin = (int)(floor(ratio * (heightTransistorRegion - MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
            maxNumNFin = (int)(floor( (1 - ratio) * (heightTransistorRegion - MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
        }
        */

        // replaced with the following code
        if (tech.featureSize == 14 * 1e-9) { // adding more cases 
            maxNumPFin = maxNumNFin = tech.max_fin_num; // changed from 3 to 4
        } else if (tech.featureSize == 10 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
        } else if (tech.featureSize == 7 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
        } else if (tech.featureSize == 5 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
        } else if (tech.featureSize == 3 * 1e-9) {
            maxNumPFin =  maxNumNFin = tech.max_fin_num;
        } else if (tech.featureSize == 2 * 1e-9) {
            maxNumPSheet= maxNumNSheet = tech.max_sheet_num;
        } else if (tech.featureSize == 1 * 1e-9) {
            maxNumPSheet= maxNumNSheet = tech.max_sheet_num;
        } 

        double temp_P=2*maxNumPFin;
        double temp_N=2*maxNumNFin;
        double temp_P_NS=2*maxNumPSheet;
        double temp_N_NS=2*maxNumNSheet;

        int temp_N_ratio;
        int temp_P_ratio;
        int temp_N_NS_ratio;
        int temp_P_NS_ratio;


        if (ratio == 0) {   /* no PFinFET */
            maxNumPFin = 0;
            maxNumNFin = temp_N;
            maxNumPSheet = 0;
            maxNumNSheet= temp_N_NS;
        } else if (ratio == 1) {    /* no NFinFET */
            maxNumPFin = temp_P;
            maxNumNFin = 0;
            maxNumPSheet = temp_P_NS;
            maxNumNSheet= 0;
        } else {
            if (ratio>0.5){
            temp_N_ratio=int (temp_N* (1-ratio));
            temp_P_ratio= temp_N-temp_N_ratio;
            temp_N_NS_ratio=int (temp_N_NS* (1-ratio));
            temp_P_NS_ratio= temp_N_NS-temp_N_NS_ratio;
            }
            else{
            temp_P_ratio=int (temp_P* (ratio));
            temp_N_ratio= temp_P - temp_P_ratio;
            temp_P_NS_ratio=int (temp_P_NS * (ratio));
            temp_N_NS_ratio=temp_P_NS - temp_P_NS_ratio;
            }

            if (temp_P_ratio==0) {temp_P_ratio +=1; temp_N_ratio = 2*maxNumPFin-temp_P_ratio;}
            if (temp_N_ratio==0) {temp_N_ratio +=1; temp_P_ratio = 2*maxNumNFin-temp_N_ratio;}
            if (temp_P_NS_ratio==0) {temp_P_NS_ratio +=1; temp_N_NS_ratio = 2*maxNumPSheet-temp_P_NS_ratio;}
            if (temp_N_NS_ratio==0) {temp_N_NS_ratio +=1; temp_P_NS_ratio = 2*maxNumNSheet-temp_N_NS_ratio;}

            maxNumPFin = temp_P_ratio; //(ratio);//2*temp_P); // change from floor to int
            maxNumNFin = temp_N_ratio;//((1-ratio));// *2*temp_N);
            maxNumPSheet = temp_P_NS_ratio; //(ratio);// *2*temp_P_NS);
            maxNumNSheet = temp_N_NS_ratio; // ((1-ratio));// *2*temp_N_NS);
        }

        if (tech.featureSize<= 14*1e-9 &&tech.featureSize >= 3 * 1e-9){

        int NumPFin = (int)(ceil(widthPMOS/(tech.PitchFin)));
        
        if (NumPFin > 0) {
            if (NumPFin <= maxNumPFin) { /* No folding */
                unitWidthRegionP = 2 * (CPP_advanced) * tech.featureSize;
                heightRegionP = (NumPFin-1) * tech.PitchFin + 2 * tech.widthFin/2; 
            } else {    /* Folding */
                numFoldedPMOS = (int)(ceil(NumPFin / maxNumPFin));
                unitWidthRegionP = (numFoldedPMOS + 1) * (CPP_advanced) * tech.featureSize;
                heightRegionP = (maxNumPFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
            }
        } else {
            unitWidthRegionP = 0;
            heightRegionP = 0;
        }

        int NumNFin = (int)(ceil(widthNMOS/(tech.PitchFin)));

        if (NumNFin > 0) {
            if (NumNFin <= maxNumNFin) { /* No folding */
                unitWidthRegionN = 2 * (CPP_advanced) * tech.featureSize;
                heightRegionN = (NumNFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
            } else {    /* Folding */
                numFoldedNMOS = (int)(ceil(NumNFin / maxNumNFin));
                unitWidthRegionN = (numFoldedNMOS + 1) * (CPP_advanced) * tech.featureSize;
                heightRegionN = (maxNumNFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
            }
        } else {
            unitWidthRegionN = 0;
            heightRegionN = 0;
        }

        }

        else {

        int NumPSheet = (int)(ceil(widthPMOS));
        
        
        if (NumPSheet > 0) {
            if (NumPSheet <= maxNumPSheet) { /* No folding */
                unitWidthRegionP = 2 * (CPP_advanced) * tech.featureSize;
                heightRegionP = 0; // not needed 
            } else {    /* Folding */
                numFoldedPMOS = (int)(ceil(NumPSheet/ maxNumPSheet));
                unitWidthRegionP = (numFoldedPMOS + 1) * (CPP_advanced) * tech.featureSize;
                heightRegionP = 0; // not needed 
            }
        } else {
            unitWidthRegionP = 0;
            heightRegionP = 0;
        }

        int NumNSheet = (int)(ceil(widthNMOS));

        if (NumNSheet > 0) {
            if (NumNSheet <= maxNumNSheet) { /* No folding */
                unitWidthRegionN = 2 * (CPP_advanced) * tech.featureSize;
                heightRegionN = 0; // not needed 
            } else {    /* Folding */
                numFoldedNMOS = (int)(ceil(NumNSheet / maxNumNSheet));
                unitWidthRegionN = (numFoldedNMOS + 1) * (CPP_advanced) * tech.featureSize;
                heightRegionN = 0; // not needed 
            }
        } else {
            unitWidthRegionN = 0;
            heightRegionN = 0;
        }

        }

    }

    switch (gateType) {
    case INV:
        widthRegionP = unitWidthRegionP;
        widthRegionN = unitWidthRegionN;
        break;
    case NOR:
        if (numFoldedPMOS == 1 && numFoldedNMOS == 1) { // Need to subtract the source/drain sharing region
            if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) { // Bulk
                widthRegionP = unitWidthRegionP * numInput
                               - (numInput-1) * tech.featureSize * (CPP);
                widthRegionN = unitWidthRegionN * numInput
                               - (numInput-1) * tech.featureSize * (CPP);
            } else {
                widthRegionP = unitWidthRegionP * numInput
                               - (numInput-1) * tech.featureSize * (CPP_advanced);
                widthRegionN = unitWidthRegionN * numInput
                               - (numInput-1) * tech.featureSize * (CPP_advanced);
            }
        } else {    // If either PMOS or NMOS has folding, there is no source/drain sharing among different PMOS and NMOS devices.
            widthRegionP = unitWidthRegionP * numInput;
            widthRegionN = unitWidthRegionN * numInput;
        }
        break;
    case NAND:
        if (numFoldedPMOS == 1 && numFoldedNMOS == 1) { // Need to subtract the source/drain sharing region
            if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) { // Bulk
                widthRegionP = unitWidthRegionP * numInput
                               - (numInput-1) * tech.featureSize * (CPP);
                widthRegionN = unitWidthRegionN * numInput
                               - (numInput-1) * tech.featureSize * (CPP);
            } else {
                widthRegionP = unitWidthRegionP * numInput
                               - (numInput-1) * tech.featureSize * (CPP_advanced);
                widthRegionN = unitWidthRegionN * numInput
                               - (numInput-1) * tech.featureSize * (CPP_advanced);
            }
        } else {    // If either PMOS or NMOS has folding, there is no source/drain sharing among different PMOS and NMOS devices.
            widthRegionP = unitWidthRegionP * numInput;
            widthRegionN = unitWidthRegionN * numInput;
        }
        break;
    default:
        widthRegionN = widthRegionP = 0;
    }

    *width = MAX(widthRegionN, widthRegionP);
    *height = heightTransistorRegion;   // Fixed standard cell height

    return (*width)*(*height);

}

 
void CalculateGateCapacitance(
    int gateType, int numInput,
    double widthNMOS, double widthPMOS,
    double heightTransistorRegion, Technology tech,
    double *capInput, double *capOutput) {
    if (capInput){
        *(capInput) = CalculateGateCap(widthNMOS, tech) + CalculateGateCap(widthPMOS, tech);
    }
    if (capOutput){

        // replaced
        /*
        if (tech.featureSize <= 14 * 1e-9) {  // finfet
            widthNMOS *= tech.PitchFin/(2 * tech.featureSize);
            widthPMOS *= tech.PitchFin/(2 * tech.featureSize);
            heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_FINFET/MAX_TRANSISTOR_HEIGHT);
        }
        */

    if (tech.featureSize <= 14 * 1e-9) {  // finfet or GAA 

    if (tech.featureSize > 2 * 1e-9) {
        widthNMOS *= tech.PitchFin/(2 * tech.featureSize); // for GAA, number of nanosheets is calculated when divided by (2 * tech.featureSize)
        widthPMOS *= tech.PitchFin/(2 * tech.featureSize); // for GAA, number of nanosheets is calculated when divided by (2 * tech.featureSize) 
    }

    else if (tech.featureSize <= 2 * 1e-9) // GAA case
    {
    widthNMOS *= 1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
    widthPMOS *= 1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
    }

        heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_FINFET/MAX_TRANSISTOR_HEIGHT);

        switch (int(tech.featureSize * 1e+9)) {
        case 10: heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_10nm /MAX_TRANSISTOR_HEIGHT_FINFET);
        case 7: heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_7nm /MAX_TRANSISTOR_HEIGHT_FINFET);
        case 5: heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_5nm /MAX_TRANSISTOR_HEIGHT_FINFET);
        case 3: heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_3nm /MAX_TRANSISTOR_HEIGHT_FINFET);
        case 2: heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_2nm /MAX_TRANSISTOR_HEIGHT_FINFET);
        case 1: heightTransistorRegion *= (MAX_TRANSISTOR_HEIGHT_1nm /MAX_TRANSISTOR_HEIGHT_FINFET);
        default: heightTransistorRegion *= 1;
        break;

        }
    }

        double  ratio = widthPMOS / (widthPMOS + widthNMOS);
        double maxWidthPMOS = 0, maxWidthNMOS = 0;
        int maxNumPFin = 0, maxNumNFin = 0; /* Max numbers of fin for the specified cell height */
        double unitWidthDrainP = 0, unitWidthDrainN = 0;
        double unitWidthSourceP = 0, unitWidthSourceN = 0;
        double widthDrainP = 0, widthDrainN = 0;
        double heightDrainP = 0, heightDrainN = 0;
        int numFoldedPMOS = 1, numFoldedNMOS = 1;
        double widthDrainSidewallP = 0, widthDrainSidewallN = 0;
        
        // add GAA
        int NumPSheet;
        int NumNSheet;
        int maxNumSheet;

        if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) { // Bulk
            if (ratio == 0) {   /* no PMOS */
                maxWidthPMOS = 0;
                maxWidthNMOS = heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize;
            } else if (ratio == 1) {    /* no NMOS */
                maxWidthPMOS = heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize;
                maxWidthNMOS = 0;
            } else {
                maxWidthPMOS = ratio * (heightTransistorRegion - MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize);
                maxWidthNMOS = maxWidthPMOS / ratio * (1 - ratio);
            }

            if (widthPMOS > 0) {
                if (widthPMOS <= maxWidthPMOS) { /* No folding */
                    unitWidthDrainP = tech.featureSize * MIN_GAP_BET_GATE_POLY;
                    unitWidthSourceP = unitWidthDrainP;
                    heightDrainP = widthPMOS;
                } else {    /* Folding */
                    numFoldedPMOS = (int)(ceil(widthPMOS / maxWidthPMOS));
                    unitWidthDrainP = (int)ceil((double)(numFoldedPMOS+1)/2) * tech.featureSize * MIN_GAP_BET_GATE_POLY;    // Num of drain fingers >= num of source fingers
                    unitWidthSourceP = (int)floor((double)(numFoldedPMOS+1)/2) * tech.featureSize * MIN_GAP_BET_GATE_POLY;
                    heightDrainP = maxWidthPMOS;
                }
            } else {
                unitWidthDrainP = 0;
                unitWidthSourceP = 0;
                heightDrainP = 0;
            }
            if (widthNMOS > 0) {
                if (widthNMOS <= maxWidthNMOS) { /* No folding */
                    unitWidthDrainN = tech.featureSize * MIN_GAP_BET_GATE_POLY;
                    unitWidthSourceN = unitWidthDrainN;
                    heightDrainN = widthNMOS;
                } else {    /* Folding */
                    numFoldedNMOS = (int)(ceil(widthNMOS / maxWidthNMOS));
                    unitWidthDrainN = (int)ceil((double)(numFoldedNMOS+1)/2) * tech.featureSize * MIN_GAP_BET_GATE_POLY;
                    unitWidthSourceN = (int)floor((double)(numFoldedNMOS+1)/2) * tech.featureSize * MIN_GAP_BET_GATE_POLY;
                    heightDrainN = maxWidthNMOS;
                }
            } else {
                unitWidthDrainN = 0;
                unitWidthSourceN = 0;
                heightDrainN = 0;
            }

        } else { //FinFET & GAA

        /*
            if (ratio == 0) {   
                maxNumPFin = 0;
                maxNumNFin = (int)(floor((heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
            } else if (ratio == 1) {    /
                maxNumPFin = (int)(floor((heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
                maxNumNFin = 0;
            } else {
                maxNumPFin = (int)(floor(ratio * (heightTransistorRegion - MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
                maxNumNFin = (int)(floor( (1 - ratio) * (heightTransistorRegion - MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize) / tech.PitchFin)) + 1;
            }
        */

        double modified_POLY_WIDTH_FINFET= POLY_WIDTH_FINFET;
        double CPP_advanced = POLY_WIDTH_FINFET + MIN_GAP_BET_GATE_POLY_FINFET;
        double modified_MIN_GAP_BET_GATE_POLY_FINFET;

        // replaced with the following code
        if (tech.featureSize == 14 * 1e-9) { // adding more cases 
            maxNumPFin = maxNumNFin = tech.max_fin_num; // changed from 3 to 4
        } else if (tech.featureSize == 10 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
            modified_POLY_WIDTH_FINFET= POLY_WIDTH_10nm;
            CPP_advanced = CPP_10nm;
        } else if (tech.featureSize == 7 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
            modified_POLY_WIDTH_FINFET= POLY_WIDTH_7nm;
            CPP_advanced = CPP_7nm;
        } else if (tech.featureSize == 5 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
            modified_POLY_WIDTH_FINFET= POLY_WIDTH_5nm;
            CPP_advanced = CPP_5nm;
        } else if (tech.featureSize == 3 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
            modified_POLY_WIDTH_FINFET= POLY_WIDTH_3nm;
            CPP_advanced = CPP_3nm;
        } else if (tech.featureSize == 2 * 1e-9) {
            maxNumSheet = tech.max_sheet_num;
            modified_POLY_WIDTH_FINFET= POLY_WIDTH_2nm;
            CPP_advanced = CPP_2nm;
        } else if (tech.featureSize == 1 * 1e-9) {
            maxNumSheet = tech.max_sheet_num;
            modified_POLY_WIDTH_FINFET= POLY_WIDTH_1nm;
            CPP_advanced = CPP_1nm;
        } 

        modified_MIN_GAP_BET_GATE_POLY_FINFET= CPP_advanced-modified_POLY_WIDTH_FINFET; 

        if (tech.featureSize < 22 * 1e-9  && tech.featureSize >= 3 * 1e-9){
            int NumPFin = (int)(ceil(widthPMOS/(tech.PitchFin)));
            if (NumPFin > 0) {
                if (NumPFin <= maxNumPFin) { /* No folding */
                    unitWidthDrainP = tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceP = unitWidthDrainP;
                    heightDrainP = tech.widthFin;
                } else {    /* Folding */
                    numFoldedPMOS = (int)(ceil(NumPFin / maxNumPFin));
                    unitWidthDrainP = (int)ceil((double)(numFoldedPMOS+1)/2) * tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceP = (int)floor((double)(numFoldedPMOS+1)/2) * tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    heightDrainP = tech.widthFin; // modified from heightDrainP = (maxNumPFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
                }
            } else {
                unitWidthDrainP = 0;
                unitWidthSourceP = 0;
                heightDrainP = 0;
            }

            int NumNFin = (int)(ceil(widthNMOS/(tech.PitchFin)));

            if (NumNFin > 0) {
                if (NumNFin <= maxNumNFin) { /* No folding */
                    unitWidthDrainN = tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceN = unitWidthDrainN;
                    heightDrainN = tech.widthFin;
                } else {    /* Folding */
                    numFoldedNMOS = (int)(ceil(NumNFin / maxNumNFin));
                    unitWidthDrainN = (int)ceil((double)(numFoldedNMOS+1)/2) * tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceN = (int)floor((double)(numFoldedNMOS+1)/2) * tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    heightDrainN = tech.widthFin; // modified from heightDrainN = (maxNumNFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
                }
            } else {
                unitWidthDrainN = 0;
                unitWidthSourceN = 0;
                heightDrainN = 0;
            }
        }
        else {
            int NumPSheet = (int)(ceil(widthPMOS));

            if (NumPSheet > 0) {
                if (NumPSheet <= maxNumSheet) { /* No folding */
                    unitWidthDrainP = tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceP = unitWidthDrainP;
                    heightDrainP = tech.widthFin;
                } else {    /* Folding */
                    numFoldedPMOS = (int)(ceil(NumPSheet/ maxNumSheet));
                    unitWidthDrainP = (int)ceil((double)(numFoldedPMOS+1)/2) * tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceP = (int)floor((double)(numFoldedPMOS+1)/2) * tech.featureSize * modified_MIN_GAP_BET_GATE_POLY_FINFET;
                    heightDrainP = tech.widthFin; // modified from heightDrainP = (maxNumPFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
                }
            } else {
                unitWidthDrainP = 0;
                unitWidthSourceP = 0;
                heightDrainP = 0;
            }

            int NumNSheet  = (int)(ceil(widthNMOS));

            if (NumNSheet > 0) {
                if (NumNSheet <= maxNumSheet) { /* No folding */
                    unitWidthDrainN = tech.featureSize * MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceN = unitWidthDrainN;
                    heightDrainN = tech.widthFin;
                } else {    /* Folding */
                    numFoldedNMOS = (int)(ceil(NumNSheet / maxNumSheet));
                    unitWidthDrainN = (int)ceil((double)(numFoldedNMOS+1)/2) * tech.featureSize * MIN_GAP_BET_GATE_POLY_FINFET;
                    unitWidthSourceN = (int)floor((double)(numFoldedNMOS+1)/2) * tech.featureSize * MIN_GAP_BET_GATE_POLY_FINFET;
                    heightDrainN = tech.widthFin; // modified from heightDrainN = (maxNumNFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
                }
            } else {
                unitWidthDrainN = 0;
                unitWidthSourceN = 0;
                heightDrainN = 0;
            }

        }
        }

        switch (gateType) {
        case INV:
            if (widthPMOS > 0) {
                widthDrainP = unitWidthDrainP;
                // Folding=1: both drain and source has 1 side; folding=2: drain has 2 sides and source has 0 side... etc
                widthDrainSidewallP = widthDrainP * 2 + heightDrainP * (1+(numFoldedPMOS+1)%2);
            }
            if (widthNMOS > 0) {
                widthDrainN = unitWidthDrainN;
                widthDrainSidewallN = widthDrainN * 2 + heightDrainN * (1+(numFoldedPMOS+1)%2);
            }
            break;
        case NOR:
            // If either PMOS or NMOS has folding, there is no source/drain sharing among different PMOS and NMOS devices
            if (numFoldedPMOS == 1 && numFoldedNMOS == 1) {
                if (widthPMOS > 0) {    // No need to consider the source capacitance in series PMOS because here source and drain shares
                    widthDrainP = unitWidthDrainP * numInput;
                    widthDrainSidewallP = widthDrainP * 2 + heightDrainP;
                }
                if (widthNMOS > 0) {    // The number of NMOS drains is not equal to the number of NMOS in parallel because drain can share
                    widthDrainN = unitWidthDrainN * (int)floor((double)(numInput+1)/2); // Use floor: assume num of source regions >= num of drain regions
                    widthDrainSidewallN = widthDrainN * 2 + heightDrainN * (1-(numInput+1)%2);
                }
            } else {
                if (widthPMOS > 0) {    // Need to consider the source capacitance in series PMOS (excluding the top one)
                    widthDrainP = unitWidthDrainP * numInput + (numInput-1) * unitWidthSourceP;
                    widthDrainSidewallP = widthDrainP * 2
                                          + heightDrainP * (1+(numFoldedPMOS+1)%2) * numInput           // Drain sidewalls
                                          + heightDrainP * (1-(numFoldedPMOS+1)%2) * (numInput-1);  // Source sidewalls
                }
                if (widthNMOS > 0) {    // Drain cannot share between different NMOS
                    widthDrainN = unitWidthDrainN * numInput;
                    widthDrainSidewallN = widthDrainN * 2 + heightDrainN * (1+(numFoldedNMOS+1)%2) * numInput;
                }
            }
            break;
        case NAND:
            // If either PMOS or NMOS has folding, there is no source/drain sharing among different PMOS and NMOS devices
            if (numFoldedPMOS == 1 && numFoldedNMOS == 1) {
                if (widthPMOS > 0) {  // The number of PMOS drains is not equal to the number of PMOS in parallel because drain can share
                    widthDrainP = unitWidthDrainP * (int)floor((double)(numInput+1)/2); // Use floor: assume num of source regions >= num of drain regions
                    widthDrainSidewallP = widthDrainP * 2 + heightDrainP * (1-(numInput+1)%2);
                }
                if (widthNMOS > 0) {  // No need to consider the source capacitance in series NMOS because here source and drain shares
                    widthDrainN = unitWidthDrainN * numInput;
                    widthDrainSidewallN = widthDrainN * 2 + heightDrainN;
                }
            } else {
                if (widthPMOS > 0) {  // Drain cannot share between different PMOS
                    widthDrainP = unitWidthDrainP * numInput;
                    widthDrainSidewallP = widthDrainP * 2 + heightDrainP * (1+(numFoldedPMOS+1)%2) * numInput;
                }
                if (widthNMOS > 0) {  // Need to consider the source capacitance in series NMOS (excluding the bottom one)
                    widthDrainN = unitWidthDrainN * numInput + (numInput-1) * unitWidthSourceN;
                    widthDrainSidewallN = widthDrainN * 2
                                          + heightDrainN * (1+(numFoldedNMOS+1)%2) * numInput         // Drain sidewalls
                                          + heightDrainN * (1-(numFoldedNMOS+1)%2) * (numInput-1);    // Source sidewalls
                }
            }
            break;
        default:
            widthDrainN = widthDrainP = widthDrainSidewallP = widthDrainSidewallN = 0;
        }
        /* Junction capacitance */
        double capDrainBottomN = widthDrainN * heightDrainN * tech.capJunction;
        double capDrainBottomP = widthDrainP * heightDrainP * tech.capJunction;

        /* Sidewall capacitance */  // FIXME
        double capDrainSidewallN, capDrainSidewallP;
        capDrainSidewallP = widthDrainSidewallP * tech.capSidewall;
        capDrainSidewallN = widthDrainSidewallN * tech.capSidewall;

        /* Drain to channel capacitance */  // FIXME
        double capDrainToChannelN = numFoldedNMOS * heightDrainN * tech.capDrainToChannel;
        double capDrainToChannelP = numFoldedPMOS * heightDrainP * tech.capDrainToChannel;
   
        *(capOutput) = capDrainBottomN + capDrainBottomP + capDrainSidewallN + capDrainSidewallP + capDrainToChannelN + capDrainToChannelP;
    }
}

double CalculateDrainCap(
    double width, int type,
    double heightTransistorRegion, Technology tech) {
    double drainCap = 0;
    if (type == NMOS)
        CalculateGateCapacitance(INV, 1, width, 0, heightTransistorRegion, tech, NULL, &drainCap);
    else //PMOS
        CalculateGateCapacitance(INV, 1, 0, width, heightTransistorRegion, tech, NULL, &drainCap);
    return drainCap;
}

double CalculateGateLeakage(
    int gateType, int numInput,
    double widthNMOS, double widthPMOS,
    double temperature, Technology tech) {
    int tempIndex = (int)temperature - 300;
    if ((tempIndex > 100) || (tempIndex < 0)) {
        cout<<"Error: Temperature is out of range"<<endl;
        exit(-1);
    }
    double *leakN = tech.currentOffNmos;
    double *leakP = tech.currentOffPmos;
    double leakageN, leakageP;
    
    double widthNMOSEff, widthPMOSEff;

    if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) {
        widthNMOSEff = widthNMOS;
        widthPMOSEff = widthPMOS;
    } else if (tech.featureSize < 22 * 1e-9  && tech.featureSize >= 3 * 1e-9 ) {// up to FinFET 7 nm
        widthNMOS *= tech.PitchFin/(2 * tech.featureSize);
        widthPMOS *= tech.PitchFin/(2 * tech.featureSize);
        widthNMOSEff = ceil(widthNMOS/tech.PitchFin)*(tech.effective_width);
        widthPMOSEff = ceil(widthPMOS/tech.PitchFin)*(tech.effective_width);
    }
    else { // GAA
        widthNMOS*=1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
        widthPMOS*=1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
        widthNMOSEff = ceil(widthNMOS)*tech.effective_width;
        widthPMOSEff = ceil(widthPMOS)*tech.effective_width;
    }
    
    switch (gateType) {
    case INV:
        leakageN = widthNMOSEff * leakN[tempIndex];
        leakageP = widthPMOSEff * leakP[tempIndex];
        return (leakageN + leakageP)/2;
    case NOR:
        leakageN = widthNMOSEff * leakN[tempIndex] * numInput;
        if (numInput == 2) {
            return AVG_RATIO_LEAK_2INPUT_NOR * leakageN;
        }
        else {
            return AVG_RATIO_LEAK_3INPUT_NOR * leakageN;
        }
    case NAND:
        leakageP = widthPMOSEff * leakP[tempIndex] * numInput;
        if (numInput == 2) {
            return AVG_RATIO_LEAK_2INPUT_NAND * leakageP;
        }
        else {
            return AVG_RATIO_LEAK_3INPUT_NAND * leakageP;
        }
    default:
        return 0.0;
    }
}

double CalculateOnResistance(double width, int type, double temperature, Technology tech) {
    double r;
    int tempIndex = (int)temperature - 300;
    if ((tempIndex > 100) || (tempIndex < 0)) {
        cout<<"Error: Temperature is out of range"<<endl;
        exit(-1);
    }

    double widthEff = 0;

    if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) {
        widthEff = width;
    } else if (tech.featureSize < 22 * 1e-9  && tech.featureSize >=  3 * 1e-9 ) {// up to FinFET 7 nm
        width *= tech.PitchFin/(2 * tech.featureSize);
        widthEff = ceil(width/tech.PitchFin)*(tech.effective_width);
    } else { // 5 nm and beyond case
        width *=1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
        widthEff = ceil(width)*tech.effective_width;
    }

    if (type == NMOS)
        r = tech.effectiveResistanceMultiplier * tech.vdd / (tech.currentOnNmos[tempIndex] * widthEff);
    else
        r = tech.effectiveResistanceMultiplier * tech.vdd / (tech.currentOnPmos[tempIndex] * widthEff);
    
    return r;
}

double CalculateTransconductance(double width, int type, Technology tech) { // not used 
    double gm;
    double widthEff = 0;
    if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) {
        widthEff = width;
    } else {
        width *= tech.PitchFin/(2 * tech.featureSize);
        widthEff = ceil(width/tech.PitchFin)*(2*tech.heightFin + tech.widthFin);
    }
    if (type == NMOS) {
        gm = (2*tech.current_gmNmos)*widthEff/(0.7*tech.vdd-tech.vth);
    } else {//type==PMOS
        gm = (2*tech.current_gmPmos)*widthEff/(0.7*tech.vdd-tech.vth);
    }
    return gm;
}

double horowitz(double tr, double beta, double rampInput, double *rampOutput) {
    double alpha = 1 / rampInput / tr;
    double vs = 0.5;    /* Normalized switching voltage */
    beta = 0.5; // Just use beta=0.5 as CACTI because we do not want to consider gm anymore
    // Need to delete this input argument in the future
    double result = tr * sqrt(log(vs) * log(vs) + 2 * alpha * beta * (1 - vs));
    if (rampOutput)
        *rampOutput = (1 - vs) / result;
    return result;
}

double CalculatePassGateArea(   // Calculate layout area, height and width of pass gate given the number of folding on the pass gate width
    // This function is for pass gate where the cell height can change. For normal standard cells, use CalculateGateArea() where the cell height is fixed
    double widthNMOS, double widthPMOS, Technology tech, int numFold, double *height, double *width) {

        // define constants again 
        double modified_MIN_GAP_BET_P_AND_N_DIFFS;
        double CPP_advanced = POLY_WIDTH_FINFET + MIN_GAP_BET_GATE_POLY_FINFET;
        double outer_height_region_advanced; 

        // replaced with the following code
        if (tech.featureSize == 14 * 1e-9) {
            modified_MIN_GAP_BET_P_AND_N_DIFFS=MIN_GAP_BET_P_AND_N_DIFFS_14nm;
            outer_height_region_advanced = OUTER_HEIGHT_REGION_14nm;
        } else if (tech.featureSize == 10 * 1e-9) {
            modified_MIN_GAP_BET_P_AND_N_DIFFS=MIN_GAP_BET_P_AND_N_DIFFS_10nm;
            outer_height_region_advanced = OUTER_HEIGHT_REGION_10nm;
            CPP_advanced = CPP_10nm;
        } else if (tech.featureSize == 7 * 1e-9) {
            modified_MIN_GAP_BET_P_AND_N_DIFFS=MIN_GAP_BET_P_AND_N_DIFFS_7nm;
            outer_height_region_advanced = OUTER_HEIGHT_REGION_7nm;
            CPP_advanced = CPP_7nm;
        } else if (tech.featureSize == 5 * 1e-9) {
            modified_MIN_GAP_BET_P_AND_N_DIFFS=MIN_GAP_BET_P_AND_N_DIFFS_5nm;
            outer_height_region_advanced = OUTER_HEIGHT_REGION_5nm;
            CPP_advanced = CPP_5nm;
        } else if (tech.featureSize == 3 * 1e-9) {
            CPP_advanced = CPP_3nm;
            modified_MIN_GAP_BET_P_AND_N_DIFFS=MIN_GAP_BET_P_AND_N_DIFFS_3nm;
            outer_height_region_advanced = OUTER_HEIGHT_REGION_3nm;
        } else if (tech.featureSize == 2 * 1e-9) {
            modified_MIN_GAP_BET_P_AND_N_DIFFS=MIN_GAP_BET_P_AND_N_DIFFS_2nm;
            outer_height_region_advanced = OUTER_HEIGHT_REGION_2nm;
            CPP_advanced = CPP_2nm;
        } else if (tech.featureSize == 1 * 1e-9) {
            modified_MIN_GAP_BET_P_AND_N_DIFFS=MIN_GAP_BET_P_AND_N_DIFFS_1nm;
            outer_height_region_advanced = OUTER_HEIGHT_REGION_1nm;
            CPP_advanced = CPP_1nm;
        } 

    

    if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) { // Bulk
        *width = (numFold + 1) * (POLY_WIDTH + MIN_GAP_BET_GATE_POLY) * tech.featureSize;   // No folding means numFold=1
        *height = widthPMOS/numFold + widthNMOS/numFold + MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize
                  + (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize;

    } else if (tech.featureSize >= 3 * 1e-9 ) {// up to FinFET 3 nm

    // FinFET

        *width = (numFold + 1) * (CPP_advanced) * tech.featureSize; // No folding means numFold=1

        /* seems to be the right code 
        int totalNumPFin = (int)(ceil(widthPMOS/tech.effective_width));
        int totalNumNFin = (int)(ceil(widthNMOS/tech.effective_width));
        */
        int totalNumPFin = (int)(ceil(widthPMOS/(2 * tech.featureSize)));
        int totalNumNFin = (int)(ceil(widthNMOS/(2 * tech.featureSize)));
        int NumPFin = (int)(ceil((double)totalNumPFin/numFold));
        int NumNFin = (int)(ceil((double)totalNumNFin/numFold));
        double heightRegionP = (NumPFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
        double heightRegionN = (NumNFin-1) * tech.PitchFin + 2 * tech.widthFin/2;
        *height = heightRegionP + heightRegionN + modified_MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize
                  + outer_height_region_advanced * tech.featureSize;
    }
    else{ // GAA // Assume using the conventional way for GAA

        double maxNumNSheet=tech.max_sheet_num; // intentional double conversion
        double maxNumPSheet=tech.max_sheet_num;

        int NumPSheet = (int)(ceil(widthPMOS/( 2 * tech.featureSize)));
        int NumNSheet = (int)(ceil(widthNMOS/( 2 * tech.featureSize)));
        *width = (numFold + 1) * (CPP_advanced) * tech.featureSize; // No folding means numFold=1
        widthNMOS *= 1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
        widthPMOS *= 1/(2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA;
        int totalNumPSheet = (int)(ceil(widthPMOS));
        int totalNumNSheet = (int)(ceil(widthNMOS));
        int NumPSheetFins = (int)(ceil((double)totalNumPSheet/numFold/(maxNumPSheet/tech.max_fin_per_GAA)));
        int NumNSheetFins = (int)(ceil((double)totalNumNSheet/numFold/(maxNumNSheet/tech.max_fin_per_GAA)));
        double heightRegionP = ( NumPSheetFins -1) * tech.PitchFin + 2 * tech.widthFin/2;
        double heightRegionN = (NumNSheetFins-1) * tech.PitchFin + 2 * tech.widthFin/2;
        *height = heightRegionP + heightRegionN + modified_MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize
                  + outer_height_region_advanced * tech.featureSize;

    }
    
    return (*width)*(*height);
}

double NonlinearResistance(double R, double NL, double Vw, double Vr, double V) {   // Nonlinearity is the current ratio between Vw and V, and R means the resistance at Vr
    double R_NL = R * V/Vr * pow(NL, (Vr-V)/(Vw/2));
    return R_NL;
}

/* enlarge min size transister to max with same layout area */
void EnlargeSize(double *widthNMOS, double *widthPMOS, double heightTransistorRegion, Technology tech) {    
    double  ratio = *widthPMOS / (*widthPMOS + *widthNMOS);
    double maxWidthPMOS, maxWidthNMOS;
    int maxNumPFin, maxNumNFin; /* Max number of fins for the specified cell height */
    int maxNumPSheet, maxNumNSheet;    /* Max number of sheets for the specified cell height */ 
    double unitWidthRegionP, unitWidthRegionN;
    double widthRegionP, widthRegionN;
    double heightRegionP, heightRegionN;
    int numFoldedPMOS = 1, numFoldedNMOS = 1;

    if (tech.featureSize >= 22 * 1e-9 || tech.transistorType != conventional) { // Bulk
        if (ratio == 0) {   /* no PMOS */
            maxWidthPMOS = 0;
            maxWidthNMOS = heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize;
        } else if (ratio == 1) {    /* no NMOS */
            maxWidthPMOS = heightTransistorRegion - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize;
            maxWidthNMOS = 0;
        } else {
            maxWidthPMOS = ratio * (heightTransistorRegion - MIN_GAP_BET_P_AND_N_DIFFS * tech.featureSize - (MIN_POLY_EXT_DIFF + MIN_GAP_BET_FIELD_POLY/2) * 2 * tech.featureSize);
            maxWidthNMOS = maxWidthPMOS / ratio * (1 - ratio);
        }
        if (ratio == 0 && *widthNMOS < maxWidthNMOS){
            *widthNMOS = maxWidthNMOS;
        } else if (ratio == 1 && *widthPMOS < maxWidthPMOS){
            *widthPMOS = maxWidthPMOS;
        } else if (*widthPMOS > 0 && *widthPMOS < maxWidthPMOS && *widthNMOS > 0 && *widthNMOS < maxWidthNMOS){
            *widthPMOS = maxWidthPMOS;
            *widthNMOS = maxWidthNMOS;
        }
    } else if (tech.featureSize < 22 * 1e-9  && tech.featureSize >=3  * 1e-9) { //FinFET
        if (tech.featureSize == 14 * 1e-9) { // adding more cases 
            maxNumPFin = maxNumNFin = tech.max_fin_num; // changed from 3 to 4
        } else if (tech.featureSize == 10 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
        } else if (tech.featureSize == 7 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
        } else if (tech.featureSize == 5 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
        } else if (tech.featureSize == 3 * 1e-9) {
            maxNumPFin = maxNumNFin = tech.max_fin_num;
        } 
    int NumPFin = (int)(ceil(*widthPMOS/(2 * tech.featureSize)));
    int NumNFin = (int)(ceil(*widthNMOS/(2 * tech.featureSize)));
        
        if (ratio == 0 && NumNFin <= maxNumNFin){
            NumNFin = maxNumNFin;
            *widthNMOS = (double) NumNFin * 2 * tech.featureSize;
        } else if (ratio == 1 && NumPFin <= maxNumPFin){
            NumPFin = maxNumPFin;
            *widthPMOS = (double) NumPFin * 2 * tech.featureSize;
        } else if (NumPFin > 0 && NumPFin <= maxNumPFin && NumNFin > 0 && NumNFin <= maxNumNFin){
            if(ratio >= 0.5){       //pmos lager
                NumPFin = maxNumPFin;
                NumNFin = (ceil) (maxNumPFin /ratio * (1 - ratio));
     
                /* possible implementation
                NumNFin = (int) ((2*maxNumPFin) * (1 - ratio));
                NumPFin = (2*maxNumPFin) - NumNFin;
                if (NumNFin==0) {
                NumNFin =1;
                NumPFin = (2*maxNumPFin) - NumNFin;
                }
                */
            }else{
                NumNFin = maxNumNFin;
                NumPFin = (ceil) (maxNumNFin /(1-ratio) * (ratio));

                /* possible implementation
                NumPFin = (int) ((2*maxNumPFin) * (ratio));
                NumNFin = (2*maxNumNFin) - NumPFin;
                if (NumPFin==0) {
                NumPFin =1;
                NumNFin = (2*maxNumNFin) - NumPFin;
                }
                */
            }
            *widthNMOS = (double) NumNFin * 2 * tech.featureSize;           
            *widthPMOS = (double) NumPFin * 2 * tech.featureSize;
        }
    } else { //GAA -> extension to CFET may be necessary
    // needs to check if enlargement for GAA or other device structures is reasonable

    maxNumNSheet=tech.max_sheet_num;//tech.max_sheet_num;
    maxNumPSheet=tech.max_sheet_num;//tech.max_sheet_num;
    /* seems to be the right code 
    int NumPSheet = (int)(ceil(*widthPMOS/(tech.effective_width)));
    int NumNSheet = (int)(ceil(*widthNMOS/(tech.effective_width)));
    */

    int NumPSheet = (int)(ceil(*widthPMOS/( 2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA));
    int NumNSheet = (int)(ceil(*widthNMOS/( 2 * tech.featureSize)*tech.max_sheet_num/tech.max_fin_per_GAA));

        if (ratio == 0 && NumNSheet <= maxNumNSheet){
            NumNSheet = maxNumNSheet;
            *widthNMOS = (double) NumNSheet * 2 * tech.featureSize;
        } else if (ratio == 1 && NumPSheet <= maxNumPSheet){
            NumPSheet = maxNumPSheet;
            *widthPMOS = (double) NumPSheet * 2 * tech.featureSize;
        } else if (NumPSheet > 0 && NumPSheet <= maxNumPSheet && NumNSheet > 0 && NumNSheet <= maxNumNSheet){
            if(ratio >= 0.5){       //pmos lager
                NumPSheet = maxNumPSheet;
                NumNSheet = (ceil) (maxNumPSheet /ratio * (1 - ratio));

                /* possible implementation 
                NumNSheet = (int) ((2*maxNumPSheet) * (1 - ratio));
                NumPSheet = (2*maxNumPSheet) - NumNSheet;
                if (NumNSheet==0) {
                NumNSheet =1;
                NumPSheet = (2*maxNumPSheet) - NumNSheet;
                }
                */ 
            }else{
                NumNSheet = maxNumNSheet;
                NumPSheet = (ceil) (maxNumNSheet /(1-ratio) * (ratio));

                /*
                NumPSheet = (int) ((2*maxNumPSheet) * (ratio));
                NumNSheet = (2*maxNumNSheet) - NumPSheet;
                if (NumPSheet==0) {
                NumPSheet =1;
                NumNSheet = (2*maxNumNSheet) - NumPSheet;
                }
                */
            }
            *widthNMOS = (double) NumNSheet * 2 * tech.featureSize;         
            *widthPMOS = (double) NumPSheet * 2 * tech.featureSize;
        }
    }
}
