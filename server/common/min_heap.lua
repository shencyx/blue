
Min_heap = oo.class(nil, "Min_heap")

function Min_heap:__init()
	self.heap = {}
	self.map_list = {}
	self.count = 0
	self.next_id = 1
end

function Min_heap:alloc_id()
	local id = self.next_id
	self.next_id = self.next_id + 1
	return id
end

function Min_heap:top()
	return self.heap[1]
end

function Min_heap:is_empty()
	return 0 == self.count
end

function Min_heap:size()
	return self.count
end

function Min_heap:clear()
	self.heap = {}
	self.map_list = {}
	self.count = 0
end

function Min_heap:shift_up(i, e)
	local p = math.floor(i / 2)
	while (i > 1 and self.heap[p].key > e.key) do
		local o = self.heap[p]
		self.heap[i] = o
		o.index = i
		i = p
		p = math.floor(i / 2)
	end
	self.heap[i] = e
	e.index = i
end

function Min_heap:shift_down(i, e)
	local c = 2 * i
	local count = self.count
	while (c <= count) do
		if c ~= count and self.heap[c].key > self.heap[c + 1].key then
			c = c + 1
		end
		
		if e.key < self.heap[c].key then
			break
		end
		
		local o = self.heap[c]
		self.heap[i] = o
		o.index = i
		i = c
		c = 2 * i
	end
	
	self.heap[i] = e
	e.index = i
end

function Min_heap:push(key, value)
	local id = self:alloc_id()
	local e = {}
	e.key = key
	e.value = value
	e.id = id
	e.index = 0
	self.count = self.count + 1
	self:shift_up(self.count, e)
	self.map_list[id] = e
	return id
end

function Min_heap:pop()
	local count = self.count
	if 0 ~= count then
		self.map_list[self.heap[1].id] = nil
		self.count = count - 1
		self:shift_down(1, self.heap[count])
		self.heap[count] = nil
	end
end

function Min_heap:erase(id)
	local count = self.count
	if 0 ~= count then
		local e = self.map_list[id]
		if e and 0 ~= e.index then
			assert(e.index <= count, string.format("%d, %d", e.index, count))
			local p = math.floor(e.index / 2)
			local last = self.heap[count]
			self.count = count - 1 
			if e.index > 1 and self.heap[p].key > last.key then
				self:shift_up(e.index, last)
			else
				self:shift_down(e.index, last)
			end
			
			self.map_list[id] = nil
			self.heap[count] = nil
			e.index = 0
		end
	end
end
