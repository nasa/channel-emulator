<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modASM_Remove</name>
  <title>ASM Detachment</title>
  <cat>CCSDS</cat>
  <depends>
  </depends>
  <desc>The ASM detachment function accepts any type of network data units of uniform length. It must be configured with two things: the type of sync marker to look for and the expected length of each data unit. If an ASM is found where expected and there is enough data in the buffer, a new unit of the correct length is sent via the primary output link. If the ASM is missing, the check occurs again at the next octet and so on until the marker is found or the buffer is too small to hold one.</desc>
  <config>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
    <setting>
      <prompt>Sync Marker Pattern</prompt>
      <type format="hex">int</type>
	  <array/>
      <desc>The sync marker pattern as an array of integers.</desc>
      <default>0x1A,0xCF,0xFC,0x1D</default>
	  <setMethod>setASM</setMethod>
	  <min>0</min>
	  <max>255</max>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.markerPattern</path>
    </setting>
    <setting>
      <prompt>Expected Unit Length</prompt>
      <type>int</type>
      <desc>The expected length of all units, not including the ASM.</desc>
	  <setMethod>setExpectedUnitLength</setMethod>
	  <min>0</min>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.expectedUnitLength</path>
    </setting>
    <setting>
      <prompt>Allowed Marker Bit Errors</prompt>
      <type>int</type>
      <desc>The number of bit errors that can be encountered in an ASM to still be considered valid.</desc>
	  <setMethod>setAllowedMarkerBitErrors</setMethod>
	  <default>0</default>
	  <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.allowedMarkerBitErrors</path>
    </setting>
  </config>
</modinfo>
