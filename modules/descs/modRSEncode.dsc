<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modRSEncode</name>
  <title>Reed-Solomon Encoding</title>
  <cat>CCSDS</cat>
  <depends>
  </depends>
  <desc>This module accepts 8-bit aligned network data of unspecified length (up to a configured maximum). It generates interleaved parity-check symbols that are appended to the received data unit. It allows for either 8 or 16 errors per code word (called maxErrorsPerCodeWord by modRSEncode) and interleaving depths of 1 (i.e. no interleaving), 2, 3, 4, 5, and 8 (called interleavingDepth by modRSEncode). However, modRSEncode does not place restrictions on interleaving depth. The modified data unit is sent out via the primary output link. The module encodes the entire received unit, rather than truncating it and/or waiting for additional data. If the unit cannot fit in the message space provided by the configuration, it is dropped. A code word is a portion of the received unit (the “message”) with parity symbols attached; for 8-bit RS encoding code word length is always 255. To select the length of the largest unit than can fit into multiple interleaved code words, first find the message length for one code word as 255 – 2*E (e.g. for E = 16, the message length per code word is 223).  Next, multiply this by a value for I large enough to contain any incoming unit (e.g. for 512-octet frames, with E = 16, select I = 3, so that units of up to 669 octets may be encoded). The size of the encoded unit will be its original length plus 2*E*I.</desc>
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
