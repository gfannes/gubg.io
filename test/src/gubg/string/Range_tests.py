from gubg.string import Range

def test_ctor():
    r = Range("abc")
    print(repr(r))
    print(r)

def test_pop_char_a():
    r = Range("a")
    assert(not r.pop_char("b"))
    assert(r.pop_char() == "a")
    assert(not r.pop_char())

def test_pop_char_abc():
    r = Range("abc")
    assert(r.pop_char("a") == "a")
    assert(not r.pop_char("a"))
    assert(r.pop_char("b"))
    assert(r.pop_char("c"))
    assert(not r.pop_char("d"))

def test_pop_open_close_0():
    r = Range("abc")
    assert(not r.pop_open_close("dc"))
    assert(not r.pop_open_close("ad"))
    assert(r.pop_open_close("ac") == "b")

def test_pop_open_close_1():
    r = Range("(())")
    assert(r.pop_open_close("()") == "()")

def test_pop_strip():
    r = Range("abacaba")
    assert(r.strip("X") == "")
    assert(r.strip("ba") == "aba")
