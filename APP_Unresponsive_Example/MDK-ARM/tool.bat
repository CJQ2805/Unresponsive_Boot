::bin文件名

@set src_filename=Test

::需要命名成的文件名

@set dest_filename=SCUD_TEST

::.bin文件所在目录
@set src_path=APP_Unresponsive_Example

::命名后的文件放置的目标路径
@set dest_path=binary
::获取日期 将格式设置为：20210107
@set datevar=%date:~0,4%%date:~5,2%%date:~8,2%
@set timevar=%time:~0,2%
@if /i %timevar% LSS 10 (
set timevar=0%time:~1,1%
)
::获取时间中的分、秒 将格式设置为：3220 ，表示 32分20秒
@set timevar=%timevar%.%time:~3,2%.%time:~6,2%
@set copyfilename=%dest_filename%_%datevar%_%timevar%
::copy %path%\%filename%.bin ..\binary\%copyfilename%.bin
::copy %src_path%\%src_filename%.bin %dest_path%\%copyfilename%.bin
::echo generated: %copyfilename%.bin
@copy %src_filename%.bin  %dest_path%\%copyfilename%.bin
@echo generated:%copyfilename%.bin

::文件末尾加入crc32校验
call add_crc32 %dest_path%\%copyfilename%.bin
@echo %copyfilename%.bin has been add crc32.