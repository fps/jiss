require "jiss"
require "jissing"

e = jiss.engine()

c = cpp_event("s.relocate(0.0);")

s = jiss.sequence(e)
s:insert(0.0, jiss.lua_event("print(\"hi\")"))
s:insert_cpp_event(1.0, c)
s:start()

e:append(s)
e:start()

io.stdin:read'*l'
