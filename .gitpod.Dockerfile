FROM gitpod/workspace-full

USER gitpod

USER root
# Install util tools.
RUN apt-get update \
 && apt-get install -y \
  apt-utils \
  sudo \
  git \
  less \
  wget

RUN sudo mkdir /opt/q && sudo chown gitpod /opt/q && cd /opt/q && brew install librdkafka

RUN mkdir -p /workspace/data && sudo chown -R gitpod:gitpod /workspace/data

RUN mkdir /home/gitpod/.conda
# Install conda
RUN wget --quiet https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh && \
    /bin/bash ~/miniconda.sh -b -p /opt/conda && \
    rm ~/miniconda.sh && \
    ln -s /opt/conda/etc/profile.d/conda.sh /etc/profile.d/conda.sh && \
    echo ". /opt/conda/etc/profile.d/conda.sh" >> ~/.bashrc && \
    echo "conda activate base" >> ~/.bashrc

ENV KAFKA_ROOT=/home/linuxbrew/.linuxbrew/Cellar/librdkafka/1.3.0/
