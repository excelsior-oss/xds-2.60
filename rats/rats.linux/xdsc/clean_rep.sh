#!/bin/sh

if [ -d reports ]; then
  rm -rf reports 1>/dev/null 2>&1
fi

mkdir reports
