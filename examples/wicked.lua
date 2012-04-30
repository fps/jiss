require "jissing"

e = jiss.engine()

-- setup some state that the sequences later use
e:run([[
	bar = 0; 
	min = 50;
	max = 60;
	stella = {
		range(min, max, min7b5(E(4))), 
		range(min, max, min7b5(E(4))), 

		range(min, max, dom7b9b13(A(4))),
		range(min, max, dom7b9b13(A(4))),

		range(min, max, min7(C(4))), 
		range(min, max, min7(C(4))), 

		range(min, max, dom7(F(4))),
		range(min, max, dom7(F(4))),

		range(min, max, min7(F(4))),
		range(min, max, min7(F(4))),

		range(min, max, dom7(B(4)-1)),
		range(min, max, dom7(B(4)-1)),

		range(min, max, maj7(E(4)-1)),
		range(min, max, maj7(E(4)-1)),

		range(min, max, dom7s11(A(4)-1)),
		range(min, max, dom7s11(A(4)-1)),

		range(min, max, maj7(B(4)-1)),
		range(min, max, maj7(B(4)-1)),

		range(min, max, min7b5(E(4))), 
		range(min, max, dom7b9b13(A(4))),

		range(min, max, min7(D(4))),
		range(min, max, min7(D(4))),

		range(min, max, min7(B(4)-1)),
		range(min, max, min7(B(4)-1)),

		range(min, max, maj7(F(4))),
		range(min, max, maj7(F(4))),

		range(min, max, min7b5(E(4))), 
		range(min, max, dom7b9b13(A(4))),

		range(min, max, min7b5(A(4))), 
		range(min, max, min7b5(A(4))), 

		range(min, max, dom7(D(4))),
		range(min, max, dom7(D(4))),

		range(min, max, dom7b9b13(G(4))),
		range(min, max, dom7b9b13(G(4))),

		range(min, max, dom7b9b13(G(4))),
		range(min, max, dom7b9b13(G(4))),

		range(min, max, min7(C(4))),
		range(min, max, min7(C(4))),

		range(min, max, min7(C(4))),
		range(min, max, min7(C(4))),

		range(min, max, dom7s11(A(4)-1)),
		range(min, max, dom7s11(A(4)-1)),

		range(min, max, dom7s11(A(4)-1)),
		range(min, max, dom7s11(A(4)-1)),

		range(min, max, maj7(B(4)-1)),
		range(min, max, maj7(B(4)-1)),

		range(min, max, maj7(B(4)-1)),
		range(min, max, maj7(B(4)-1)),

		range(min, max, min7b5(E(4))), 
		range(min, max, min7b5(E(4))), 

		range(min, max, dom7b9b13(A(4))),
		range(min, max, dom7b9b13(A(4))),

		range(min, max, min7b5(D(4))), 
		range(min, max, min7b5(D(4))), 

		range(min, max, dom7b9b13(G(4))),
		range(min, max, dom7b9b13(G(4))),

		range(min, max, min7b5(C(4))), 
		range(min, max, min7b5(C(4))), 

		range(min, max, dom7b9b13(F(4))),
		range(min, max, dom7b9b13(F(4))),

		range(min, max, maj7s11(B(4)-1)),
		range(min, max, maj7s11(B(4)-1)),

		range(min, max, maj7s11(B(4)-1)),
		range(min, max, maj7s11(B(4)-1))
	}
]])

-- due to some small unresolved timing issues
-- it's a good practice to reloc all sequences
-- to a common sync point after a while..
play(loop(32, lua_seq(e, "reloc", lines(1, 
[[
	seqs = {tune, control, drums1, notes, bass1, bass2}; for i = 1,#seqs do seqs[i]:relocate(0) end
]]
))))

play(loop(32, lua_seq(e, "onoff",  lines(1, 
[[
	seqs = {tune, drums1, bass1, bass2}; for i = 1,#seqs do if math.random() > 0.4 then seqs[i]:start_() else seqs[i]:stop_() end end
]]
))))

-- every 8th second randomly turn drums off for a bar
play(loop(8, lua_seq(e, "tune", lines(1, 
[[
	drums1:start_();





	if math.random() > 0.6 then drums1:stop_() end
]]
))))


-- a sequence that controls the global variable bar to advance through the song
play(loop(1, lua_seq(e, "control", lines(1, 
[[
	bar = bar + 1; bar = (bar % #stella); 
]]
))))

play(loop(1.25, lua_seq(e, "notes", {
	{ 0.125, [[ for i = 1,2 do vel = 10 + math.random()*12; note_on(0, 36 + stella[bar][math.random(#stella[bar])], vel) end	]]	},
	{ 0.5,   [[ for i = 1,2 do vel = 10 + math.random()*12; note_on(0, 36 + stella[bar][math.random(#stella[bar])], vel) end ]] }
})))

drums = 
[[
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


bass = 
[[
	note_on(3, 48 + stella[bar][math.random(#stella[bar])] % 12, 127)

	--

	note_on(3, 48 + stella[bar][math.random(#stella[bar])] % 12, 127) 

	--

	if (math.random() > 0.9) then note_on(3, 60 + stella[bar][math.random(#stella[bar])] % 12, 127) end
	
	-- 

	-- -- 

	if (math.random() > 0.7) then note_on(3, 60 + stella[bar][math.random(#stella[bar])] % 12, 127) end

	-- --

	-- 

	if (math.random() > 0.6) then note_on(3, 60 + stella[bar][math.random(#stella[bar])] % 12, 127) end

	--

]]

play(loop(4, lua_seq(e, "bass", lines(1/8, bass))))

bass2 =
[[
	if math.random() > 0.4 then  note_on(4, 72 + stella[bar][math.random(#stella[bar])] % 12, 30) end 
]]

play(loop(2.5, lua_seq(e, "bass2", lines(1/8, bass2))))


connect(e,"jass:in")
e:start()

-- wait for the user to press enter
io.stdin:read'*l'
