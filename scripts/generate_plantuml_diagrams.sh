#!/usr/bin/env bash
# This script generates PlantUML diagrams from the source code.


PLANTUML_JAR=$(find . -iname 'plantuml-*' | head)
if [[ -z "$PLANTUML_JAR" ]]; then
  echo "PlantUML jar file not found."
  exit 1
fi
echo "Using plantuml from: $PLANTUML_JAR"

PLANTUML_FILES_DIR="Docs/plantuml"
PLANTUML_OUTPUT_DIR=".."

java -jar ${PLANTUML_JAR} ${PLANTUML_FILES_DIR} -t png -o ${PLANTUML_OUTPUT_DIR}
