require "jissing"

e = jiss.engine()

p = [[
x   x   x   x   
  y   y   y   y
    z       z
]]



tick = e:get_samplerate()/4

play(
	loop(
		16 * tick, 
		cpp_seq(e, "s1", format_xs(
					  tick, p, 'std::cout<< "hi %s" << std::endl;', {x="10", y=20, z=30}))))

e:start()