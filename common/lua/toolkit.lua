local daySecNumber = 86400

function f_getToday(t_time)
	local today = os.date("*t", t_time)
	today.hour = 0
	today.min = 0
	today.sec = 0
	return os.time(today), today
end

function f_getMidday(t_time)
	local today = os.date("*t", t_time)
	today.hour = 12
	today.min = 0
	today.sec = 0
	return os.time(today), today
end

function f_getTomorrow(t_time)
	local today = os.date("*t", t_time)
	today.hour = 0
	today.min = 0
	today.sec = 0
	return os.time(today) + daySecNumber
end

function f_getDate(t_time)
	return tonumber(os.date("%Y%m%d",t_time))
end

--返回当前日期所在星期的第一天(以星期天为起始)的时间戳
function f_getSunday()
	local today = os.date("*t")
	today.hour = 0
	today.min = 0
	today.sec = 0
	return os.time(today) - ((today.wday -1) * daySecNumber)
end

--获取周一时间
function f_getMonday()
	local f_sunday = f_getSunday()
	local time = engine.time()

	if time > f_sunday + daySecNumber then
		return f_sunday + daySecNumber
	else
		return f_sunday - 6 * daySecNumber
	end
end

function f_getMondayBefore(wday, hour, min, end_wday, end_hour, end_min)
	--开始时间
	local s_day = os.date("*t")
	s_day.wday = wday or 0
	s_day.hour = hour or 0
	s_day.min = min or 0
	--结束时间
	local e_day = os.date("*t")
	e_day.hour = end_hour or 0
	e_day.min = end_min or 0
	e_day.wday = end_wday or 0

	local f_sunday = f_getSunday()
	local start_t = f_sunday + wday * daySecNumber + hour * 3600 + min * 60
	local end_t =  f_sunday + end_wday * daySecNumber + end_hour * 3600 + end_min * 60

	if start_t < end_t then
		if ev.time < end_t then
			return start_t, end_t
		else
			return start_t + 7 * 24 * 3600, end_t + 7 * 24 * 3600
		end
	else
		if ev.time < end_t then
			return start_t - 7 * 24 * 3600, end_t
		else
			return  start_t, end_t + 7 * 24 * 3600
		end
	end
end

function f_formatTimeString(timeStr)
	local year,month,day,hour,min,sec = string.match(timeStr,"(%d+)-(%d+)-(%d+)%s(%d+):(%d+):(%d+)")
	local tmp = {}
	tmp.year = year or 0
	tmp.month = month or 0
	tmp.day = day or 0
	tmp.hour = hour or 0
	tmp.min = min or 0
	tmp.sec = sec or 0

	return os.time(tmp)
end