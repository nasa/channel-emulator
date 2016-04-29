<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_VC_Rcv_Access</name>
  <title>Virtual Channel Reception – Access Service</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>The Virtual Channel Reception – Access Service accepts TM Transfer Frame wrappers on its primary input queue. It extracts a VCA_SDU (Virtual Channel Access Service Data Unit) from each frame that is exactly the size of the TM Transfer Frame Data Field, and sends it via its primary output queue. However, if there is a problem with the frame, such as a size mismatch or the synchronization flag being unset, the data is dropped.</desc>
  <config>
    <ref>
		<id>TM_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.TM_PhysicalChannel.idlePattern</ignore>
    </ref>
    <ref>
		<id>TM_Common_MC_Settings</id>
    </ref>
    <ref>
		<id>TM_Common_VC_Settings</id>
    </ref>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
    </ref>
  </config>
</modinfo>
