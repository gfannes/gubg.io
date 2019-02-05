class Range:
    def __init__(self, str):
        self.str = str
        self.begin = 0
        self.end = len(str)

    def __repr__(self):
        return f"[string:Range](b:{self.begin})(e:{self.end}){{{self.str}}}"

    def __str__(self):
        return self.str[self.begin:self.end]

    def save(self):
        sp = Range(self.str)
        sp.begin = self.begin
        sp.end = self.end
        return sp

    def restore(self, sp):
        self.str = sp.str
        self.begin = sp.begin
        self.end = sp.end

    def empty(self):
        return self.begin == self.end

    def size(self):
        return self.end - self.begin

    def pop_count(self, nr):
        size = self.size()
        if nr > size:
            nr = size
        self.begin += nr

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
        sp = self.save()

        o, c = oc[0], oc[1]

        if not self.pop_char(o):
            self.restore(sp)
            return None
        level = 1

        res = None
        for ix in range(self.begin, self.end):
            ch = self.str[ix]
            if ch == o:
                level += 1
            elif ch == c:
                level -= 1
                if level == 0:
                    res = self.str[self.begin:ix]
                    self.begin = ix+1
                    break

        if res is None:
            self.restore(sp)
        return res

    def strip(self, needles):
        if self.empty():
            return False

        def is_needle(pos):
            if pos >= self.end:
                return False
            if needles.find(self.str[pos]) < 0:
                return False
            return True

        pos = self.begin
        while is_needle(pos):
            pos += 1

        begin = self.begin
        self.pop_count(pos-begin)

        return self.str[begin:pos]
