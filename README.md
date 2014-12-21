Packetizer
==========

Packetizer is a class to analyze data for a start and/or end condition. If both, start- and end-condition is met the data between start and end is returned via a callback function as raw bytes.

Examples for Arduino show the use with incoming serial data:

|Example | demonstrates
|------|----------
|simple_packets | show basic functionality of Packetizer
|simple_proto_send_receive | the use to control another remote Arduino<br>controls LED 13 on a remote Arduino
|simple_proto_send_receive_2 | based on simple_proto_send_receive<br>shows the use of byte-pair packets<br>controls the brightness of LEDs on a remote Arduino
