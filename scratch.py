"""A test script that does nothing"""

import re

from maya import cmds


# a comment
sel = cmds.ls(sl=True)


class Test:
    """A test class"""
    def __init__(self, x=1, y=2):
        self.x = x
        self.y = y

    def sum(self):
        """A docstring"""
        return self.x + self.y


def a_fn():
    return "test"


reg = re.compile(r"$|\btest\b")

t = Test(2, 3)
t.x

r = a_fn()


print("hi", sep=" ")
