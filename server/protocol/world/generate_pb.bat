
@echo 
cd %cd%

set out_path=D:/projects/blue/server/world/generate/
copy /y ..\pb.proto .\pb.proto

protoc -o %out_path%pb.pb pb.proto
protoc -o %out_path%world.pb world.proto



pause