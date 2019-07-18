#!/bin/bash
# Bash script to send muliple commands to test GardenControl App

echo "Sending commands to http://localhost:30000"
for i in {1..10} 
do 
    echo "sending msg $RANDOM" 
    curl -v -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"temp_out":'$RANDOM', "temp_in":'$RANDOM'}' http://localhost:3000/measures
done