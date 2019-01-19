class Range:
    def __init__(self, str):
        self.str = str
        self.begin = 0
        self.end = len(str)
    def pop_node(self):
        if self.str[self.begin] != "[":
            return None
        ix = self.str.find("]", self.begin+1, self.end)
        if ix < 0:
            return None
        n = self.str[self.begin+1:ix]
        self.begin = ix+1
        return n
    def pop_char(self,wanted_char=None):
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
    def __repr__(self):
        return f"[Range](b:{self.begin})(e:{self.end}){{{self.str}}}"
    def __str__(self):
        return self.str[self.begin:self.end]
