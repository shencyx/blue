package.cpath = package.cpath..";".."D:\\projects\\blue\\release\\?.dll"
package.path = package.path..";".."D:\\projects\\blue\\common\\lua\\?.lua"
print("in logic")
require "util"
mo = require "mongo_oper"


local what = 5
for i = 0, what do
	print("just a test 222", i)
end
g_mgr = 0

logicTimer = function(now, argv)
	print("----->logicTimer", now, argv, self)
	my:sendIns("log", 2, "he he")
	g_mgr = g_mgr + 1
	if g_mgr % 2 == 0 then
		print("logic send mongo")
		--my:sendIns("mongo", 3, "mongo test")
		mo.findOne("faction", {name="yyyyyyyppp"}, {})
	end
end
require "bson_test"

print("end logic")