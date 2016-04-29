<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modASM_Add</name>
  <title>ASM Attachment</title>
  <cat>CCSDS</cat>
  <depends>
  </depends>
  <desc>The ASM attachment function accepts any type of network data of unspecified length and prepends a sync marker of the configured type. The modified data unit is sent out via the primary output link.</desc>
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
      <desc>Set the sync marker pattern to attach as an array of integers.</desc>
      <default>0x1A,0xCF,0xFC,0x1D</default>
	  <setMethod>setASM</setMethod>
	  <required/>
	  <min>0</min>
	  <max>255</max>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.markerPattern</path>
    </setting>
  </config>
</modinfo>
