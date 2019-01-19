from gubg.naft import Range

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
def test_pop_node_ko_0():
    r = Range("node")
    n = r.pop_node()
    assert(n == None)
def test_pop_node_ko_1():
    r = Range("[node")
    n = r.pop_node()
    assert(n == None)
def test_pop_node_ok_0():
    r = Range("[node]")
    n = r.pop_node()
    assert(n == "node")
    assert(str(r) == "")
def test_pop_node_ok_1():
    r = Range("[node]rest")
    n = r.pop_node()
    assert(n == "node")
    assert(str(r) == "rest")

