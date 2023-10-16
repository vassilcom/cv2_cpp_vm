
print("hello from test.py")

def magic (x):
    print (x)

def doubling (x):
    print ("x*2")
    return x*2

# module that is defind in c++
import embeddedmodule
embeddedmodule.say_something()

# can be also with typdef: def magic2 (my_data : embeddedmodule.MyData):
def magic2 (my_data ):
    print (my_data.x)
