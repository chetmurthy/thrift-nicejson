import zoo
# In zoo.cpp we expose hello() function, and it now exists in the zoo module.
assert 'echo' in dir(zoo)
# zoo.hello is a callable.
assert callable(zoo.echo)
# Call the C++ hello() function from Python.
print zoo.echo("world")
