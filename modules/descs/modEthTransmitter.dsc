<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEthTransmitter</name>
  <title>Ethernet Frame Transmission</title>
  <cat>Ethernet</cat>
  <depends>
	  <modName>devEthernet</modName>
  </depends>
  <desc>The Ethernet Frame Transmission function accepts an IEEE 802.3 Ethernet frame wrapper on its primary input, typically from an Ethernet Frame Encoding segment. It writes the buffer to a packet socket associated with a CE Ethernet Device. This module has a few settings that manage the link detection capabilities of the Ethernet Device. These are occasionally necessary because a CE channel can come online so quickly that a newly activated hardware device may not be ready to send. If data were written to it anyway, it would just be lost, which is not an acceptable situation for “unreliable” protocols where a connection is expected nevertheless. The link detection settings can be used to block transmission until a carrier is found or a timeout occurs.</desc>
  <config>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutput</ignore>
	</ref>
	<setting>
      <prompt>Link Check Frequency</prompt>
      <type>string</type>
	  <acceptedValues>
		  <item>Never</item>
		  <item>Once</item>
		  <item>Always</item>
	  </acceptedValues>
      <default>Never</default>
      <desc>How often to check for a carrier. Never: Do not check; Once: Check before the segment enters its active loop, but not again; Always: Check before each frame is sent.</desc>
      <setMethod>setLinkCheckFrequency</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.linkCheckFreq</path>
    </setting>
    <setting>
      <prompt>Link Check Attempts</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>When testing for a carrier, check the specified integer number of times before failing.</desc>
      <setMethod>setLinkCheckMaxIterations</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.linkCheckMaxIterations</path>
    </setting>
    <setting>
	  <prompt>Milliseconds Between Checks</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>When testing for a carrier, wait the specified integer milliseconds between checks.</desc>
      <setMethod>setLinkCheckSleepMSec</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.linkCheckSleepMsec</path>
    </setting>
  </config>
</modinfo>
