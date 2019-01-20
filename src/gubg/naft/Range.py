from gubg.string import Range as Strange

class Range:
    def __init__(self, str):
        self.strange = Strange(str)
    def pop_node(self):
        return self.strange.pop_open_close("[]")
    def __repr__(self):
        return f"[naft:Range]{{{str(self.strange)}}}"
    def __str__(self):
        return str(self.strange)
