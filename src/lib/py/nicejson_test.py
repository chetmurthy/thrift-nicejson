import nicejson
# In nicejson.cpp we expose hello() function, and it now exists in the nicejson module.
assert "something_which_throws" in dir(nicejson)
assert 'echo' in dir(nicejson)
# nicejson.hello is a callable.
assert callable(nicejson.something_which_throws)
assert callable(nicejson.echo)
# Call the C++ hello() function from Python.
print nicejson.echo("world")
print nicejson.something_which_throws()
