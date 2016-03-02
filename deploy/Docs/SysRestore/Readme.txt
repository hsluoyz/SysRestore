System Restore Plugin for NSIS

2005-2006, 2010-2011, 2014 MouseHelmet Software

Author: Jason Ross aka JasonFriday13 on the forums

Will only work on Windows Me and Windows XP (and higher).

Requires NSIS 2.42+, uses new plugin api's.
Unicode version included.

Introduction
~~~~~~~~~~~~

This plugin was written because I noticed other installers set a restore point
when they were run, so I set about writing a plugin for NSIS that does the same.

Functions
~~~~~~~~~

Note: Avoid renaming files on windows ME as NSIS does not callback the function
after a reboot (like Quit does not return after being called). Because of this,
FinishRestorePoint cannot be called after a reboot to finish setting up the restore point.

SysRestore::StartInstallPoint TEXT

SysRestore::StartUninstallPoint TEXT
	
	Starts the restore point. TEXT is the description of the restore
	point. Recommended text: "Installed $(^Name)" or "Uninstalled $(^Name)"
	for the uninstaller. I recommend putting this call at the start of a
	section. Returns on the top of the stack. Except for 0, the error
	indicates the function failed to set a restore point. Here is a list of
	the possible return values:
 
	0	The function was successful.

	1	Start point already set (start function only).

	10 	The system is running in safe mode. 
	
	13 	The sequence number is invalid.
		(Not sure what to say for this error).

	80	Windows Me: Pending file-rename operations exist in the 
		file %windir%\Wininit.ini.	

	112	System Restore is in standby mode because disk space is low. 
		Windows Me:  This value is not supported. 
				
	1058	System Restore is disabled. 
	
	1359  	An internal error with system restore occurred.	

	1460 	The call timed out due to a wait on a mutex for setting restore points.

	Note: I don't know if these are the actual error codes, I just seached 
	the platform sdk include files for the defines shown in the platform sdk help.
	I have tested on the university computers and it returned a 1058 code which is
	system restore is disabled, and I have tested in safe mode and this gave me
	an error code of 10, so I am guessing that the codes are correct.

SysRestore::FinishRestorePoint

	Closes the restore point. Required, because then system restore knows
	what files have been added/changed/removed. Returns the same codes above, 
	as well as an extra return value:

	2	No Start point set (finish function only).

	I recommend putting this call at the end of the last section. See the example
	for "correct" usage.

SysRestore::RemoveRestorePoint

	Removes the restore point that was created with this plugin. Return values:

	3	No restore point to remove (remove function only).