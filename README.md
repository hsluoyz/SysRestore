# SysRestore
The SysRestore plug-in for NSIS packaging system forked and used by [Npcap](https://github.com/nmap/npcap). This plug-in is used to create a system restore point during [Npcap](https://github.com/nmap/npcap) installation.

# Build
Visual Studio 2013 (it's static linked, so no need to distribute additional DLLs)

# Usage
1. Copy the built ``ANSI x86`` version ``SysRestore.dll`` to your ``C:\Program Files (x86)\NSIS\Plugins`` path (most people use ``ANSI`` by default).
2. Feel free to use ``SysRestore::XXX`` functions in your ``.nsi`` script.

# Releases
https://github.com/hsluoyz/SysRestore/releases

# Original Official Site
http://nsis.sourceforge.net/SysRestore_plug-in
