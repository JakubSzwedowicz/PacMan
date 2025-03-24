#!/usr/bin/env bash

python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt

# On mac
#brew install graphviz

# Verify if it works...
hpp2plantuml -i "src/**/*.h" -o "Docs/output.puml"

# You can also extract dependencies from methods with -d
#hpp2plantuml -i "src/**/*.h" -d -o "Docs/output.puml"