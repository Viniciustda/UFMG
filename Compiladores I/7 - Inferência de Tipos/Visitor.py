import sys
from abc import ABC, abstractmethod
from Expression import *


class Visitor(ABC):
    """
    The visitor pattern consists of two abstract classes: the Expression and the
    Visitor. The Expression class defines on method: 'accept(visitor, args)'.
    This method takes in an implementation of a visitor, and the arguments that
    are passed from expression to expression. The Visitor class defines one
    specific method for each subclass of Expression. Each instance of such a
    subclasse will invoke the right visiting method.
    """

    @abstractmethod
    def visit_var(self, exp, arg):
        pass

    @abstractmethod
    def visit_bln(self, exp, arg):
        pass

    @abstractmethod
    def visit_num(self, exp, arg):
        pass

    @abstractmethod
    def visit_eql(self, exp, arg):
        pass

    @abstractmethod
    def visit_and(self, exp, arg):
        pass

    @abstractmethod
    def visit_or(self, exp, arg):
        pass

    @abstractmethod
    def visit_add(self, exp, arg):
        pass

    @abstractmethod
    def visit_sub(self, exp, arg):
        pass

    @abstractmethod
    def visit_mul(self, exp, arg):
        pass

    @abstractmethod
    def visit_div(self, exp, arg):
        pass

    @abstractmethod
    def visit_leq(self, exp, arg):
        pass

    @abstractmethod
    def visit_lth(self, exp, arg):
        pass

    @abstractmethod
    def visit_neg(self, exp, arg):
        pass

    @abstractmethod
    def visit_not(self, exp, arg):
        pass

    @abstractmethod
    def visit_let(self, exp, arg):
        pass

    @abstractmethod
    def visit_ifThenElse(self, exp, arg):
        pass

class EvalVisitor(Visitor):
    """
    The EvalVisitor class evaluates logical and arithmetic expressions. The
    result of evaluating an expression is the value of that expression. The
    inherited attribute propagated throughout visits is the environment that
    associates the names of variables with values.
    
    Notice that this implementation must perform type verification. If some
    verification fail, then it invokes sys.exit with the correct error
    message. We expect two different messages:
    
    1. sys.exit("Type error")
    2. sys.exit("Def error")

    Examples:
    >>> e0 = Let('v', Add(Num(40), Num(2)), Mul(Var('v'), Var('v')))
    >>> e1 = Not(Eql(e0, Num(1764)))
    >>> ev = EvalVisitor()
    >>> e1.accept(ev, {})
    False

    >>> e0 = Let('v', Add(Num(40), Num(2)), Sub(Var('v'), Num(2)))
    >>> e1 = Lth(e0, Var('x'))
    >>> ev = EvalVisitor()
    >>> e1.accept(ev, {'x': 41})
    True
    """
    def visit_var(self, exp, env):
        if exp.identifier not in env:
            sys.exit("Def error")
        return env[exp.identifier]

    def visit_bln(self, exp, env):
        return exp.bln

    def visit_num(self, exp, env):
        return exp.num

    def visit_eql(self, exp, env):
        left = exp.left.accept(self, env)
        right = exp.right.accept(self, env)
        if type(left) == type(right):
            return left == right
        sys.exit("Type error")

    def visit_and(self, exp, env):
        left = exp.left.accept(self, env)
        if not isinstance(left, bool):
            sys.exit("Type error")
        if not left:
            return False

        right = exp.right.accept(self, env)
        if not isinstance(right, bool):
            sys.exit("Type error")
        return right

    def visit_or(self, exp, env):
        left = exp.left.accept(self, env)
        if not isinstance(left, bool):
            sys.exit("Type error")
        if left:
            return True

        right = exp.right.accept(self, env)
        if not isinstance(right, bool):
            sys.exit("Type error")
        return right


    def visit_add(self, exp, env):
        left = exp.left.accept(self, env)
        right = exp.right.accept(self, env)
        if isinstance(left, int) and isinstance(right, int):
            return left + right
        sys.exit("Type error")

    def visit_sub(self, exp, env):
        left = exp.left.accept(self, env)
        right = exp.right.accept(self, env)
        if isinstance(left, int) and isinstance(right, int):
            return left - right
        sys.exit("Type error")

    def visit_mul(self, exp, env):
        left = exp.left.accept(self, env)
        right = exp.right.accept(self, env)
        if isinstance(left, int) and isinstance(right, int):
            return left * right
        sys.exit("Type error")

    def visit_div(self, exp, env):
        left = exp.left.accept(self, env)
        right = exp.right.accept(self, env)
        if isinstance(left, int) and isinstance(right, int):
            if right == 0:
                sys.exit("Type error: division by zero")
            return left // right
        sys.exit("Type error")

    def visit_leq(self, exp, env):
        left = exp.left.accept(self, env)
        right = exp.right.accept(self, env)
        if isinstance(left, int) and isinstance(right, int):
            return left <= right
        sys.exit("Type error")

    def visit_lth(self, exp, env):
        left = exp.left.accept(self, env)
        right = exp.right.accept(self, env)
        if isinstance(left, int) and isinstance(right, int):
            if not (isinstance(left, bool) or isinstance(right, bool)):
                return left < right
        sys.exit("Type error")

    def visit_neg(self, exp, env):
        val = exp.exp.accept(self, env)
        if isinstance(val, int):
            return -val
        sys.exit("Type error")

    def visit_not(self, exp, env):
        val = exp.exp.accept(self, env)
        if isinstance(val, bool):
            return not val
        sys.exit("Type error")

    def visit_let(self, exp, env):
        local_env = env.copy()
        local_env[exp.identifier] = exp.exp_def.accept(self, env)
        return exp.exp_body.accept(self, local_env)
        
    def visit_ifThenElse(self, exp, env):
        condition_value = exp.cond.accept(self, env)
        if isinstance(condition_value, bool):
            if condition_value:
                return exp.e0.accept(self, env)  # Avalia apenas o ramo 'then'
            else:
                return exp.e1.accept(self, env)  # Avalia apenas o ramo 'else'
        sys.exit("Type error")
        
class UseDefVisitor(Visitor):
    """
    The UseDefVisitor class reports the use of undefined variables. It takes
    as input an environment of defined variables, and produces, as output,
    the set of all the variables that are used without being defined.

    Examples:
    >>> e0 = Let('v', Add(Num(40), Num(2)), Mul(Var('v'), Var('v')))
    >>> e1 = Not(Eql(e0, Num(1764)))
    >>> ev = UseDefVisitor()
    >>> len(e1.accept(ev, set()))
    0

    >>> e0 = Let('v', Add(Num(40), Num(2)), Sub(Var('v'), Num(2)))
    >>> e1 = Lth(e0, Var('x'))
    >>> ev = UseDefVisitor()
    >>> len(e1.accept(ev, set()))
    1

    >>> e = Let('v', Add(Num(40), Var('v')), Sub(Var('v'), Num(2)))
    >>> ev = UseDefVisitor()
    >>> len(e.accept(ev, set()))
    1

    >>> e1 = Let('v', Add(Num(40), Var('v')), Sub(Var('v'), Num(2)))
    >>> e0 = Let('v', Num(3), e1)
    >>> ev = UseDefVisitor()
    >>> len(e0.accept(ev, set()))
    0
    """
    def visit_var(self, exp, defined):
        if exp.identifier not in defined:
            return {exp.identifier}
        return set()
    
    def visit_bln(self, exp, defined):
        return set()
    
    def visit_num(self, exp, defined):
        return set()
    
    def visit_eql(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)
    
    def visit_add(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)
    
    def visit_sub(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)
    
    def visit_mul(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)
    
    def visit_div(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)
    
    def visit_leq(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)
    
    def visit_lth(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)
    
    def visit_neg(self, exp, defined):
        return exp.exp.accept(self, defined)
    
    def visit_not(self, exp, defined):
        return exp.exp.accept(self, defined)
    
    def visit_let(self, exp, defined):
        inner_defined = defined | {exp.identifier}
        exp_def_result = exp.exp_def.accept(self, defined)
        exp_body_result = exp.exp_body.accept(self, inner_defined)
        return exp_def_result | exp_body_result
    
    def visit_and(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)

    def visit_or(self, exp, defined):
        return exp.left.accept(self, defined) | exp.right.accept(self, defined)

    def visit_if_then_else(self, exp, defined):
        condition_undef = exp.cond.accept(self, defined)
        then_undef = exp.e0.accept(self, defined)
        else_undef = exp.e1.accept(self, defined)
        return condition_undef | then_undef | else_undef

def safe_eval(exp):
    """
    This method applies one simple semantic analysis onto an expression, before
    evaluating it: it checks if the expression contains free variables, there
    is, variables used without being defined.

    Example:
    >>> e0 = Let('v', Add(Num(40), Num(2)), Mul(Var('v'), Var('v')))
    >>> e1 = Not(Eql(e0, Num(1764)))
    >>> safe_eval(e1)
    Value is False

    >>> e0 = Let('v', Add(Num(40), Num(2)), Sub(Var('v'), Num(2)))
    >>> e1 = Lth(e0, Var('x'))
    >>> safe_eval(e1)
    Error: expression contains undefined variables.
    """
    ev = EvalVisitor()
    uv = UseDefVisitor()
    undefined_vars = exp.accept(uv, set())
    if undefined_vars:
        print("Error: expression contains undefined variables.")
    else:
        try:
            result = exp.accept(ev, {})
            print(f"Value is {result}")
        except ValueError as e:
            print(e)


class CtrGenVisitor(Visitor):
    """
    This visitor creates constraints for a type-inference engine. Basically,
    it traverses the abstract-syntax tree of expressions, producing pairs like
    (type0, type1) on the way. A pair like (type0, type1) indicates that these
    two type variables are the same.

    Examples:
        >>> e = Let('v', Num(40), Let('w', Num(2), Add(Var('v'), Var('w'))))
        >>> ev = CtrGenVisitor()
        >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
        ["('TV_1', 'TV_2')", "('TV_2', 'TV_3')", "('v', <class 'int'>)", "('w', <class 'int'>)", "(<class 'int'>, 'TV_3')", "(<class 'int'>, 'v')", "(<class 'int'>, 'w')"]
    """

    def __init__(self):
        self.fresh_type_counter = 0

    def fresh_type_var(self):
        """
        Create a new type var using the current value of the fresh_type_counter.
        Two successive calls to this method will return different type names.
        Notice that the name of a type variable is always TV_x, where x is
        some integer number. That means that probably we would run into
        errors if someone declares a variable called, say, TV_1 or TV_2, as in
        "let TV_1 <- 1 in TV_1 end". But you can assume that such would never
        happen in the test cases. In practice, we should define a new class
        to represent type variables. But let's keep the implementation as
        simple as possible.

        Example:
            >>> ev = CtrGenVisitor()
            >>> [ev.fresh_type_var(), ev.fresh_type_var()]
            ['TV_1', 'TV_2']
        """
        self.fresh_type_counter += 1
        return f"TV_{self.fresh_type_counter}"

    """
    The CtrGenVisitor class creates constraints that, once solved, will give
    us the type of the different variables. Every accept method takes in
    two arguments (in addition to self):
    
    exp: is the expression that is being analyzed.
    type_var: that is a name that works as a placeholder for the type of the
    expression. Whenever we visit a new expression, we create a type variable
    to represent its type (you can do that with the method fresh_type_var).
    The only exception is the type of Var expressions. In this case, the type
    of a Var expression is the identifier of that expression.
    """

    def visit_var(self, exp, type_var):
        """
        Example:
            >>> e = Var('v')
            >>> ev = CtrGenVisitor()
            >>> e.accept(ev, ev.fresh_type_var())
            {('v', 'TV_1')}
        """
        return {(exp.identifier, type_var)}

    def visit_bln(self, exp, type_var):
        """
        Example:
            >>> e = Bln(True)
            >>> ev = CtrGenVisitor()
            >>> e.accept(ev, ev.fresh_type_var())
            {(<class 'bool'>, 'TV_1')}
        """
        return {(type(True), type_var)}

    def visit_num(self, exp, type_var):
        """
        Example:
            >>> e = Num(1)
            >>> ev = CtrGenVisitor()
            >>> e.accept(ev, ev.fresh_type_var())
            {(<class 'int'>, 'TV_1')}
        """
        return {(type(1), type_var)}

    def visit_eql(self, exp, type_var):
        """
        Example:
            >>> e = Eql(Num(1), Bln(True))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'bool'>, 'TV_1')", "(<class 'bool'>, 'TV_2')", "(<class 'int'>, 'TV_2')"]

        Notice that if we have repeated constraints, they only appear once in
        the set of constraints (after all, it's a set!). As an example, we
        would have two occurrences of the pair (TV_2, int) in the following
        example:
            >>> e = Eql(Num(1), Num(2))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'bool'>, 'TV_1')", "(<class 'int'>, 'TV_2')"]
        """
        # TODO: Implement this method!
        left_type = self.fresh_type_var()
        right_type = self.fresh_type_var()
        return {(bool, type_var), (left_type, right_type)} | exp.left.accept(self, left_type) | exp.right.accept(self, right_type)

    def visit_and(self, exp, type_var):
        """
        Example:
            >>> e = And(Bln(False), Bln(True))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'bool'>, 'TV_1')", "(<class 'bool'>, <class 'bool'>)"]

        In the above example, notice that we ended up getting a trivial
        constraint, e.g.: (<class 'bool'>, <class 'bool'>). That's alright:
        don't worry about these trivial constraints at this point. We can
        remove them from the set of constraints later on, when we try to
        solve them.
        """
        # TODO: Implement this method!
        bool_type = bool
        return {(bool_type, type_var)} | exp.left.accept(self, bool_type) | exp.right.accept(self, bool_type)

    def visit_or(self, exp, type_var):
        """
        Example:
            >>> e = Or(Bln(False), Bln(True))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'bool'>, 'TV_1')", "(<class 'bool'>, <class 'bool'>)"]
        """
        # TODO: Implement this method!
        bool_type = bool
        return {(bool_type, type_var)} | exp.left.accept(self, bool_type) | exp.right.accept(self, bool_type)

    def visit_add(self, exp, type_var):
        """
        Example:
            >>> e = Add(Num(1), Num(2))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'int'>, 'TV_1')", "(<class 'int'>, <class 'int'>)"]
        """
        # TODO: Implement this method!
        int_type = int
        return {(int_type, type_var)} | exp.left.accept(self, int_type) | exp.right.accept(self, int_type)

    def visit_sub(self, exp, type_var):
        """
        Example:
            >>> e = Sub(Num(1), Num(2))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'int'>, 'TV_1')", "(<class 'int'>, <class 'int'>)"]
        """
        # TODO: Implement this method!
        int_type = int
        return {(int_type, type_var)} | exp.left.accept(self, int_type) | exp.right.accept(self, int_type)

    def visit_mul(self, exp, type_var):
        """
        Example:
            >>> e = Mul(Num(1), Num(2))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'int'>, 'TV_1')", "(<class 'int'>, <class 'int'>)"]
        """
        int_type = int
        return {(int_type, type_var)} | exp.left.accept(self, int_type) | exp.right.accept(self, int_type)

    def visit_div(self, exp, type_var):
        """
        Example:
            >>> e = Div(Num(1), Num(2))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'int'>, 'TV_1')", "(<class 'int'>, <class 'int'>)"]
        """
        # TODO: Implement this method!
        int_type = int
        return {(int_type, type_var)} | exp.left.accept(self, int_type) | exp.right.accept(self, int_type)

    def visit_leq(self, exp, type_var):
        """
        Example:
            >>> e = Leq(Num(1), Num(2))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'bool'>, 'TV_1')", "(<class 'int'>, <class 'int'>)"]
        """
        # TODO: Implement this method!
        int_type = int
        return {(bool, type_var), (int_type, int_type)} | exp.left.accept(self, int_type) | exp.right.accept(self, int_type)

    def visit_lth(self, exp, type_var):
        """
        Example:
            >>> e = Lth(Num(1), Num(2))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'bool'>, 'TV_1')", "(<class 'int'>, <class 'int'>)"]
        """
        # TODO: Implement this method!
        int_type = int
        return {(bool, type_var), (int_type, int_type)} | exp.left.accept(self, int_type) | exp.right.accept(self, int_type)

    def visit_neg(self, exp, type_var):
        """
        Example:
            >>> e = Neg(Num(1))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'int'>, 'TV_1')", "(<class 'int'>, <class 'int'>)"]
        """
        # TODO: Implement this method!
        int_type = int
        return {(int_type, type_var)} | exp.exp.accept(self, int_type)

    def visit_not(self, exp, type_var):
        """
        Example:
            >>> e = Not(Bln(True))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["(<class 'bool'>, 'TV_1')", "(<class 'bool'>, <class 'bool'>)"]
        """
        # TODO: Implement this method!
        bool_type = bool
        return {(bool_type, type_var)} | exp.exp.accept(self, bool_type)

    def visit_let(self, exp, type_var):
        """
        Example:
            >>> e = Let('v', Num(42), Var('v'))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["('TV_1', 'TV_2')", "('v', 'TV_2')", "(<class 'int'>, 'v')"]
        """
        # TODO: Implement this method!
        id_type = exp.identifier
        body_type = self.fresh_type_var()
        return exp.exp_def.accept(self, id_type) | exp.exp_body.accept(self, body_type) | {(type_var, body_type)}

    def visit_ifThenElse(self, exp, type_var):
        """
        Example:
            >>> e = IfThenElse(Bln(True), Num(42), Num(30))
            >>> ev = CtrGenVisitor()
            >>> sorted([str(ct) for ct in e.accept(ev, ev.fresh_type_var())])
            ["('TV_1', 'TV_2')", "(<class 'bool'>, <class 'bool'>)", "(<class 'int'>, 'TV_2')"]
        """
        # TODO: Implement this method!
        cond_type = bool
        then_type = self.fresh_type_var()
        else_type = self.fresh_type_var()
        return {(type_var, then_type), (then_type, else_type)} | exp.cond.accept(self, cond_type) | exp.e0.accept(self, then_type) | exp.e1.accept(self, else_type)