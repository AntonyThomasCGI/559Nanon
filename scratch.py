from maya import cmds

sel = cmds.ls(sl=True)




class Test:
    def __init__(self, x, y):
        self.x = x
        self.y = y


t = Test()




print("hi", sep=5)
