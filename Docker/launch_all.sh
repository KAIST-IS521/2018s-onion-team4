#!/bin/sh
tmux new-window -c $PWD './launch.sh 1'
sleep 1 # wait until node 1 create
tmux new-window -c $PWD './launch.sh 2'
tmux new-window -c $PWD './launch.sh 3'
tmux new-window -c $PWD './launch.sh 4'
tmux new-window -c $PWD './launch.sh 5'
