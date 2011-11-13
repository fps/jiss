require "jiss"
require "jissing"

e = jiss.engine()

function g()
        print("g")
        s:relocate(0.0)
end

h = (string.dump(g):gsub(".", function(character) return "\\" .. character:byte() end))

play_events(e, "g", {{0.1, "loadstring(\"" .. h .. "\")()"}})

play_sequence(e, loop_sequence(2.0, midi_sequence(e, "midi", 0.2, {1, 2, 3, 4, 5, 6})))

e:start()

io.stdin:read'*l'

