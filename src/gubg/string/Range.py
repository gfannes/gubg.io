class Range:
    def __init__(self, str):
        self.str = str
        self.begin = 0
        self.end = len(str)
    def save(self):
        sp = Range(self.str)
        sp.begin = self.begin
        sp.end = self.end
        return sp
    def restore(self, sp):
        self.str = sp.str
        self.begin = sp.begin
        self.end = sp.end
    def pop_char(self, wanted_char=None):
        if self.begin >= self.end:
            return None
        char = self.str[self.begin]
        if not wanted_char:
            pass
        elif char == wanted_char:
            pass
        else:
            return None
        self.begin += 1
        return char
    def pop_until(self, needle):
        ix = self.str.find(needle, self.begin, self.end)
        if ix < 0:
            return None
        res = self.str[self.begin:ix]
        self.begin = ix+len(needle)
        return res
    def pop_open_close(self, oc):
        o, c = oc[0], oc[1]
        sp = self.save()
        if not self.pop_char(o):
            self.restore(sp)
            return None
        res = self.pop_until(c)
        if not res:
            self.restore(sp)
            return None
        return res
    def __repr__(self):
        return f"[string:Range](b:{self.begin})(e:{self.end}){{{self.str}}}"
    def __str__(self):
        return self.str[self.begin:self.end]
