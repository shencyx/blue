print("in test2")


local log2 = require "log"

local l = log2()
l:hello()
l:write()

l = nil
print("end test2")