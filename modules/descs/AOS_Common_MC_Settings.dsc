<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <config>
    <setting>
      <prompt>Spacecraft Identifier</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>255</max>
      <desc>Set the Spacecraft Identifier for the Master Channel to the specified integer.</desc>
	  <setMethod>setSCID</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.SCID</path>
    </setting>
  </config>
</modinfo>