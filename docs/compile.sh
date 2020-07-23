#!/bin/bash

while inotifywait -e modify $(pwd)/docs.texi; do make pdf; done
