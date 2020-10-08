#!/bin/bash

mkdir cbuild

if [ "$TRAVIS_OS_NAME" == "osx" ]; then
  brew install librdkafka
  cp -r /usr/local/opt/librdkafka/lib ./cbuild
  cp -r /usr/local/opt/librdkafka/include ./cbuild
elif [ "$TRAVIS_OS_NAME" == "linux" ]; then
  git clone https://github.com/edenhill/librdkafka
  cd librdkafka
  ./configure --prefix=../cbuild --disable-sasl --disable-lz4 --disable-ssl --mbits=64
  make
  sudo make install
  cd ..
else
  echo "$TRAVIS_OS_NAME is currently not supported"  
fi
