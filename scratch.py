# Welcome to 559 Nanon!

import functools

# !! Missing the argument scope in this case:
call_me = functools.partial(print, "hi")

call_me()

import functools
from contextlib import contextmanager


class CoolClass:
    def __init__(self, x):
        self.x = x


print('this is a \'great\' test')


@contextmanager
def test_manager(resource):
    try:
        yield "Hi"
    finally:
        print('cleanup')


def fn(a, b, limit=10, count=0):
    """This is an example fibonachi function."""
    if count >= limit:
        return

    c = a + b
    print(f"{c} (iteration={count + 1})")
    fn(b, c, limit=limit, count=count + 1)


with test_manager(None):
    pass

fn(0, 1)

i = 5
while i <= 10:
    i += 1


CONSTANT = True



