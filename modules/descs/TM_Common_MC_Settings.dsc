<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <config>
    <setting>
      <prompt>Spacecraft Identifier</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>1024</max>
      <desc>Set the Spacecraft Identifier for the Master Channel to the specified integer.</desc>
	  <setMethod>setSCID</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.SCID</path>
    </setting>
    <setting>
      <prompt>Operational Control Field</prompt>
      <type>boolean</type>
      <desc>If true, use the Operational Control Field in this Master or Virtual Channel.</desc>
	  <setMethod>setUseOperationalControlField</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.useOperationalControl</path>
    </setting>
    <setting>
      <prompt>Frame Secondary Header (FSH)</prompt>
      <type>boolean</type>
      <desc>If true, use the Frame Secondary Header in this Master or Virtual Channel.</desc>
	  <setMethod>setUseFSH</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.useFSH</path>
    </setting>
    <setting>
      <prompt>FSH Length</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>If this an integer other than 0, fix the size of the FSH to it. If s is zero than the FSH will be variable in length. Only used if useFSH is true.</desc>
	  <setMethod>setFSHSize</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.fshSize</path>
    </setting>
    <setting>
      <prompt>Synchronization Flag</prompt>
      <type>boolean</type>
      <desc>If true, the synchronization flag will be set (or expected to be set in the case of receiving services) for all TM Transfer Frames. If false, the flag will be unset (or expected to be unset).</desc>
	  <setMethod>setSyncFlag</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.syncFlag</path>
    </setting>
	</config>
</modinfo>