# install dependencies
#Should match Config.py
echo Initializing...

#Create directories to store persistent data
mkdir -p /workspace/conda
mkdir -p /workspace/data

#Create a new env called arcw
conda create --prefix /workspace/conda/arcw python=3.6 &&
echo "conda activate /workspace/conda/arcw" >> ~/.bashrc &&
export PATH=/workspace/conda/arcw/bin:$PATH &&
source ~/.bashrc
export SHELL=/bin/bash

conda install -c kx embedPy

echo Done...

