#Flashing tasmota onto Sonoff S26 smart socket

## Install tasmotizer
- run `pip3 install tasmotizer` 

##Run tasmotizer
- run `tasmotizer.py` from cli

##Download tasmota
https://github.com/arendst/Tasmota/releases 

search for tasmota.bin

##Wiring & flashing
- Solder wire to corner connection and connect to vcc 
- Solder wire to middle connection and connect to gnd 
- Hold power button and connect usb
- LED should not flash!
- There is no need to solder the tx and rx, juist hold them while running the tasmotiser. 
- After flash is done, reconnect usb te reset. 

##Tasmota interface
- Connect WiFi of my Phone to Tasmota WiFi
- When connected, Press i button and remember the ip address of router.
- In Safari, go to ip address of router
- Set SSID and password for main router

Remove wiring and put it back together

## Setup MQTT
- Go to router interface and find the IP Address of our new Tasmota device
- Go to ip adress in Safari
- Set the following information on the Configuration->configure MQTT page:
  - Host: 192.168.1.x
  - port: 1883
  - user: homeassistant
  - password: password of mqtt broker (set in secrets.yaml)
  - topic: SonoffS26

Add the following configuration in configuration.yaml
```
  - platform: mqtt
    name: "SonoffS26"
    state_topic: "stat/SonoffS26/POWER"
    command_topic: "cmnd/SonoffS26/power"
    payload_on: "ON"
    payload_off: "OFF"
    qos: 1
    optimistic: false 
    retain: true    
```