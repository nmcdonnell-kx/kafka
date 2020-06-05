#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "osx" ]; then # use homebrew version
  brew update
  brew install librdkafka
  echo "brew install finished"
else 
  wget -qO - http://packages.confluent.io/deb/3.3/archive.key | sudo apt-key add - 
  sudo add-apt-repository "deb http://packages.confluent.io/deb/3.3 stable main"
  sudo apt-get update -qq # Update quietly.
  sudo apt-get install -y gcc-multilib librdkafka-dev librdkafka1
fi

