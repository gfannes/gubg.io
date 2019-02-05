from gubg import string

class Range:
    def __init__(self, str):
        self.strange = string.Range(str)
        self._whitespace = " \t\n"

    def __repr__(self):
        return f"[naft:Range]{{{str(self.strange)}}}"

    def __str__(self):
        return str(self.strange)

    def pop_node(self, wanted=None):
        self.strange.strip(self._whitespace)
        node = self.strange.pop_open_close("[]")
        if wanted is not None:
            return node == wanted
        return node

    def pop_attrs(self):
        attrs = {}
        while True:
            kv = self.strange.pop_open_close("()")
            if not kv:
                break
            ix = kv.find(":")
            if ix < 0:
                k, v = kv, None
            else:
                k, v = kv[:ix], kv[ix+1:]
            attrs[k] = v
        return attrs

    def pop_block(self):
        block = self.strange.pop_open_close("{}")
        if block is None:
            return None
        block = Range(block)
        block.strange.strip(self._whitespace)
        return block
