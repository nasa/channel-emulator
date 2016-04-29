<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>devEthernet</name>
  <title>Ethernet Interface</title>
  <desc>This portion of the Channel Emulator's Ethernet device management deals with activating and deactivating the device, setting flags, and getting counters. When management of an interface is finished, the CE restores its flags to the state it found them in, but counters are not modified.</desc>
  <cat>device</cat>
  <depends></depends>
  <config>
    <ref>
      <id>CommonDeviceSettings</id>
    </ref>
    <setting>
      <prompt>Interface Tag</prompt>
      <type>string</type>
      <desc>If set, this string will be used to refer to the interface at the OS level instead of the name of the CE device. This allows, for example, for the CE device to be named "eth_0_vlan_5" even though the Ethernet interface it manages is actually called "eth0.5" by the OS.</desc>
	  <setMethod>setIfaceName</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.ifaceName</path>
      <ethernetInterfaces/>
    </setting>
    <setting>
      <prompt>Promiscuous</prompt>
      <type>boolean</type>
      <desc>If true, put the interface in promiscuous mode. If false, don't change the promiscuous mode setting.</desc>
	  <setMethod>setPromiscuous</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.flagPromisc</path>
    </setting>
    <setting>
      <prompt>NoARP</prompt>
      <type>boolean</type>
      <desc>If true, disable ARP request processing on the interface. If false, don't change the ARP setting</desc>
	  <setMethod>setNoARP</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.flagNoARP</path>
    </setting>
    <setting>
      <prompt>Snap Length</prompt>
      <type>int</type>
      <desc>Number of octets to read from the interface (performed by a "receiving" segment)</desc>
	  <setMethod>setSnapLength</setMethod>
	  <min>0</min>
      <path>Devices.XYZ_MODNAME_XYZ.snapLen</path>
    </setting>
    <setting>
      <prompt>Activate On Load</prompt>
      <type>boolean</type>
      <desc>If a is true, activate (bring up) the Ethernet interface as soon as management begins. If false, wait until instructed to do so</desc>
      <default>true</default>
      <path>Devices.XYZ_MODNAME_XYZ.activateOnLoad</path>
    </setting>
  </config>
</modinfo>
