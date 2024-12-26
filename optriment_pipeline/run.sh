#!/bin/bash

# Unset TMUX variable to prevent nested tmux sessions
unset TMUX

# Define the tmux session name
SESSION_NAME="my_tmux_session"

# Start a new tmux session in the background (without attaching to it)
tmux new-session -d -s $SESSION_NAME

# Create 4 panes
tmux split-window -h -t $SESSION_NAME:0.0
tmux split-window -v -t $SESSION_NAME:0.1
tmux split-window -v -t $SESSION_NAME:0.2

# Start the Kafka server
tmux send-keys -t $SESSION_NAME:0.1 'cd /opt/kafka && ./bin/zookeeper-server-start.sh ./config/zookeeper.properties' C-m
tmux send-keys -t $SESSION_NAME:0.2 'cd /opt/kafka && ./bin/kafka-server-start.sh ./config/server.properties' C-m

# Start the Kafka listener
tmux send-keys -t $SESSION_NAME:0.3 'sleep 5; python3 kafka_listener.py' C-m

tmux send-keys -t $SESSION_NAME:0.0 'echo "Build & run here."' C-m

# Attach to the tmux session
tmux select-pane -t $SESSION_NAME:0.0
tmux attach -t $SESSION_NAME
