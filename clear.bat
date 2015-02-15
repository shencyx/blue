@echo off 
@echo 批处理删除文件

del *.obj /q /s 
del *.tlog /q /s 
del *.pdb /q /s 
del *.dll /q /s 

rem rd *debug*  /s 
rem rd release  /s 
@echo 请使用windows查找方法删除debug和release目录！ 
pause