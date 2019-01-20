from gubg.naft import Range

def test_ctor():
    r = Range("abc")
    print(repr(r))
    print(r)
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

