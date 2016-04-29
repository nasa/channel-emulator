<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_VC_Demux</name>
  <title>Virtual Channel De-multiplexing Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>The Virtual Channel De-multiplexing module accepts AOS Transfer Frames that may have varying GVCIDs. It selects them based on their GVCID and sends them out one of several primary outputs to a segment that only accepts a single GVCID.</desc>
  <config>
    <ref>
		<id>AOS_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.idlePattern</ignore>
    </ref>
    <ref>
		<id>AOS_Common_MC_Settings</id>
    </ref>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
    <setting>
      <prompt>Primary Outputs</prompt>
	  <desc>AOS Transfer Frames received with each Spacecraft Identifier are sent on to either the Primary or Auxiliary input of the specified segments. The segments must already exist in the channel.</desc>
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
          <max>63</max>
          <order>3</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutputs</path>
    </setting>
  </config>
</modinfo>

