require "jissing"




--- create engine
e = jiss.engine()

p1 = [[
        X             X     
  oo  oo    oo  oo  oo    oo
x   x   x x   x   x   x x   
]]

tick = e:get_samplerate()/16

play(
	loop(
		28 * tick, 
		cpp_seq(e, "s1", format_xs(
					  2*tick, p1, 
					  [[
							  int i = %s; 
							  switch(i) { 
								  case 1: s.midi_note_on(0, 64, 127); break;
								  case 2: s.midi_note_on(2, 64, 60); break;
								  case 3: s.midi_note_on(5, 64, 50); break;
								  
								  default: s.midi_note_on(0, 64, 127); 
							  }
					  ]], 
					  {x="1", o="2", X="3"}))))

connect(e, "jass:in")

e:start()
