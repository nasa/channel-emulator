<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>devTcp4Client</name>
  <title>IPv4 TCP Client</title>
  <desc>This device creates and maintains an IPv4 TCP connection with a remote server. If the remote IP address and port are known on startup, the device attempts to connect immediately. If the attempt is unsuccessful, it will continue trying every five seconds until it succeeds. Similarly, if the connection is closed, the device will try to reconnect every five seconds.</desc>
  <cat>device</cat>
  <depends></depends>
  <config>
    <ref>
      <id>CommonDeviceSettings</id>
    </ref>
    <setting>
      <prompt>Remote Address</prompt>
      <type>ipv4</type>
      <required/>
      <desc>Set the numerical IPv4 address of the remote server to string "a.b.c.d", where each of a-d is a single octet</desc>
	  <setMethod>setAddress</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.address</path>
    </setting>
    <setting>
      <prompt>Remote Port</prompt>
      <type>int</type>
      <required/>
      <desc>Set the TCP port of the remote server to an integer between 0 and 65535.</desc>
	  <setMethod>setPort</setMethod>
	  <min>0</min>
	  <max>65535</max>
      <path>Devices.XYZ_MODNAME_XYZ.port</path>
    </setting>
	<setting>
      <prompt>Time Between Attempts (ms)</prompt>
      <type>int</type>
      <required/>
      <desc>The number of milliseconds to wait after a failed connection attempt before trying again.</desc>
	  <setMethod>setMsBetweenAttempts</setMethod>
	  <min>0</min>
	  <default>2500</default>
      <path>Devices.XYZ_MODNAME_XYZ.msBetweenAttempts</path>
    </setting>
  </config>
</modinfo>
