
@echo 
cd %cd%

set out_path=D:/projects/blue/server/logon/generate/
copy /y ..\pb.proto .\pb.proto

protoc -o %out_path%pb.pb pb.proto
protoc -o %out_path%account.pb account.proto



pause