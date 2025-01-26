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
