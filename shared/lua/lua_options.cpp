
#include <stdio.h>
#include <iostream>
#include <vector>
#include "lua_options.h"

static void append_path(std::string& path, const char* str)
{
	path.append(LUA_PATHSEP).append(str);
	if (path[path.size() -1] != '/')
		path.append("/?.lua");
	else
		path.append("?.lua");
}

static void append_cpath(std::string& path, const char* str)
{
	path.append(LUA_PATHSEP).append(str);
	if (path[path.size() -1] != '/')
		path.append("/?.so");
	else
		path.append("?.so");
}

namespace lua
{
	bool optionInit(lua_State* L, const util::Option& op)
	{
		/*boost::filesystem::path binpath = resolve(boost::filesystem::path(argv[0]));
		op.binPath = binpath.string();
		op.binName = binpath.filename();
		op.binDir = binpath.remove_filename().string();*/

		std::string path;
		std::string cpath;
		lua_getglobal(L, "package");

		lua_getfield(L, -1, "path");
		path.assign(lua_tostring(L, -1));
		lua_pop(L, 1);

		lua_getfield(L, -1, "cpath");
		cpath.assign(lua_tostring(L, -1));
		lua_pop(L, 1);
		lua_pop(L, 1);

		/*std::string scriptpath = "./main.lua";
		std::string enginepath = ".";
		try
		{
			boost::program_options::options_description desc("Allowed options");
			desc.add_options()
				("help", "produce help message")
				("include-path,I", boost::program_options::value< std::vector<std::string> >(),"[-I require path]")
				("include-enginepath,S", boost::program_options::value< std::vector<std::string> >(),"[-S require enginepath]")
				("input-file", boost::program_options::value<std::string>(), "input file");

			boost::program_options::positional_options_description p;
			p.add("input-file", -1);

			boost::program_options::variables_map vm;        
			boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
				options(desc).positional(p).run(), vm);
			boost::program_options::notify(vm);  

			if(vm.count("help"))
			{
				std::cout << "USAGE: [-I require path] script\n" << std::endl;
				std::cout << "USAGE: [-S require engine path] script\n" << std::endl;
				std::cout << "NOTS:\n\tmudrv pathto/script.lua IMPLICIT exe -I pathto pathto/script.lua\n" << std::endl;
				return false;
			}
			if(vm.count("include-path"))
			{
				std::vector<std::string> arrpath = vm["include-path"].as< std::vector<std::string> >();
				for(int i=0; i < (int)arrpath.size(); ++i)
				{
					std::string strpath = arrpath[i];
					boost::filesystem::path bpath = resolve(boost::filesystem::path(strpath));
					strpath = bpath.string(); 

					append_path(path, strpath.c_str());
				}
			}
			if(vm.count("include-enginepath"))
			{
				std::vector<std::string> arrpath = vm["include-enginepath"].as< std::vector<std::string> >();
				for(int i=0; i < (int)arrpath.size(); ++i)
				{
					std::string strpath = arrpath[i];
					boost::filesystem::path bpath = resolve(boost::filesystem::path(strpath));
					strpath = bpath.string(); 

					append_path(path, strpath.c_str());
					enginepath = strpath;
				}
			}
			if(vm.count("input-file"))
			{
				scriptpath = vm["input-file"].as<std::string>();
			}

		}
		catch (std::exception& e)
		{
			std::cerr << "boost::program_options error: " << e.what() << "\n";
			return false;
		}
		catch(...) 
		{
			std::cerr << "Exception of unknown type!\n";
			return false;
		}*/

		//script

		/*boost::filesystem::path curPath = resolve(boost::filesystem::path(scriptpath));
		op.scriptPath = curPath.string();
		op.scriptName = curPath.filename();
		op.scriptDir = curPath.remove_filename().string();*/

		//增加输入参数到path
		for(size_t i = 0; i < op.includePathArr.size(); ++i)
		{
			append_path(path, op.includePathArr[i].c_str());
		}
		std::string enginepath = op.enginePath;
		append_path(path, enginepath.c_str());

		lua_getglobal(L, "package");
		lua_pushlstring(L, path.c_str(), path.size());
		lua_setfield(L, -2, "path");
		lua_pop(L, 1);

		enginepath += "/engine.lua";
		if(luaL_dofile(L, enginepath.c_str()) != 0)
		{
			std::cout << lua_tostring(L, -1) << std::endl;
			return false;
		}

		return true;
	}
}
