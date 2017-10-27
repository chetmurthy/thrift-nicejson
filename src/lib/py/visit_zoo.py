import libzoo
# In zoo.cpp we expose hello() function, and it now exists in the zoo module.
assert 'echo' in dir(libzoo)
# zoo.hello is a callable.
assert callable(libzoo.echo)
# Call the C++ hello() function from Python.
print libzoo.echo("world")
