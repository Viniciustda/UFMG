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

class EvalVisitor(Visitor):
    """
    The EvalVisitor class evaluates logical and arithmetic expressions. The
    result of evaluating an expression is the value of that expression. The
    inherited attribute propagated throughout visits is the environment that
    associates the names of variables with values.

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
    def visit_var(self, var, arg):
        if var.identifier not in arg:
            raise ValueError(f"Variavel inexistente {var.identifier}")
        return arg[var.identifier]

    def visit_bln(self, bln, arg):
        return bln.bln

    def visit_num(self, num, arg):
        return num.num

    def visit_eql(self, eql, arg):
        return eql.left.accept(self, arg) == eql.right.accept(self, arg)

    def visit_add(self, add, arg):
        return add.left.accept(self, arg) + add.right.accept(self, arg)

    def visit_sub(self, sub, arg):
        return sub.left.accept(self, arg) - sub.right.accept(self, arg)

    def visit_mul(self, mul, arg):
        return mul.left.accept(self, arg) * mul.right.accept(self, arg)

    def visit_div(self, div, arg):
        return div.left.accept(self, arg) // div.right.accept(self, arg)

    def visit_leq(self, leq, arg):
        return leq.left.accept(self, arg) <= leq.right.accept(self, arg)

    def visit_lth(self, lth, arg):
        return lth.left.accept(self, arg) < lth.right.accept(self, arg)

    def visit_neg(self, neg, arg):
        return -neg.exp.accept(self, arg)

    def visit_not(self, not_expr, arg):
        return not not_expr.exp.accept(self, arg)

    def visit_let(self, let, arg):
        local_env = arg.copy()
        local_env[let.identifier] = let.exp_def.accept(self, arg)
        return let.exp_body.accept(self, local_env)

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
