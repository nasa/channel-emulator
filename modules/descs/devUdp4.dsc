<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>devUdp4</name>
  <title>IPv4 UDP Device</title>
  <desc>The IPv4 UDP Server Device simply opens and binds to an IPv4 UDP socket. One module, usually modUdp4Receiver, can receive from the socket and another, usually modUdp4Transmitter, can send through it.</desc>
  <cat>device</cat>
  <depends></depends>
  <config>
    <ref>
      <id>CommonDeviceSettings</id>
    </ref>
    <setting>
      <prompt>Listen Address</prompt>
      <type>ipv4</type>
      <desc>Set the numerical IPv4 address to listen on to string "a.b.c.d", where each of a-d is a single octet</desc>
	  <setMethod>setAddress</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.address</path>
    </setting>
    <setting>
      <prompt>Listen Port</prompt>
      <type>int</type>
      <desc>Set the UDP port to listen on to an integer between 0 and 65535.</desc>
	  <setMethod>setPort</setMethod>
	  <min>0</min>
	  <max>65535</max>
      <path>Devices.XYZ_MODNAME_XYZ.port</path>
    </setting>
  </config>
</modinfo>
