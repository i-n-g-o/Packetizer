Packetizer
==========

Packetizer is a class to analyze data for a start and/or end condition. If both, start- and end-condition is met the data between start and end is returned via a callback function as raw bytes.

Examples for Arduino show the use with incoming serial data:

|Example | 
|------|----------
|simple_packets | show basic functionality of Packetizer
|Prose | vla

simple_packets:
- show basic functionality of Packetizer

simple_proto_send_receive:
- demonstrates the use to control another remote Arduino
- controls LED 13 on a remote Arduino

simple_proto_send_receive_2:
- based on simple_proto_send_receive
- shows the use of byte-pair packets
- controls the brightness of LEDs on a remote Arduino
