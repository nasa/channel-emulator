<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_Macros</name>
  <title>TM Macros</title>
  <desc>Creates complete TM channels with minimal information.</desc>
  <cat>macro</cat>
  <depends></depends>
  <macro>
    <title>TM Forward Channel</title>
    <desc>Create an TM forward Physical Channel, which can support all available service types. Following creation of the channel, devices and device connectors will still be required, and the settings of each segment should be reviewed. Operational Control Field (OCF) and Frame Secondary Header (FSH) services can be added at that time.</desc>
    <method>newForwardChannel</method>
    <setting>
      <prompt>Frame Length</prompt>
      <type>int</type>
      <desc>The exact size for all TM Transfer Frames in the channel in whole octets.</desc>
      <default>256</default>
      <min>0</min>
      <path>frameLength</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Packet Services</prompt>
      <type>int</type>
      <desc>The number of virtual channels that will provide Packet Services.</desc>
      <default>1</default>
      <min>0</min>
      <path>packetSvcCount</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>VC Access Services</prompt>
      <type>int</type>
      <desc>The number of virtual channels that will provide VC Access Services.</desc>
      <default>0</default>
      <min>0</min>
      <path>vcAccessSvcCount</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>VC Frame Services</prompt>
      <type>int</type>
      <desc>The number of virtual channels that will consist of VC Frame Services.</desc>
      <default>0</default>
      <min>0</min>
      <max>63</max>
      <path>vcFrameSvcCount</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Master Channels</prompt>
      <type>int</type>
      <desc>Select the number of master channels to create. Multiplies the number of Packet, Bitstream, VC Access, and VC Frame Services.</desc>
      <default>1</default>
      <min>1</min>
      <max>256</max>
      <path>masterChannelCount</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>MC Frame Services</prompt>
      <type>int</type>
      <desc>The number of master channels that will consist of MC Frame Services.</desc>
      <default>0</default>
      <min>0</min>
      <max>256</max>
      <path>mcFrameSvcCount</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Frame Error Control</prompt>
      <type>boolean</type>
      <desc>If true, enable error control for the entire frame via a checksum.</desc>
      <default>false</default>
      <path>useFrameEC</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Correctable Errors Per Codeword</prompt>
      <type>int</type>
      <acceptedValues>
        <item>0</item>
        <item>8</item>
        <item>16</item>
      </acceptedValues>
      <desc>If non-zero, generate Reed-Solomon parity symbols and append them to each frame.</desc>
      <default>8</default>
      <path>maxErrorsReedSolomon</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Attached Sync Markers</prompt>
      <type>boolean</type>
      <desc>If true, attach an sync marker to each frame.</desc>
      <default>true</default>
      <partOfSettingsStruct/>
      <path>useASM</path>
    </setting>
	<setting>
      <prompt>Pseudo-Randomization</prompt>
      <type>boolean</type>
      <desc>If true, XOR each data unit with a pre-determined sequence.</desc>
      <default>true</default>
      <path>usePseudoRandomize</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Minimize Segments</prompt>
      <type>boolean</type>
      <desc>If true, implement the template with as few segments as possible (i.e. no mux functions when zero or one services exist).</desc>
      <default>false</default>
      <partOfSettingsStruct/>
      <path>minimize</path>
    </setting>
    <setting>
      <prompt>Segment Name Prefix</prompt>
      <type>id</type>
      <desc>A string to prepend to each segment name.</desc>
      <partOfSettingsStruct/>
      <path>segNamePrefix</path>
    </setting>
    <setting>
      <prompt>Segment Name Suffix</prompt>
      <type>id</type>
      <desc>A string to append to each segment name.</desc>
      <partOfSettingsStruct/>
      <path>segNameSuffix</path>
    </setting>
  </macro>
  <macro>
    <title>TM Return Channel</title>
	<desc>Create an TM return Physical Channel, which can support all available service types. Following creation of the channel, devices and device connectors will still be required, and the settings of each segment should be reviewed. Following creation of the channel, devices and device connectors will still be required, and the settings of each segment should be reviewed. Operational Control Field (OCF) and Frame Secondary Header (FSH) services can be added at that time.</desc>
    <method>newReturnChannel</method>
    <setting>
      <prompt>Frame Length</prompt>
      <type>int</type>
      <desc>The exact size for all TM Transfer Frames in the channel in whole octets.</desc>
      <default>256</default>
      <min>0</min>
      <path>frameLength</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Packet Services</prompt>
      <type>int</type>
      <desc>The number of virtual channels that will provide Packet Services.</desc>
      <default>1</default>
      <min>0</min>
      <path>packetSvcCount</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>VC Access Services</prompt>
      <type>int</type>
      <desc>The number of virtual channels that will provide VC Access Services.</desc>
      <default>0</default>
      <min>0</min>
      <path>vcAccessSvcCount</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Master Channels</prompt>
      <type>int</type>
      <desc>Select the number of master channels to create. Multiplies the number of Packet, Bitstream, and VC Access Services.</desc>
      <default>1</default>
      <min>1</min>
      <max>256</max>
      <partOfSettingsStruct/>
      <path>masterChannelCount</path>
    </setting>
    <setting>
      <prompt>Frame Error Control</prompt>
      <type>boolean</type>
      <desc>If true, enable error control for the entire frame via a checksum.</desc>
      <default>false</default>
      <partOfSettingsStruct/>
      <path>useFrameEC</path>
    </setting>
    <setting>
      <prompt>Correctable Errors Per Codeword</prompt>
      <type>int</type>
      <acceptedValues>
        <item>0</item>
        <item>8</item>
        <item>16</item>
      </acceptedValues>
      <desc>If non-zero, decode Reed-Solomon parity symbols attached to each frame.</desc>
      <default>8</default>
      <partOfSettingsStruct/>
      <path>maxErrorsReedSolomon</path>
    </setting>
    <setting>
      <prompt>Attached Sync Markers</prompt>
      <type>boolean</type>
      <desc>If true, find a sync marker to each frame.</desc>
      <default>true</default>
      <partOfSettingsStruct/>
      <path>useASM</path>
    </setting>
    	<setting>
      <prompt>Pseudo-Randomization</prompt>
      <type>boolean</type>
      <desc>If true, XOR each data unit with a pre-determined sequence.</desc>
      <default>true</default>
      <path>usePseudoRandomize</path>
      <partOfSettingsStruct/>
    </setting>
    <setting>
      <prompt>Minimize Segments</prompt>
      <type>boolean</type>
      <desc>If true, implement the template with as few segments as possible (i.e. no demux functions when zero or one services exist).</desc>
      <default>false</default>
      <partOfSettingsStruct/>
      <path>minimize</path>
    </setting>
    <setting>
      <prompt>Segment Name Prefix</prompt>
      <type>id</type>
      <desc>A string to prepend to each segment name.</desc>
      <partOfSettingsStruct/>
      <path>segNamePrefix</path>
    </setting>
    <setting>
      <prompt>Segment Name Suffix</prompt>
      <type>id</type>
      <desc>A string to append to each segment name.</desc>
      <partOfSettingsStruct/>
      <path>segNameSuffix</path>
    </setting>
  </macro>
</modinfo>
