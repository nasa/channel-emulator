<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_B_PDU_Add</name>
  <title>Bitstream Processing Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This service accepts wrappers with untyped data, splits (or merges) their buffers into the length configured for the Bitstream PDU Data Field, and finally transmits Bitstream PDU wrappers on its primary output.[+br/+][+br/+]It can be configured as either asynchronous or periodic. If periodic, a Bitstream PDU will always be sent after a constant time interval. If too little (or no) data is available at transmission time, the Data Field will be filled with an idle pattern.[+br/+][+br/+]To determine the length of outgoing Bitstream PDUs, modAOS_B_PDU_Add first checks for a locally specified MTU (e.g. specified by the configuration file MTU setting in the segment’s section). If that value is zero, it queries its primary output target for an MRU value and uses it for the length. If still zero or too short, the service does not have enough information to create a new Bitstream PDU and must drop any data it receives.</desc>
  <config>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
    <setting>
      <prompt>Immediate Send</prompt>
      <type>boolean</type>
	  <default>true</default>
      <desc>If true, transmit each B_PDU as it is created (default). If false, wait until an incoming buffer has been completely encoded into multiple B_PDUs before sending.</desc>
	  <setMethod>setImmediateSend</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.immediateSend</path>
    </setting>
    <setting>
      <prompt>Unit Length</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>Set the size of each outgoing Bitstream PDU to the specified number of whole octets. This is the same as setting the MTU, however, if unspecified or set to zero, it is automatically derived from the MRU of its primary output target. The latter is the preferred behavior in a complete AOS channel.</desc>
	  <setMethod>setLength</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</path>
    </setting>
  </config>
</modinfo>
