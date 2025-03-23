#!/usr/bin/env bash

python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt

# Verify if it works...
hpp2plantuml -i "src/**/*.h" -o "Docs/output.puml"