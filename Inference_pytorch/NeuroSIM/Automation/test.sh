#!/usr/bin/env bash
for b in 0 1 2 
do
pushd /home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch/NeuroSIM;
make
let bb=b+1;
mv Param.cpp Param.txt;
sed -i 's/'tech_tech=$b'/'tech_tech=$bb'/' Param.txt;
mv Param.txt Param.cpp;
pushd /home/junmo/DNN_NeuroSim_V1.3_modified/Inference_pytorch;
python inference.py;
done;
