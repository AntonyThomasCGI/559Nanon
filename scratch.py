

print('this is a test')


def fn(a, b, limit=10, count=0):
    """This is an example fibonachi function."""
    if count >= limit:
        return

    c = a + b
    print(f"{c} (iteration={count + 1})")
    fn(b, c, limit=limit, count=count + 1)


fn(0, 1)


if True:
    pass
else:
    pass


