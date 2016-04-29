<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <config>
    <setting>
      <prompt>Virtual Channel Identifier</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>7</max>
      <desc>Set the Virtual Channel Identifier for this segment to an integer between 0 and 7.</desc>
	  <setMethod>setVCID</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.VCID</path>
    </setting>
  </config>
</modinfo>
