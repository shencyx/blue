
@echo 
cd %cd%

set out_path=D:/projects/blue/server/scene/generate/
copy /y ..\pb.proto .\pb.proto

protoc -o %out_path%pb.pb pb.proto
protoc -o %out_path%scene.pb scene.proto



pause