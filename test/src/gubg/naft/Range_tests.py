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

def test_pop_node__attrs_ok():
    test_cases = {
            "[]()": {},
            "[](a)": {"a":None},
            "[](a:b)": {"a":"b"},
            "[](:b)": {"":"b"},
            "[](a:c)": {"a":"c"},
            "[](a:b:c)": {"a":"b:c"},
            "[](a:b)(c:d)": {"a":"b", "c":"d"},
            }
    for naft, wanted in test_cases.items():
        r = Range(naft)
        r.pop_node()
        a = r.pop_attrs()
        assert(a == wanted)

def test_pop_node_block_ok():
    test_cases = {
            "[]": None,
            "[]{}": "",
            "[]{ }": "",
            "[]{ \t\n\t }": "",
            }
    for naft, wanted in test_cases.items():
        r = Range(naft)
        r.pop_node()
        b = r.pop_block()
        if b is None:
            assert(wanted is None)
        else:
            assert(str(b) == wanted)
