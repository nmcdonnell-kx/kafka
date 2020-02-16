FROM gitpod/workspace-full

USER gitpod

# Install custom tools, runtime, etc. using apt-get
# For example, the command below would install "bastet" - a command line tetris clone:
#
# RUN sudo apt-get -q update && #     sudo apt-get install -yq bastet && #     sudo rm -rf /var/lib/apt/lists/*
#
# More information: https://www.gitpod.io/docs/config-docker/

RUN sudo mkdir /opt/q && sudo chown gitpod /opt/q && cd /opt/q && wget https://github.com/sshanks-kx/kafka/raw/gitpod/q.zip && brew install librdkafka

ENV QHOME=/opt/q PATH=$PATH:/opt/q/l32/
