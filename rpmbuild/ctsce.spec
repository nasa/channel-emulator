Name:           ctsce
Version:        1.8.6.5
Release:        1%{?dist}
Summary:        The NASA Channel Emulator developed for the CTS project at GRC

Group:          Applications/Communications
License:        NASA
URL:            http://channel-emulator.grc.nasa.gov
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  gcc-c++
BuildRequires:	libstdc++
BuildRequires:	nl3-devel
BuildRequires:	ace-devel
BuildRequires:	xmlrpc-c-c++
BuildRequires:	xmlrpc-c-devel
BuildRequires:	file-devel
BuildRequires:	boost-regex
BuildRequires:	libtool automake autoconf
Requires:       nl3
Requires:	ace
Requires:	xmlrpc-c-c++
Requires:	xmlrpc-c
Requires:	libstdc++
Requires:	boost-regex

%description
The Channel Emulator (CE) is a software-based networking tool.
Its primary functions are acting as flexible protocol gateway
and providing network emulation capabilities

%prep
%setup -q

%package mods-common
Group:          Applications/Communications
Summary:        Commonly-used modules for the CE
Requires:       ctsce == %{version}

%description mods-common
Commonly-used modules for the CE

%package mods-aos
Group:          Applications/Communications
Summary:        AOS-specific modules for the CE
Requires:       ctsce == %{version}

%description mods-aos
AOS-specific modules for the CE

%package mods-tm
Group:          Applications/Communications
Summary:        TM-specific modules for the CE
Requires:       ctsce == %{version}

%description mods-tm
TM-specific modules for the CE

%package mods-ecc
Group:          Applications/Communications
Summary:        Error correcting modules for the CE
Requires:       ctsce == %{version}

%description mods-ecc
Error correcting modules for the CE

%build
cmake	-DCMAKE_INSTALL_PREFIX:PATH=%{_prefix} \
	-DCMAKE_INSTALL_LIBDIR:PATH=%{_libdir} \
	-DCMAKE_INSTALL_SYSCONFDIR:PATH=%{_sysconfdir} \
	-DCMAKE_INSTALL_LIBEXECDIR:PATH=%{_libexecdir}
make

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
if [ -d %{buildroot}%{_prefix}/lib ]; then
	mv %{buildroot}%{_prefix}/lib %{buildroot}%{_prefix}/lib64
fi
find $RPM_BUILD_ROOT -name \*.la -exec rm {} \;
mkdir -p %{buildroot}%{_sysconfdir}
cat > %{buildroot}%{_sysconfdir}/ctsced.cfg <<EOF1
# Behavior controls
foreground = false;
debugLevel = 7;
defaultDebug = 7;
reactorThreads = 4;
saveCfgAtExit = false;

# HTTP server
listenAddress = "127.0.0.1";
listenPort = 8080;
xmlrpcPath = "/RPC2";
webDocDir = "/var/lib/ctsce/www";

# Other file/directory locations
cfgPath = "/etc/ctsce";
cfgFile = "devices-channels.cfg";
dllPath = "/usr/lib64/ctsce-modules";
fileIODir = "/var/lib/ctsce/fileIO";
logFile = "/var/log/ctsced.log";
EOF1

mkdir -p %{buildroot}%{_sysconfdir}/ctsce
cat > %{buildroot}%{_sysconfdir}/ctsce/devices-channels.cfg<<EOF2
Devices : {

};

Channels : {

};
EOF2
mkdir -p %{buildroot}%{_sysconfdir}/init.d
cp -p %{_builddir}/%{buildsubdir}/rpmbuild/ctsce.init %{buildroot}%{_sysconfdir}/init.d/ctsce
mkdir -m 0770 -p %{buildroot}%{_var}/lib/ctsce/fileIO
mkdir -m 0775 -p %{buildroot}%{_var}/lib/ctsce/www
cp -pr %{_builddir}/%{buildsubdir}/www/* %{buildroot}%{_var}/lib/ctsce/www
cp -pr %{_builddir}/%{buildsubdir}/modules/descs/*dsc %{buildroot}%{_libdir}/ctsce-modules/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%config(noreplace) %{_sysconfdir}/ctsced.cfg
%config(noreplace) %{_sysconfdir}/ctsce/devices-channels.cfg
%config(noreplace) %{_var}/lib/ctsce/fileIO
%{_var}/lib/ctsce/www
%{_sysconfdir}/init.d/ctsce
%{_libdir}/libCE_Device.*
%{_libdir}/libCE_Module.*
%{_libdir}/libNetworkData.*
%{_sbindir}/ctsced

%files mods-common
%defattr(-,root,root,-)
%{_libdir}/ctsce-modules/libdev*.so*
%{_libdir}/ctsce-modules/dev*.dsc
%{_libdir}/ctsce-modules/libmodEmulate*.so*
%{_libdir}/ctsce-modules/modEmulate*.dsc
%{_libdir}/ctsce-modules/libmodEth*.so*
%{_libdir}/ctsce-modules/modEth*.dsc
%{_libdir}/ctsce-modules/libmodExtractor.so*
%{_libdir}/ctsce-modules/modExtractor.dsc
%{_libdir}/ctsce-modules/libmodFd*.so*
%{_libdir}/ctsce-modules/modFd*.dsc
%{_libdir}/ctsce-modules/libmodResolvePhaseAmbiguity.so*
%{_libdir}/ctsce-modules/modResolvePhaseAmbiguity.dsc
%{_libdir}/ctsce-modules/libmodSplitter.so*
%{_libdir}/ctsce-modules/modSplitter.dsc
%{_libdir}/ctsce-modules/libmodTcp4*.so*
%{_libdir}/ctsce-modules/modTcp4*.dsc
%{_libdir}/ctsce-modules/libmodUdp4*.so*
%{_libdir}/ctsce-modules/modUdp4*.dsc
%{_libdir}/ctsce-modules/libmodUDP*.so*
%{_libdir}/ctsce-modules/modUDP*.dsc
%{_libdir}/ctsce-modules/libmodASM*.so*
%{_libdir}/ctsce-modules/modASM*.dsc
%{_libdir}/ctsce-modules/libmodPseudoRandomize*.so*
%{_libdir}/ctsce-modules/modPseudoRandomize*.dsc
%{_libdir}/ctsce-modules/libmodEncapPkt*.so*
%{_libdir}/ctsce-modules/modEncapPkt*.dsc
%{_libdir}/ctsce-modules/Common*Settings.dsc

%files mods-ecc
%defattr(-,root,root,-)
%{_libdir}/ctsce-modules/libmodResolvePhaseAmbiguity.so*
%{_libdir}/ctsce-modules/modResolvePhaseAmbiguity.dsc
%{_libdir}/ctsce-modules/libmodRS*.so*
%{_libdir}/ctsce-modules/modRS*.dsc

%files mods-aos
%defattr(-,root,root,-)
%{_libdir}/libCE_AOS_Module.*
%{_libdir}/ctsce-modules/libmodAOS*.so*
%{_libdir}/ctsce-modules/modAOS*.dsc
%{_libdir}/ctsce-modules/AOS_Common_*_Settings.dsc

%files mods-tm
%defattr(-,root,root,-)
%{_libdir}/libCE_TM_Module.*
%{_libdir}/ctsce-modules/libmodTM*.so*
%{_libdir}/ctsce-modules/modTM*.dsc
%{_libdir}/ctsce-modules/TM_Common_*_Settings.dsc

%post
chkconfig ctsce on

%preun
chkconfig ctsce off

%changelog
*Thu Feb 20 2014 Tad Kollar < > 1.8.6.5
--Made it possible to view some integers in hex format.
--Used SO_REUSEADDR in devTcp4Server.

*Fri Feb 14 2014 Tad Kollar < > 1.8.6.4
--Created the FdInputHandler base class for modUdp4Receiver, modTcp4Receiver, and modEthReceiver.

*Thu Feb 13 2014 Tad Kollar < > 1.8.6.3
--Added more counters and minor opts to modASM_Remove.

*Wed Feb 12 2014 Tad Kollar < > 1.8.6.2
--Backported NetworkData memory management fixes/opts from 1.9.x.

*Tue Dec 17 2013 Tad Kollar < > 1.8.6.1
--Fixed AOS and TM get_idle_pattern so it wouldn't crash the CE.

*Fri Dec 13 2013 Tad Kollar < > 1.8.6.0
--New 1.8.6.* branch to retain old Channel GUI while applying fixes.
--Fixes for valid TM/AOS Frame counting.
--Fixes for TM/AOS garbage collection when dealing with dropBadFrames.
--Fix for modEthFrame_Add that caused a seg fault when loaded.

*Fri Sep 27 2013 Tad Kollar < > 1.8.5.1
--Fixed connectOutput/connectPrimaryOutput for modTM_VC_Rcv_Packet.

*Wed Sep 25 2013 Tad Kollar < > 1.8.5
--Fixed ability to change a device when one is already in use by the calling segment, or the device is already in use by a different segment.
--Fixed ability to add outputs to the Demux modules after they've been initialized.
--Began implementing BitPattern classes that will generate standard bit patterns, usable by modules like devSource (not yet implemented).

*Thu Sep 05 2013 Tad Kollar < > 1.8.4
--modTM_Macros.newForwardChannel now inserts modTM_MC_Gen.
--modTM_Macros.newReturnChannel now inserts modTM_MC_Rcv.
--All of the AOS and TM macros accept a new "minimize" parameter, which will prevent the insertion of mux/demux modules if not required
--All of the AOS and TM macros accept a new "usePseudoRandomize" parameter, which will insert modPseudoRandomize at the appropriate place.
--All AOS and TM modules have a new "dropBadFrames" setting that defaults to true, with getDropBadFrames and setDropBadFrames methods available. It will cause malformed frames (or in the case of All Frames Reception, frames that fail ECC) to be discarded. A malformed frame is one with the wrong length or a wrong identifier (TVFN, SCID, VCID, etc., depending on the module). This setting is local to each channel segment. It replaces "performErrorControlHere" in the TM modules, which is basically what that setting used to do. There wasn't an equivalent in AOS.
--The default initial queue size for each segment is now 4MB instead of 16MB.
--Improved the garbage collection and locking for the data "wrapper" base class, NetworkData. It now only creates a lock object when a shallow copy is created, instead of for every object. It turns out these lock objects were not always being deleted, which had been causing a bit of a memory leak. Also, there is new a template function that performs some checks and invokes a reference-counting release() method, which is safer for shallow copies and multi-part data blocks than a simple delete.
--The manual and properties files were updated with the new settings.

*Thu Aug 01 2013   1.8.3
--Added modPseudoRandomize.

*Mon Jul 01 2013   1.8.2
--Data/config file selection, uploads, and downloads; XML schema.

*Fri May 31 2013   1.8.1
--Several bug fixes to TM modules and GUI.

*Tue May 28 2013   1.8.0
--New GUI is complete; module property files complete; new settings calls; bugfixes.

*Fri Sep 14 2012   13
--New GUI; many new setting/DLL management XML-RPC calls; module property files.

*Fri Jun 01 2012   12
--Added support for CCSDS TM protocol.

*Tue May 07 2012   11
--Converted to cmake from automake.

*Tue Apr 02 2012   10
--Wrote a new Reed-Solomon library to support CCSDS RS(255,223), RS(255,239), and RS(10,6).

*Tue Jan 13 2012   9
--Added options and a bad Encap counter to modAOS_M_PDU_Remove.

*Tue Jan 10 2012   8
--Added user-configurable fill pattern to modAOS_M_PDU_Add.

*Tue Jan 03 2012   7
--Added timeline capabilities to emulation mods. Improved debugging capabilities.

*Thu Dec 08 2011   6
--Fixed CCSDS Reed-Solomon compatibility problems. Improved Encap support.

*Wed Sep 30 2011   5
--Fixed watermark bug and set default MRU for modFdReceiver.

*Wed Sep 22 2011   4
--Fixed XML-RPC module.pause() method for modEthReceiver, modTcp4Receiver, and modUdp4Receiver.

*Wed Sep 07 2011   3
--Fixed XML-RPC module.pause() method.

*Tue Aug 30 2011   2
--Added web server content to RPM.

*Fri Aug 26 2011   1
--Initial Build.
