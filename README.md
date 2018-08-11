
# yog-lang

**yog** is an imperative programming language that i'm working on in order to learn the construction of compilers and interpreters.  

## code example

```
# the variables declaration #
var
	a : int; # the first number  #
	b : int; # the second number #
	c : int; # the third number  #

# the beginning of the program #
begin
	# take the two operands #
	read a;
	read b;

	# compute the sum #
	c := a + b;

	# write the result #
	write c;

end # the end of the program #
```

## installation

```
git clone https://github.com/loreloc/yog-lang
cd yog-lang
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```

