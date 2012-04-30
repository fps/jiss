
-- this document does not contain working examples, but rather
-- code snippets that I want one day to work :D

require "jissin"

-- a simple four to the floor pattern with some small variations
p1 = map_horizontal(0.125, {
	[[note_on(0, GM_open_snare, 127)]], 
	[[note_on(0, GM_open_hihat, 127)]], 
	[[note_on(0, GM_closed_hihat, 127)]], 
	[[note_on(0, GM_bass_drum, 127)]]
}, 	
[[
    x       x       x       x
  x   x   x   x   x   x   X   x
x   xx  x  xxx  x   xx  xxx x    
x   x   x   x x x   x   xx  x   x
]]
)

bossa_drums = 
[[
xxxxxxxxxxxxxxxx
  x  x  x  x  x
x  xx  xx  xx  x
]]

-- here time flows from top to bottom
tracker_style = map_vertical([[note_on(0, %1, 127)]],
[[
C4	Eb4	G4	Bb4


C4	Eb4	G4	Bb4


C4	Eb4	G4	Bb4


]]

