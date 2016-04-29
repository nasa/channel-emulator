<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_VC_Demux</name>
  <title>Virtual Channel De-multiplexing Function</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>This module accepts TM Transfer Frame wrappers on its primary input, separates them based on their Virtual Channel Identifier (VCID), and sends them out an associated primary output. There can be as many primary outputs as there are possible virtual channels in one master channel, and each is associated with a single VCID.</desc>
  <config>
	<ref>
		<id>TM_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.TM_PhysicalChannel.idlePattern</ignore>
    </ref>
    <ref>
		<id>TM_Common_MC_Settings</id>
    </ref>
    <setting>
      <prompt>Primary Outputs</prompt>
	  <desc>TM Transfer Frames received with each Virtual Channel Identifier are sent on to either the Primary or Auxiliary input of the specified segments. The segments must already exist in the channel.</desc>
	  <setMethod>connectPrimaryOutput</setMethod>
	  <unsetMethod>DO_NOT_UNSET</unsetMethod>
	  <type>structList</type>
      <structList>
        <setting>
          <prompt>Target Segment</prompt>
          <type>output</type>
          <desc>The existing segment that will receive the output.</desc>
          <order>1</order>
        </setting>
        <setting>
          <prompt>Target Input Rank</prompt>
          <type>string</type>
          <acceptedValues>
            <item value="PrimaryInput">Primary</item>
            <item value="AuxiliaryInput">Auxiliary</item>
          </acceptedValues>
          <desc>Primary or Auxiliary.</desc>
          <order>2</order>
        </setting>
        <setting>
          <prompt>VCID</prompt>
          <type>int</type>
          <desc>The Virtual Channel Identifier of frames to be sent to this target.</desc>
          <min>0</min>
          <max>7</max>
          <order>3</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutputs</path>
    </setting>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>