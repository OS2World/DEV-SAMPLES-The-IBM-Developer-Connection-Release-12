/* MKADMIN.CMD */
/* Saves admin account and password for later use */
Say "Use this program to save a special administrative account"
Say "for migration purposes only."
Say "Delete that account from the domain after migration."
AdminFile = '\MIGRATE\ADMIN.DAT'
Say "Please provide the name of an administrator account "
Say "on the migrating LAN Manager domain : "
Pull AdminName
If AdminName = "" Then Exit
LAdminName = "NAME "AdminName
Say "Please provide the password for that account : "
Pull AdminPwd
LAdminPwd = "PWD "AdminPwd
Call LineOut AdminFile,LAdminName
Call LineOut AdminFile,LAdminPwd
Call LineOut AdminFile
