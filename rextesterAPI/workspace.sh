#!/bin/bash
if [ -d "./virtualenv" ]; then
  echo "Virtualenv present, continue..."
else
  echo "Creating virtualenv..."
  virtualenv virtualenv
  echo "Linking src"
  ln -s src virtualenv/src
fi

cd virtualenv
source bin/activate
cd worktree/src
