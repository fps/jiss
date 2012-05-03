require "jissing"

e = jiss.engine()

p = [[
x   x   x   x   
  y   y   y   y
    z       z
]]

xs = 
	function(tick, p, f, y) 
		ret = {}
		for line in p:gmatch("[^\r\n]+") do
			for i = 1, #line do
				if line:sub(i,i) ~= " " then
					-- print(f(line:sub(i,i), y))
					table.insert(ret, {(i-1)*tick, f(line:sub(i,i), y)})
				end
			end
		end
		return ret
	end

format_xs = 
	function(tick, p, s, t)
		return xs(tick, p, function(x,y) return string.format(s, y[x]) end, t)
	end

tick = e:get_samplerate()/4

play(loop(16 * tick, cpp_seq(e, "s1", format_xs(tick, p, 'std::cout<< "hi %s" << std::endl;', {x="10", y=20, z=30}))))

e:start()