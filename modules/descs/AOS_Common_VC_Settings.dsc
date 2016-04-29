<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <config>
    <setting>
      <prompt>Operational Control Field</prompt>
      <type>boolean</type>
      <desc>If true, use the Operational Control Field in this Virtual Channel.</desc>
	  <setMethod>setUseOperationalControlField</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.useOperationalControl</path>
    </setting>
    <setting>
      <prompt>Virtual Channel Identifier</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>63</max>
      <desc>Set the Virtual Channel Identifier for this segment to an integer between 0 and 63.</desc>
	  <setMethod>setVCID</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.VCID</path>
    </setting>
	<setting>
      <prompt>Service Type</prompt>
      <type>string</type>
	  <acceptedValues>
		  <item>Access</item>
		  <item>Bitstream</item>
		  <item>Packet</item>
	  </acceptedValues>
      <default>Bitstream</default>
      <desc>Specify the type of traffic that this segment will service.</desc>
      <setMethod>setServiceType</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.serviceType</path>
    </setting>
    <setting>
      <prompt>Virtual Channel Frame Count Cycle</prompt>
      <type>boolean</type>
      <desc>If true, count the number of times that the Virtual Channel Frame Count has been reset to 0.</desc>
	  <setMethod>setUseVCFrameCountCycle</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.useVCFrameCycle</path>
    </setting>
  </config>
</modinfo>
