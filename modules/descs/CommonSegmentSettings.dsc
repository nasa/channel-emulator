<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <config>
    <setting>
      <prompt>High Water Mark</prompt>
      <type>int</type>
      <desc>The maximum octet count that the segment's message queue can hold. When this limit is reached, new messages are disallowed and writers block until the queue size falls below the low water mark.</desc>
      <default>4194304</default>
      <min>0</min>
      <setMethod>setHighWaterMark</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.highWaterMark</path>
    </setting>
    <setting>
      <prompt>Low Water Mark</prompt>
      <type>int</type>
      <desc>If the message queue has hit the high water mark, wait until the size drops below this many octets before accepting data again.</desc>
      <default>3145728</default>
      <min>0</min>
      <setMethod>setLowWaterMark</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.lowWaterMark</path>
    </setting>
    <setting>
      <prompt>Device Name</prompt>
      <type>device</type>
      <desc>The segment will read or write directly from/to this device, which must be defined in the Devices section. Only certain segments do this and are typically named "mod*Receiver" or "mod*Transmitter".</desc>
      <setMethod>setDevice</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</path>
    </setting>
    <setting>
      <prompt>Primary Output</prompt>
      <type>structList</type>
      <desc>Primary output is sent to either the "Primary" or "Auxiliary" input of the specified segment. That segment must already exist in the channel before being specified here.</desc>
      <setMethod>connectPrimaryOutput</setMethod>
      <structList dimensions="1" grow="false">
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
            <item value="AuxInput">Auxiliary</item>
          </acceptedValues>
          <desc>Primary or Auxiliary.</desc>
          <order>2</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutput</path>
	</setting>
    <setting>
      <prompt>Auxiliary Output</prompt>
      <type>structList</type>
	  <desc>Auxiliary output is sent to either the "Primary" or "Auxilary" input of the specified segment. That segment must already exist in the channel before being specified here.</desc>
      <setMethod>connectAuxiliaryOutput</setMethod>
	  <structList dimensions="1" grow="false">
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
            <item>Primary</item>
            <item>Auxiliary</item>
          </acceptedValues>
          <desc>Primary or Auxiliary.</desc>
          <order>2</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</path>
	</setting>
    <setting>
      <prompt>Immediate Start</prompt>
      <type>boolean</type>
      <desc>If true, the segment will begin processing as soon as it is configured. Otherwise, it will need to receive a remote directive to start it.</desc>
      <default>true</default>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.immediateStart</path>
	</setting>
    <setting>
      <prompt>Maximum Transmission Unit (MTU)</prompt>
      <type>int</type>
      <desc>Maximum octets to transmit per data unit. Some segments will override with a derived value.</desc>
      <min>0</min>
      <default>0</default>
	  <setMethod>setMTU</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</path>
	</setting>
    <setting>
      <prompt>Maximum Receive Unit (MRU)</prompt>
      <type>int</type>
      <desc>Maximum per data unit to receive. Some segments will override with a derived value.</desc>
      <min>0</min>
      <default>0</default>
	  <setMethod>setMRU</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</path>
	</setting>
    <setting>
      <prompt>Microseconds Between Sends</prompt>
      <type>int</type>
      <min>0</min>
      <desc>Wait the specified integer microseconds between data transmissions, whether incoming traffic has been received or not. The segment will need to generate idle units when there is no new data to send.</desc>
	  <setMethod>setIntervalUsec</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</path>
	</setting>
  </config>
</modinfo>
