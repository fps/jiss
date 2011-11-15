require "jissing"

e = jiss.engine()

-- setup some state that the sequences later use
e:run([[
	bar = 0; 
	min = 20;
	max = 80;
	stella = {
		range(min, 80, min7b5(E(4))), 
		range(min, 80, min7b5(E(4))), 

		range(min, 80, dom7b9b13(A(4))),
		range(min, 80, dom7b9b13(A(4))),

		range(min, 80, min7(C(4))), 
		range(min, 80, min7(C(4))), 

		range(min, 80, dom7(F(4))),
		range(min, 80, dom7(F(4))),

		range(min, 80, min7(F(4))),
		range(min, 80, min7(F(4))),

		range(min, 80, dom7(B(4)-1)),
		range(min, 80, dom7(B(4)-1)),

		range(min, 80, maj7(E(4)-1)),
		range(min, 80, maj7(E(4)-1)),

		range(min, 80, dom7s11(A(4)-1)),
		range(min, 80, dom7s11(A(4)-1)),

		range(min, 80, maj7(B(4)-1)),
		range(min, 80, maj7(B(4)-1)),

		range(min, 80, min7b5(E(4))), 
		range(min, 80, dom7b9b13(A(4))),

		range(min, 80, min7(D(4))),
		range(min, 80, min7(D(4))),

		range(min, 80, min7(B(4)-1)),
		range(min, 80, min7(B(4)-1)),

		range(min, 80, maj7(F(4))),
		range(min, 80, maj7(F(4))),

		range(min, 80, min7b5(E(4))), 
		range(min, 80, dom7b9b13(A(4))),

		range(min, 80, min7b5(A(4))), 
		range(min, 80, min7b5(A(4))), 

		range(min, 80, dom7(D(4))),
		range(min, 80, dom7(D(4))),

		range(min, 80, dom7b9b13(G(4))),
		range(min, 80, dom7b9b13(G(4))),

		range(min, 80, dom7b9b13(G(4))),
		range(min, 80, dom7b9b13(G(4))),

		range(min, 80, min7(C(4))),
		range(min, 80, min7(C(4))),

		range(min, 80, min7(C(4))),
		range(min, 80, min7(C(4))),

		range(min, 80, dom7s11(A(4)-1)),
		range(min, 80, dom7s11(A(4)-1)),

		range(min, 80, dom7s11(A(4)-1)),
		range(min, 80, dom7s11(A(4)-1)),

		range(min, 80, maj7(B(4)-1)),
		range(min, 80, maj7(B(4)-1)),

		range(min, 80, maj7(B(4)-1)),
		range(min, 80, maj7(B(4)-1)),

		range(min, 80, min7b5(E(4))), 
		range(min, 80, min7b5(E(4))), 

		range(min, 80, dom7b9b13(A(4))),
		range(min, 80, dom7b9b13(A(4))),

		range(min, 80, min7b5(D(4))), 
		range(min, 80, min7b5(D(4))), 

		range(min, 80, dom7b9b13(G(4))),
		range(min, 80, dom7b9b13(G(4))),

		range(min, 80, min7b5(C(4))), 
		range(min, 80, min7b5(C(4))), 

		range(min, 80, dom7b9b13(F(4))),
		range(min, 80, dom7b9b13(F(4))),

		range(min, 80, maj7s11(B(4)-1)),
		range(min, 80, maj7s11(B(4)-1)),

		range(min, 80, maj7s11(B(4)-1)),
		range(min, 80, maj7s11(B(4)-1))
	}
]])

-- this sequence can control the others since it's processed before
-- the others in the engine
tune = loop(8, lua_seq(e, "tune", lines(1, 
[[
	drums1:relocate(0.0); drums1:start_(); notes:relocate(0.0); notes:start_()





	drums1:stop_()
]])))

tune:start()
e:append(tune)

-- a sequence that controls the global variable bar to advance through the song
play(loop(1, lua_seq(e, "control", lines(1, [[
	bar = bar + 1; bar = (bar % #stella);
]]))))

play(loop(0.75, lua_seq(e, "notes", {
	{ 0.125, [[ for i = 1,4 do note_on(0, 24 + stella[bar][math.random(#stella[bar])], 30 + math.random()*64) end	]]	},
	{ 0.5,   [[ for i = 1,2 do note_on(0, 24 + stella[bar][math.random(#stella[bar])], 10 + math.random()*34) end ]] }
})))

drums = [[
	note_on(1, 64, 127); note_on(2, 64, 127) 
	note_on(2, 64, 127)
	note_on(2, 64, math.random(127))
	note_on(2, 64, math.random(127)) 
	note_on(2, 42, 110)
	note_on(2, 64, 127) 
	note_on(2, 64, math.random(127))
	note_on(1, 64, 127); note_on(2, 64, 127) 
	note_on(2, 64, math.random(127))
	note_on(2, 64, math.random(127)) 
	note_on(2, 32, 127); note_on(2, 64, math.random(127)) 
	note_on(2, 64, math.random(127)) 
	note_on(2, 64, math.random(127))
	note_on(2, 64, math.random(127))
	note_on(2, 64, math.random(127)) 
	note_on(1, 32, 127); 
	note_on(2, 64, math.random(127)) 
]]

play(loop(2, lua_seq(e, "drums1", lines(1/8, drums))))
-- play_events(e, "drums2", loop_events(1, events_string(0.125, drums)))


bass = [[
	note_on(3, 48 + stella[bar][math.random(#stella[bar])] % 12, 127)






	note_on(3, 48 + stella[bar][math.random(#stella[bar])] % 12, 127)
]]


play(loop(4, lua_seq(e, "bass", lines(1/8, bass))))

connect(e,"jass:in")
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
