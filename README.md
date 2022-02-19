# TIPO Programming Language

TIPO is an interpreted functional language to explore a lambda-calculus based
type checking system. TIPO is the DSL that is going to be used as semantic verifier
for Bee and other projects.

The language looks like this:
```
# Function definition
\I x = x
\M x = x x
\K x y = x
\KI x y = y

# Also can be defined as follows
λI x = x
λM x = x x
λK x y = x
λKI x y = y
...

# Type verifications
checkSame (Int) (Int)
checkCallable (Apply Int Int) (Apply Int Float)

# So on ...
```

There is no literal or any other stuff to build programs, it is only targeted to check
the types of other programming languages.

# ADTs and Lambda calculus

The main idea is to define ADTs using this embeddable PL into other projects. 