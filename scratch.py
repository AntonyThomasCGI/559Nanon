
import re

from maya import cmds

sel = cmds.ls(sl=True)


class Test:
    """A test class"""
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def sum(self):
        """A docstring"""
        return self.x + self.y


# a comment

reg = re.compile(r"$|\btest\b")


def a_fn():
    return "test"


t = Test(2, 3)
t.x

r = a_fn()


print("hi", sep=" ")
