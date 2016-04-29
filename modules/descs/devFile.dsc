<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>devFile</name>
  <title>File Descriptor</title>
  <desc>Provide access to a file/file descriptor. A single file device can be either read or written to, but not both. A second file device can be configured to access the same file, but the results of doing so are untested.</desc>
  <cat>device</cat>
  <depends></depends>
  <config>
    <ref>
      <id>CommonDeviceSettings</id>
    </ref>
    <setting>
      <prompt>Filename</prompt>
      <type>string</type>
      <required/>
      <desc>The path and filename, below relative to the top directory, of the file to be operated on</desc>
	  <setMethod>setFilename</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.fileName</path>
    </setting>
    <setting>
      <prompt>Is Input</prompt>
      <type>boolean</type>
      <desc>If true, the file is used for reading; if false, use it for writing to (the file will be truncated if it already exists)</desc>
      <path>Devices.XYZ_MODNAME_XYZ.isInput</path>
    </setting>
  </config>
</modinfo>
