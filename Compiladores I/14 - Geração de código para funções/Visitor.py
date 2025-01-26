import sys
from abc import ABC, abstractmethod
from Expression import *
import Asm as AsmModule

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

    @abstractmethod
    def visit_fn(self, exp, arg):
        pass

    @abstractmethod
    def visit_app(self, exp, arg):
        pass

class GenVisitor(Visitor):
    """
    The GenVisitor class compiles arithmetic expressions into a low-level
    language.
    """

    def __init__(self):
        self.next_var_counter = 0
        self.next_fn_counter = 0
        self.label_map = {}  # Armazena mapeamento de nomes para endereços

    def next_var_name(self):
        self.next_var_counter += 1
        return f"v{self.next_var_counter}"
    
    def next_fn_label(self):
        self.next_fn_counter += 1
        return f"fn{self.next_fn_counter}"
    
    def map_label_to_address(self, label_name, prog):
        """
        Mapeia um nome de label para o endereço atual no programa.
        """
        self.label_map[label_name] = prog.get_number_of_instructions()

    def get_label_address(self, label_name):
        """
        Recupera o endereço associado a um label.
        """
        if label_name not in self.label_map:
            raise ValueError(f"Label {label_name} não foi mapeado para nenhum endereço.")
        return self.label_map[label_name]

    def visit_var(self, exp, prog):
        """
        Usage:
            >>> e = Var('x')
            >>> p = AsmModule.Program({"x":1}, [])
            >>> g = GenVisitor()
            >>> v = e.accept(g, p)
            >>> p.eval()
            >>> p.get_val(v)
            1
        """
        return exp.identifier

    def visit_bln(self, exp, prog):
        """
        Usage:
            >>> e = Bln(True)
            >>> p = AsmModule.Program({}, [])
            >>> g = GenVisitor()
            >>> v = e.accept(g, p)
            >>> p.eval()
            >>> p.get_val(v)
            1

            >>> e = Bln(False)
            >>> p = AsmModule.Program({}, [])
            >>> g = GenVisitor()
            >>> v = e.accept(g, p)
            >>> p.eval()
            >>> p.get_val(v)
            0
        """
        v_name = self.next_var_name()
        prog.set_val(v_name, 1 if exp.bln else 0)
        return v_name

    def visit_num(self, exp, prog):
        """
        Usage:
            >>> e = Num(13)
            >>> p = AsmModule.Program({}, [])
            >>> g = GenVisitor()
            >>> v = e.accept(g, p)
            >>> p.eval()
            >>> p.get_val(v)
            13
        """
        v_name = self.next_var_name()
        prog.set_val(v_name, int(exp.num))
        return v_name

    def visit_eql(self, exp, prog):
        """
        >>> e = Eql(Num(13), Num(13))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Eql(Num(13), Num(10))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Eql(Num(-1), Num(1))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0
        """
        l_name = str(exp.left.accept(self, prog))
        r_name = str(exp.right.accept(self, prog))

        temp_sub = self.next_var_name()
        temp_slt1 = self.next_var_name()
        temp_slt2 = self.next_var_name()
        v_name = self.next_var_name()

        # temp_sub = l_name - r_name
        prog.add_inst(AsmModule.Sub(temp_sub, l_name, r_name))

        # temp_slt1 = (temp_sub < 0) ? 1 : 0
        prog.add_inst(AsmModule.Slti(temp_slt1, temp_sub, 0))

        # temp_slt2 = (0 < temp_sub) ? 1 : 0
        prog.add_inst(AsmModule.Slt(temp_slt2, "x0", temp_sub))

        # v_name = temp_slt1 ^ temp_slt2 (non-zero if not equal)
        prog.add_inst(AsmModule.Xor(v_name, temp_slt1, temp_slt2))

        # Invert v_name to represent equality: v_name = v_name ^ 1
        prog.add_inst(AsmModule.Xori(v_name, v_name, 1))

        return v_name

    def visit_and(self, exp, prog):
        """
        >>> e = And(Bln(True), Bln(True))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = And(Bln(False), Bln(True))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = And(Bln(True), Bln(False))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = And(Bln(False), Bln(False))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = And(Bln(False), Div(Num(3), Num(0)))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0
        """
        left_var = exp.left.accept(self, prog)
        right_var = exp.right.accept(self, prog)
        v_name = self.next_var_name()

        # AND lógico com multiplicação
        prog.add_inst(AsmModule.Mul(v_name, left_var, right_var))
        return v_name

    def visit_or(self, exp, prog):
        """
        >>> e = Or(Bln(True), Bln(True))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Or(Bln(False), Bln(True))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Or(Bln(True), Bln(False))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Or(Bln(False), Bln(False))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Or(Bln(True), Div(Num(3), Num(0)))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1
        """
        left_var = exp.left.accept(self, prog)
        right_var = exp.right.accept(self, prog)
        mul_name = self.next_var_name()
        add_name = self.next_var_name()
        v_name = self.next_var_name()

        # Calcula A * B
        prog.add_inst(AsmModule.Mul(mul_name, left_var, right_var))
        # Calcula A + B
        prog.add_inst(AsmModule.Add(add_name, left_var, right_var))
        # OR lógico: A + B - A * B
        prog.add_inst(AsmModule.Sub(v_name, add_name, mul_name))
        return v_name

    def visit_add(self, exp, prog):
        """
        >>> e = Add(Num(13), Num(-13))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Add(Num(13), Num(10))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        23
        """
        l_name = exp.left.accept(self, prog)
        r_name = exp.right.accept(self, prog)
        v_name = self.next_var_name()
        prog.add_inst(AsmModule.Add(v_name, l_name, r_name))
        return v_name

    def visit_sub(self, exp, prog):
        """
        >>> e = Sub(Num(13), Num(-13))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        26

        >>> e = Sub(Num(13), Num(10))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        3
        """
        l_name = exp.left.accept(self, prog)
        r_name = exp.right.accept(self, prog)
        v_name = self.next_var_name()
        prog.add_inst(AsmModule.Sub(v_name, l_name, r_name))
        return v_name

    def visit_mul(self, exp, prog):
        """
        >>> e = Mul(Num(13), Num(2))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        26

        >>> e = Mul(Num(13), Num(10))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        130
        """
        l_name = exp.left.accept(self, prog)
        r_name = exp.right.accept(self, prog)
        v_name = self.next_var_name()
        prog.add_inst(AsmModule.Mul(v_name, l_name, r_name))
        return v_name

    def visit_div(self, exp, prog):
        """
        >>> e = Div(Num(13), Num(2))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        6

        >>> e = Div(Num(13), Num(10))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1
        """
        l_name = exp.left.accept(self, prog)
        r_name = exp.right.accept(self, prog)
        v_name = self.next_var_name()
        prog.add_inst(AsmModule.Div(v_name, l_name, r_name))
        return v_name

    def visit_leq(self, exp, prog):
        """
        >>> e = Leq(Num(3), Num(2))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Leq(Num(3), Num(3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Leq(Num(2), Num(3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Leq(Num(-3), Num(-2))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Leq(Num(-3), Num(-3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Leq(Num(-2), Num(-3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0
        """
        l_name = exp.left.accept(self, prog)
        r_name = exp.right.accept(self, prog)

        temp_sub = self.next_var_name()
        temp_slt = self.next_var_name()
        temp_slt1 = self.next_var_name()
        temp_slt2 = self.next_var_name()
        temp_eql = self.next_var_name()
        v_name = self.next_var_name()

        # temp_slt = (l_name < r_name) ? 1 : 0
        prog.add_inst(AsmModule.Slt(temp_slt, l_name, r_name))

        # temp_sub = l_name - r_name
        prog.add_inst(AsmModule.Sub(temp_sub, l_name, r_name))

        # temp_slt1 = (temp_sub < 0) ? 1 : 0
        prog.add_inst(AsmModule.Slti(temp_slt1, temp_sub, 0))

        # temp_slt2 = (0 < temp_sub) ? 1 : 0
        prog.add_inst(AsmModule.Slt(temp_slt2, "x0", temp_sub))

        # temp_eql = temp_slt1 ^ temp_slt2 (non-zero if not equal)
        prog.add_inst(AsmModule.Xor(temp_eql, temp_slt1, temp_slt2))

        # Invert temp_eql to represent equality: temp_eql = temp_eql ^ 1
        prog.add_inst(AsmModule.Xori(temp_eql, temp_eql, 1))

        # v_name = temp_slt | temp_eql (either < or ==)
        prog.add_inst(AsmModule.Add(v_name, temp_slt, temp_eql))

        return v_name

    def visit_lth(self, exp, prog):
        """
        >>> e = Lth(Num(3), Num(2))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Lth(Num(3), Num(3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Lth(Num(2), Num(3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1
        """
        l_name = exp.left.accept(self, prog)
        r_name = exp.right.accept(self, prog)
        v_name = self.next_var_name()
        prog.add_inst(AsmModule.Slt(v_name, l_name, r_name))
        return v_name

    def visit_neg(self, exp, prog):
        """
        >>> e = Neg(Num(3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        -3

        >>> e = Neg(Num(0))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Neg(Num(-3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        3
        """
        name = exp.exp.accept(self, prog)
        v_name = self.next_var_name()
        prog.add_inst(AsmModule.Sub(v_name, "x0", name))
        return v_name

    def visit_not(self, exp, prog):
        """
        >>> e = Not(Bln(True))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Not(Bln(False))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Not(Num(0))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        1

        >>> e = Not(Num(-2))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0

        >>> e = Not(Num(2))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        0
        """
        name = str(exp.exp.accept(self, prog))
        v_name = self.next_var_name()


        # Usa slt para calcular (name == 0): 1 se name == 0, 0 caso contrário
        prog.add_inst(AsmModule.Slt(v_name, "x0", name))
        # Inverte o resultado para implementar NOT (1 - (name == 0))
        prog.add_inst(AsmModule.Xori(v_name, v_name, 1))
    
        return v_name

    def visit_let(self, exp, prog):
        """
        Usage:
            >>> e = Let('v', Not(Bln(False)), Var('v'))
            >>> p = AsmModule.Program({}, [])
            >>> g = GenVisitor()
            >>> v = e.accept(g, p)
            >>> p.eval()
            >>> p.get_val(v)
            1

            >>> e = Let('v', Num(2), Add(Var('v'), Num(3)))
            >>> p = AsmModule.Program({}, [])
            >>> g = GenVisitor()
            >>> v = e.accept(g, p)
            >>> p.eval()
            >>> p.get_val(v)
            5

            >>> e0 = Let('x', Num(2), Add(Var('x'), Num(3)))
            >>> e1 = Let('y', e0, Mul(Var('y'), Num(10)))
            >>> p = AsmModule.Program({}, [])
            >>> g = GenVisitor()
            >>> v = e1.accept(g, p)
            >>> p.eval()
            >>> p.get_val(v)
            50
        """
        def_var = exp.exp_def.accept(self, prog)
        prog.add_inst(AsmModule.Add(exp.identifier, "x0", def_var))
        result_var = exp.exp_body.accept(self, prog)
        return result_var

    def visit_ifThenElse(self, exp, prog):
        """
        >>> e = IfThenElse(Bln(True), Num(3), Num(5))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        3

        >>> e = IfThenElse(Bln(False), Num(3), Num(5))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        5

        >>> e = IfThenElse(And(Bln(True), Bln(True)), Num(3), Num(5))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        3

        >>> e0 = Mul(Num(2), Add(Num(3), Num(4)))
        >>> e1 = IfThenElse(And(Bln(True), Bln(False)), Num(3), e0)
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e1.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        14

        >>> e0 = Div(Num(2), Num(0))
        >>> e1 = IfThenElse(Bln(True), Num(3), e0)
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e1.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        3

        >>> e0 = Div(Num(2), Num(0))
        >>> e1 = IfThenElse(Bln(False), e0, Num(3))
        >>> p = AsmModule.Program({}, [])
        >>> g = GenVisitor()
        >>> v = e1.accept(g, p)
        >>> p.eval()
        >>> p.get_val(v)
        3
        """
        cond_var = str(exp.cond.accept(self, prog))
        then_var = str(exp.e0.accept(self, prog))
        else_var = str(exp.e1.accept(self, prog))

        # Variáveis intermediárias
        cond_then = self.next_var_name()
        cond_else = self.next_var_name()
        v_name = self.next_var_name()

        # Multiplica cond_var pelo valor de "then"
        prog.add_inst(AsmModule.Mul(cond_then, cond_var, then_var))
        # Inverte cond_var para obter a negação
        prog.add_inst(AsmModule.Xori(cond_else, cond_var, 1))
        # Multiplica o inverso por "else"
        prog.add_inst(AsmModule.Mul(cond_else, cond_else, else_var))
        # Soma os dois resultados: final = cond_then + cond_else
        prog.add_inst(AsmModule.Add(v_name, cond_then, cond_else))
        return v_name

    def visit_fn(self, exp, prog):
        # Here goes some hints. Feel free to disregard them if you think about
        # any other way to implement functions. Indeed, there are many ways to
        # to just that:
        #
        # 1. Allocate space on the stack:
        # 2. Get the param value. Assumed to be on a0:
        # 3. Generate a body for the function and save the return value in a0:
        # 4. Restore the stack, and get back the return address:
        # 5. Jump back to the caller:

        # Aloca um endereço para a função (início do código gerado para ela)
        function_address = self.next_var_name()
        prog.add_inst(AsmModule.Addi(function_address, 'x0', prog.get_number_of_instructions() + 2))

        # Instrução de salto para o código após a função
        jump_to_function = AsmModule.Jal('x0')
        prog.add_inst(jump_to_function)

        # Reserva espaço na pilha e salva o registrador de retorno
        prog.add_inst(AsmModule.Addi('sp', 'sp', -1))
        prog.add_inst(AsmModule.Sw('sp', 0, 'ra'))

        # Move o valor do parâmetro formal para um registrador
        prog.add_inst(AsmModule.Add(exp.formal, "a0", "x0"))

        # Gera o código para o corpo da função e obtém a variável de retorno
        return_value = exp.body.accept(self, prog)

        # Move o valor de retorno para o registrador 'a0'
        prog.add_inst(AsmModule.Add("a0", return_value, "x0"))

        # Restaura o registrador de retorno e ajusta a pilha
        prog.add_inst(AsmModule.Lw('sp', 0, 'ra'))
        prog.add_inst(AsmModule.Addi('sp', 'sp', 1))
        prog.add_inst(AsmModule.Jalr('x0', 'ra'))

        # Define o endereço de salto para o início da função
        jump_to_function.set_target(prog.get_number_of_instructions())

        # Retorna o endereço da função
        return function_address

    def visit_app(self, exp, prog):
        # Here goes some more hints. Again, take them if you feel like it:
        #
        # 1. Generate the instructions to find out the target of the call:
        # 2. Generate code to compute the parameter of the call:
        # 3. Jump to the function. Remember that Jarl saves the current address into ra:
        # 4. Get the return value of the function. It's meant to be on a0:
        
        # Gera o código para a função chamada e obtém seu endereço
        function_address = exp.function.accept(self, prog)

        # Gera o código para o valor do argumento da função
        parameter_value = exp.actual.accept(self, prog)
        prog.add_inst(AsmModule.Add("a0", parameter_value, "x0"))

        # Salta para a função e armazena o endereço de retorno em 'ra'
        prog.add_inst(AsmModule.Jalr("ra", function_address))

        # Captura o valor retornado pela função em uma nova variável
        result_variable = self.next_var_name()
        prog.add_inst(AsmModule.Add(result_variable, "a0", "x0"))

        # Retorna a variável que contém o resultado da chamada
        return result_variable

class RenameVisitor(ABC):
    """
    This visitor traverses the AST of a program, renaming variables to ensure
    that they all have different names.
    """

    def __init__(self):
        # TODO: You might want to initialize some stuff here.
        self.counter = 0 
    
    def generate_name(self, base_name):
        name = f"{base_name}_{self.counter}"
        self.counter += 1 
        return name

    def visit_var(self, exp, name_map):
        if exp.identifier in name_map:
            exp.identifier = name_map[exp.identifier]

    def visit_bln(self, exp, name_map):
        pass

    def visit_num(self, exp, name_map):
        pass

    def visit_eql(self, exp, name_map):
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_and(self, exp, name_map):
        """
        Example:
            >>> y0 = Var('x')
            >>> y1 = Var('x')
            >>> x0 = And(Lth(y0, Num(2)), Leq(Num(2), y1))
            >>> x1 = Var('x')
            >>> e0 = Let('x', Num(2), Add(x0, Num(3)))
            >>> e1 = Let('x', e0, Mul(x1, Num(10)))
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> y0.identifier == y1.identifier
            True

            >>> y0 = Var('x')
            >>> y1 = Var('x')
            >>> x0 = And(Lth(y0, Num(2)), Leq(Num(2), y1))
            >>> x1 = Var('x')
            >>> e0 = Let('x', Num(2), Add(x0, Num(3)))
            >>> e1 = Let('x', e0, Mul(x1, Num(10)))
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> y0.identifier == x1.identifier
            False
        """
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_or(self, exp, name_map):
        """
        Example:
            >>> y0 = Var('x')
            >>> y1 = Var('x')
            >>> x0 = Or(Lth(y0, Num(2)), Leq(Num(2), y1))
            >>> x1 = Var('x')
            >>> e0 = Let('x', Num(2), Add(x0, Num(3)))
            >>> e1 = Let('x', e0, Mul(x1, Num(10)))
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> y0.identifier == y1.identifier
            True

            >>> y0 = Var('x')
            >>> y1 = Var('x')
            >>> x0 = Or(Lth(y0, Num(2)), Leq(Num(2), y1))
            >>> x1 = Var('x')
            >>> e0 = Let('x', Num(2), Add(x0, Num(3)))
            >>> e1 = Let('x', e0, Mul(x1, Num(10)))
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> y0.identifier == x1.identifier
            False
        """
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_add(self, exp, name_map):
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_sub(self, exp, name_map):
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_mul(self, exp, name_map):
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_div(self, exp, name_map):
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_leq(self, exp, name_map):
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_lth(self, exp, name_map):
        exp.left.accept(self, name_map)
        exp.right.accept(self, name_map)

    def visit_neg(self, exp, name_map):
        exp.exp.accept(self, name_map)

    def visit_not(self, exp, name_map):
        exp.exp.accept(self, name_map)

    def visit_ifThenElse(self, exp, name_map):
        """
        Examples:
            >>> x0 = Var('x')
            >>> x1 = Var('x')
            >>> e0 = IfThenElse(Lth(x0, x1), Num(1), Num(2))
            >>> e1 = Let('x', Num(3), e0)
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> x0.identifier == x1.identifier
            True

            >>> x0 = Var('x')
            >>> x1 = Var('x')
            >>> e0 = IfThenElse(Lth(x0, x1), Num(1), Num(2))
            >>> e1 = Let('x', Num(3), e0)
            >>> e2 = Let('x', e1, Num(3))
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> e2.identifier != x1.identifier == e1.identifier
            True
        """
        exp.cond.accept(self, name_map)
        exp.e0.accept(self, name_map)
        exp.e1.accept(self, name_map)

    def visit_let(self, exp, name_map):
        """
        Examples:
            >>> e0 = Let('x', Num(2), Add(Var('x'), Num(3)))
            >>> e1 = Let('x', e0, Mul(Var('x'), Num(10)))
            >>> e0.identifier == e1.identifier
            True

            >>> e0 = Let('x', Num(2), Add(Var('x'), Num(3)))
            >>> e1 = Let('x', e0, Mul(Var('x'), Num(10)))
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> e0.identifier == e1.identifier
            False

            >>> x0 = Var('x')
            >>> x1 = Var('x')
            >>> e0 = Let('x', Num(2), Add(x0, Num(3)))
            >>> e1 = Let('x', e0, Mul(x1, Num(10)))
            >>> x0.identifier == x1.identifier
            True

            >>> x0 = Var('x')
            >>> x1 = Var('x')
            >>> e0 = Let('x', Num(2), Add(x0, Num(3)))
            >>> e1 = Let('x', e0, Mul(x1, Num(10)))
            >>> r = RenameVisitor()
            >>> e1.accept(r, {})
            >>> x0.identifier == x1.identifier
            False
        """
        original_name = exp.identifier
        new_name = self.generate_name(original_name)
        
        previous_mapping = name_map.get(original_name)

        exp.exp_def.accept(self, name_map)
        
        name_map[original_name] = new_name
        exp.identifier = new_name
        
        exp.exp_body.accept(self, name_map)
        
        if previous_mapping is None:
            del name_map[original_name]  
        else:
            name_map[original_name] = previous_mapping

    def visit_fn(self, exp, name_map):
        """
        >>> e0 = Fn('v', Mul(Var('v'), Var('v')))
        >>> e1 = Let('v', e0, Var('v'))
        >>> e1.accept(RenameVisitor(), {})
        >>> e0.formal != e1.identifier
        True

        >>> x0 = Var('v')
        >>> x1 = Var('v')
        >>> x2 = Var('v')
        >>> e0 = Fn('v', Mul(x0, x2))
        >>> e1 = Let('v', e0, x1)
        >>> e1.accept(RenameVisitor(), {})
        >>> x0.identifier != x1.identifier and x0.identifier == x2.identifier
        True
        """
        
        # Gerar um novo nome para o parâmetro formal
        original_formal = exp.formal
        new_formal = self.generate_name(original_formal)
        
        # Atualizar o mapeamento de nomes
        previous_mapping = name_map.get(original_formal)
        name_map[original_formal] = new_formal
        exp.formal = new_formal
        
        # Renomear o corpo da função
        exp.body.accept(self, name_map)
        
        # Restaurar o mapeamento anterior
        if previous_mapping is None:
            del name_map[original_formal]
        else:
            name_map[original_formal] = previous_mapping

    def visit_app(self, exp, name_map):
        """
        >>> x0 = Var('x')
        >>> x1 = Var('x')
        >>> x2 = Var('x')
        >>> e = Let('x', Fn('x', Add(x0, Num(1))), App(x1, x2))
        >>> e.accept(RenameVisitor(), {})
        >>> x0.identifier != x1.identifier and x1.identifier == x2.identifier
        True
        """
                
        # Renomear a função sendo chamada
        exp.function.accept(self, name_map)
        
        # Renomear o argumento da chamada
        exp.actual.accept(self, name_map)
        