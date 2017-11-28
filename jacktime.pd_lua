local JT = pd.Class:new():register("jacktime")

-- Reports the current Jack transport information on three outlets, comprising
-- the current frame number, transport status (a symbol: stopped, rolling,
-- etc.) and BBT (if available, as a list of seven numbers: the current
-- position as a triple bar, beat, tick, the meter as a pair of numerator and
-- denominator, the number of ticks per beat, and the current tempo as a bpm
-- value).

-- Please check http://jackaudio.org/files/docs/html/transport-design.html for
-- details.

-- Once the object is kicked off with a bang or a nonzero number, the
-- information is queried at regular intervals. The object outputs the current
-- status only when it changes. A zero on the inlet stops reporting.

-- The update period in msecs can be given as the optional creation
-- argument. If not given then a hard-coded default of 10 msec is used, but
-- you can change this by setting the value of the self.period member below.

-- Finally, transport can be started and stopped with the messages 'start' and
-- 'stop', and you can seek to a position using a message of the form 'locate
-- pos' where pos is specified in frames (samples). These messages also work
-- if transport information reporting is off.

function JT:initialize(name, atoms)
    self.inlets = 1
    self.outlets = 3
    self.last = { fr = -1, status = "none", bbt = nil}
    self.period = #atoms>0 and atoms[1] or nil
    if type(self.period) ~= "number" then
       self.period = 10 -- hard-coded default
    end
    return true
end

function JT:postinitialize()
   self.clock = pd.Clock:new():register(self, "tick")
end

function JT:finalize()
   self.clock:destruct()
end

function JT:in_1_bang()
   self:tick()
end

function JT:in_1_float(f)
   if f ~= 0 then
      self:tick()
   else
      self.clock:unset()
   end
end

jtime = require("jtime")

function JT:tick()
   function bbt_eq(bbt1, bbt2)
      if bbt1 == nil or bbt2 == nil then
	 return bbt1 == bbt2
      elseif #bbt1 ~= #bbt2 then
	 return false
      else
	 for i = 1, #bbt1 do
	    -- we only do a shallow comparison of the elements here, which are
	    -- supposed to be numbers
	    if bbt1[i] ~= bbt2[i] then
	       return false
	    end
	 end
	 return true
      end
   end
   fr, tm, status, bbt = jtime.jtime()
   if bbt and not bbt_eq(bbt, self.last.bbt) then
      self:outlet(3, "list", bbt)
      self.last.bbt = bbt
   end
   if status ~= self.last.status then
      self:outlet(2, "symbol", {status})
      self.last.status = status
   end
   if fr ~= self.last.fr then
      self:outlet(1, "float", {fr})
      self.last.fr = fr
   end
   self.clock:delay(self.period)
end

function JT:in_1_start()
   jtime.jtime_start()
end

function JT:in_1_stop()
   jtime.jtime_stop()
end

function JT:in_1_locate(atoms)
   if #atoms ~= 1 or type(atoms[1]) ~= "number" then
      self:error("jacktime: locate must have one number as argument")
   else
      jtime.jtime_locate(atoms[1])
   end
end
