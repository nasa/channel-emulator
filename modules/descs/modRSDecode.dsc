<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modRSDecode</name>
  <title>Reed-Solomon Decoding</title>
  <cat>CCSDS</cat>
  <depends>
  </depends>
  <desc>This module accepts 8-bit aligned network data of unspecified length (up to a configured maximum). It uses parity-check symbols that are appended to the received data unit to correct errors in the message. It allows for either 8 or 16 errors per code word (called maxErrorsPerCodeWord by modRSDecode) and interleaving depths of 1 (i.e. no interleaving), 2, 3, 4, 5, and 8 (called interleavingDepth by modRSDecode). However, modRSDecode does not place restrictions on interleaving depth. A new data unit, consisting of an untyped wrapper around the corrected message and minus the parity checks symbols is sent via the primary output link.</desc>
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
      <prompt>Interleaving Depth</prompt>
      <type>int</type>
      <desc>The number of codewords to use to generate parity check symbols.</desc>
	  <setMethod>setInterleavingDepth</setMethod>
	  <required/>
	  <default>1</default>
	  <min>1</min>
	  <max>255</max>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.interleavingDepth</path>
    </setting>
    <setting>
      <prompt>Max Errors Per Codeword</prompt>
      <type>int</type>
      <desc>The maximum number of errors that can be corrected per codeword; must be either 8 or 16.</desc>
	  <setMethod>setMaxErrorsPerCodeWord</setMethod>
	  <required/>
	  <default>8</default>
        <acceptedValues>
                <item>8</item>
                <item>16</item>
        </acceptedValues>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.maxErrorsPerCodeword</path>
    </setting>
  </config>
</modinfo>
