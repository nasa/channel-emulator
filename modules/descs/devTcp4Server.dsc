<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>devTcp4Server</name>
  <title>IPv4 TCP Server</title>
  <desc>The IPv4 TCP Server Device listens on a port for incoming connections. A module (i.e. modTcp4Receiver) must manage its listening and connected sockets, because a connection closed by a client can only be detected while attempting to receive input. Only one connection is accepted at a time although more may be queued.</desc>
  <cat>device</cat>
  <depends></depends>
  <config>
    <ref>
      <id>CommonDeviceSettings</id>
    </ref>
    <setting>
      <prompt>Listen Address</prompt>
      <type>ipv4</type>
      <default>127.0.0.1</default>
      <desc>Set the numerical IPv4 address to listen on to string "a.b.c.d", where each of a-d is a single octet</desc>
      <setMethod>setAddress</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.address</path>
    </setting>
    <setting>
      <prompt>Listen Port</prompt>
      <type>int</type>
      <desc>Set the TCP port to listen on to an integer between 0 and 65535.</desc>
      <default>12345</default>
      <setMethod>setPort</setMethod>
	  <min>0</min>
	  <max>65535</max>
      <path>Devices.XYZ_MODNAME_XYZ.port</path>
    </setting>
  </config>
</modinfo>
