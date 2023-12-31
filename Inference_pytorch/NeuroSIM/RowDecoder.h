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
*   Pai-Yu Chen	    Email: pchen72 at asu dot edu 
*                    
*   Xiaochen Peng   Email: xpeng15 at asu dot edu
********************************************************************************/

#ifndef ROWDECODER_H_
#define ROWDECODER_H_

#include "typedef.h"
#include "InputParameter.h"
#include "Technology.h"
#include "MemCell.h"
#include "FunctionUnit.h"

class RowDecoder: public FunctionUnit {
public:
	RowDecoder(const InputParameter& _inputParameter, const Technology& _tech, const MemCell& _cell);
	virtual ~RowDecoder() {}
	const InputParameter& inputParameter;
	const Technology& tech;
	const MemCell& cell;

	/* Functions */
	void PrintProperty(const char* str);
	void SaveOutput(const char* str);
	void Initialize(DecoderMode _mode, int _numAddrRow, bool _MUX, bool _parallel);
	void CalculateArea(double _newHeight, double _newWidth, AreaModify _option);

	// 1.4 update: update the latency function
	void CalculateLatency(double _rampInput, double _capLoad1, double _capLoad2, double resLoad, double colnum, double numRead, double numWrite);
	void CalculatePower(double numRead, double numWrite);

	/* Properties */
	bool initialized;	/* Initialization flag */
	int numAddrRow;		/* Number of rows */
	double capLoad1;	// REGULAR: general capLoad, MUX: the NMOS Tg gates
	double capLoad2;	// MUX: the PMOS Tg gates
	int numColNor, numNorPerCol, numColNand, numNandPerCol, numColInv, numInvPerCol;
	int numRowNor, numNorPerRow, numRowNand, numNandPerRow, numRowInv, numInvPerRow;

	double widthInvN, widthInvP, widthNandN, widthNandP, widthNorN, widthNorP, widthDriverInvN, widthDriverInvP;
	int numInv, numNand, numNor, numMetalConnection;
	double capInvInput, capInvOutput, capNandInput, capNandOutput, capNorInput, capNorOutput, capDriverInvInput, capDriverInvOutput;
	double rampInput, rampOutput;
	DecoderMode mode;	// ROW or COLUMN mode
	bool MUX;	// MUX mode
	bool parallel;    // for mux decoder of parallel reading mode

	// 1.4 update: final driver width 
	double widthDriverInvN_final, widthDriverInvP_final;
	double capDriverInvInput_final, capDriverInvOutput_final;
};

#endif /* ROWDECODER_H_ */
