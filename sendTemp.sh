#!/bin/bash
# Bash script to send muliple commands to test GardenControl App

echo "Sending commands to http://localhost:30000"
#for i in {1..10} 
##do 
    echo "sending msg $RANDOM" 
    curl -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"temp_out": 27.0, "temp_in": 28.0, "humidity_out": 40, "humidity_in": 45, "pump_status": 1}' http://localhost:3000/measures
    #curl -v -X POST http://localhost:30000/measures"
    
#done