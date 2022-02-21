# TIPO Programming Language

TIPO is an interpreted functional language to explore a lambda-calculus based
type checking system. TIPO is the DSL that is going to be used as semantic verifier
for Bee and other projects.

The language looks like this:
```
# Binding
a = b

# Function Application
a b

# Lambda definition
x => x      # I
x => x x    # M
x y => x    # K
x y => y    # KI

# Function binding
f = x => x
f x

# Shorthand for function binding
f x = x
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