require("min_heap")
--ʱ�������
--ʱ������Ϊ��λ
TimerMgr = oo.class(nil, "TimerMgr")

local _6_DAY_SEC = 86400 * 6
local _7_DAY_SEC = 86400 * 7

function TimerMgr:__init()
	self.heap = Min_heap()
	
	self.nowMs = my:getNowMs()	--��ǰʱ�����
	self.nowS = math.floor(self.nowMs / 1000)	--��ǰʱ����
	
	self.onNewDayList = {}
	self.onNewDayListId = 0

	self.onEveryDay = {}
	self.onEveryDayId = 0

	self.today = self:_getToday()
	self.tomorrow = self.today + 86400;
end

function TimerMgr:getTime()
	return self.nowS
end

function TimerMgr:getTimeMs()
	return self.nowMs
end

--�ظ�����, stepTime:ÿ���ʱ�䣨�룩
function TimerMgr:registerRepeat(stepTime, obj, method, args)
	local timestamp = self.nowS + stepTime
	local value = {obj, method, args, stepTime}
	return self.heap:push(timestamp, value)
end

--����һ�κ�ͻ�ɾ�� method:Ϊ�ַ���
function TimerMgr:register1(hour, minute, second, timeout, obj, method, args)
	local timestamp = self.today + hour * 3600 + minute * 60 + second
	return self:register(timestamp, timeout, obj, method, args)
end

--����һ�κ�ͻ�ɾ��
function TimerMgr:register(timestamp, timeout, obj, method, args)
	local now = self.nowS
	if timestamp < now then
		if timestamp + timeout < now then
			return
		end
		timestamp = now + 1
	end
	if timestamp > self.tomorrow then
		print("error TimerMgr:register:", timestamp)
		print(debug.traceback())
		return
	end

	local value = {obj, method, args}
	
	return self.heap:push(timestamp, value)
end

function TimerMgr:unregister(id)
	self.heap:erase(id)
end

--ÿ���Ǹ�ʱ�䶼����һ��
function TimerMgr:registEveryDay(hour, minute, timeout, obj, method, args)
	local value = {
		["obj"] = obj
		, ["method"] = method
		, ["args"] = args
		, ["hour"] = hour
		, ["minute"] = minute
		, ["timeout"] = timeout
	}
	
	self:register1(hour, minute, 0, timeout, obj, method, args)

	self.onEveryDayId = self.onEveryDayId + 1
	self.onEveryDay[self.onEveryDayId] = value
	return self.onEveryDayId
end

--��2�����Ч
function TimerMgr:unregistEveryDay(id)
	self.onEveryDay[id] = nil
end

--�������ֻ��Ҫע��һ�ξͻ��ظ����� method:Ϊ�ַ���
function TimerMgr:registOnNewDay(obj, method, args)
	local value = {
		["obj"] = obj
		, ["method"] = method
		, ["args"] = args
	}
	self.onNewDayListId = self.onNewDayListId + 1
	self.onNewDayList[self.onNewDayListId] = value
	return self.onNewDayListId
end

function TimerMgr:unregistOnNewDay(id)
	self.onNewDayList[id] = nil
end

function TimerMgr:onTimer(nowMs)
	self.nowMs = nowMs
	self.nowS = math.floor(nowMs / 1000)
	local now = self.nowS 
	while not self.heap:is_empty() do
		local o = self.heap:top()
		if o.key > now then
			break
		end
		
		self.heap:pop()
		
		local value = o.value
		local method = value[1][value[2]]
		if method then
			method(value[1], value[3])
		end
		if value[4] ~= nil  then
			local timestamp = now + value[4]
			if timestamp > now then
				self.heap:push(timestamp, value)
			else
				print("error TimerMgr repeat:", value[1], value[2], value[3], value[4])
			end
		end
	end

	if now > self.tomorrow then
		self:onNewDay()
	end
end

function TimerMgr:onNewDay()
	self.today = self:_getToday()
	self.tomorrow = self.today + 86400;
	--
	self.heap:clear()
	for k, v in pairs(self.onEveryDay) do
		self:register1(v.hour, v.minute, 0, v.timeout, v.obj, v.method, v.args)
	end
	for k, v in pairs(self.onNewDayList) do
		if v.method then
			v.obj[v.method](v.obj, v.args)
		end
	end
end

function TimerMgr:getToday()
	return self.today
end

function TimerMgr:getTomorrow()
	return self.tomorrow
end

function TimerMgr:_getToday()
	local today = os.date("*t", self.nowS)
	today.hour = 0
	today.min = 0
	today.sec = 0
	return os.time(today), today
end

function TimerMgr:getDate(t_time)
	local date = os.date("*t", t_time)
	return date
end

function TimerMgr:isToday(time)
	--print("is today", time, self.today, self.tomorrow)
	if time >= self.today and time <= self.tomorrow then
		return true
	end
	return false
end

--����Ϊ��һ��
function TimerMgr:isThisWeek(time)
	if time >= self.today and time <= self.tomorrow then
		return true
	end
	local today = os.date("*t")
	today.hour = 0
	today.min = 0
	today.sec = 0
	local sundayBegin = os.time(today) - ((today.wday -1) * 86400)
	--print("isThisWeek:",sundayBegin, time)
	if time >= sundayBegin and time < sundayBegin + _7_DAY_SEC then
		return true
	end
	return false
end

--��һΪ��һ��
function TimerMgr:isThisWeek2(time)
	if time >= self.today and time <= self.tomorrow then
		return true
	end
	local today = os.date("*t")
	today.hour = 0
	today.min = 0
	today.sec = 0
	local sundayBegin
	if today.wday == 1 then
		sundayBegin = os.time(today) - (_6_DAY_SEC)
	else
		sundayBegin = os.time(today) - ((today.wday -2) * 86400)
	end
	--print("isThisWeek:",sundayBegin, time)
	if time >= sundayBegin and time < sundayBegin + _7_DAY_SEC then
		return true
	end
	return false
end

--
g_timerMgr = g_timerMgr or TimerMgr()