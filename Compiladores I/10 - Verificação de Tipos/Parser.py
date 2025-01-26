import sys

from Expression import *
from Lexer import Token, TokenType

"""
This file implements a parser for SML with anonymous functions and type
annotations. The grammar is as follows:

fn_exp  ::= fn <var>: types => fn_exp
          | if_exp
if_exp  ::= <if> if_exp <then> fn_exp <else> fn_exp
          | or_exp
or_exp  ::= and_exp (or and_exp)*
and_exp ::= eq_exp (and eq_exp)*
eq_exp  ::= cmp_exp (= cmp_exp)*
cmp_exp ::= add_exp ([<=|<] add_exp)*
add_exp ::= mul_exp ([+|-] mul_exp)*
mul_exp ::= unary_exp ([*|/] unary_exp)*
unary_exp ::= <not> unary_exp
             | ~ unary_exp
             | let_exp
let_exp ::= <let> <var>: types <- fn_exp <in> fn_exp <end>
          | val_exp
val_exp ::= val_tk (val_tk)*
val_tk ::= <var> | ( fn_exp ) | <num> | <true> | <false>

types ::= type -> types | type

type ::= int | bool | ( types )

References:
    see https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm#classic
"""

class Parser:
    def __init__(self, tokens):
        """
        Initializes the parser. The parser keeps track of the list of tokens
        and the current token. For instance:
        """
        self.tokens = list(tokens)
        self.cur_token_idx = 0 # This is just a suggestion!
        # You can (and probably should!) modify this method.

    def current_token(self):
        """Retorna o token atual sem consumir."""
        if self.cur_token_idx < len(self.tokens):
            return self.tokens[self.cur_token_idx]
        return None

    def consume_token(self):
        """Avança para o próximo token e retorna o token atual."""
        token = self.current_token()
        self.cur_token_idx += 1
        return token

    def parse(self):
        """
        Returns the expression associated with the stream of tokens.

        Examples:
        >>> parser = Parser([Token('123', TokenType.NUM)])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> parser = Parser([Token('True', TokenType.TRU)])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> parser = Parser([Token('False', TokenType.FLS)])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> tk0 = Token('~', TokenType.NEG)
        >>> tk1 = Token('123', TokenType.NUM)
        >>> parser = Parser([tk0, tk1])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('3', TokenType.NUM)
        >>> tk1 = Token('*', TokenType.MUL)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('3', TokenType.NUM)
        >>> tk1 = Token('*', TokenType.MUL)
        >>> tk2 = Token('~', TokenType.NEG)
        >>> tk3 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('30', TokenType.NUM)
        >>> tk1 = Token('/', TokenType.DIV)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('3', TokenType.NUM)
        >>> tk1 = Token('+', TokenType.ADD)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('30', TokenType.NUM)
        >>> tk1 = Token('-', TokenType.SUB)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('2', TokenType.NUM)
        >>> tk1 = Token('*', TokenType.MUL)
        >>> tk2 = Token('(', TokenType.LPR)
        >>> tk3 = Token('3', TokenType.NUM)
        >>> tk4 = Token('+', TokenType.ADD)
        >>> tk5 = Token('4', TokenType.NUM)
        >>> tk6 = Token(')', TokenType.RPR)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5, tk6])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('4', TokenType.NUM)
        >>> tk1 = Token('==', TokenType.EQL)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> tk0 = Token('4', TokenType.NUM)
        >>> tk1 = Token('<=', TokenType.LEQ)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> tk0 = Token('4', TokenType.NUM)
        >>> tk1 = Token('<', TokenType.LTH)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> tk0 = Token('not', TokenType.NOT)
        >>> tk1 = Token('(', TokenType.LPR)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> tk3 = Token('<', TokenType.LTH)
        >>> tk4 = Token('4', TokenType.NUM)
        >>> tk5 = Token(')', TokenType.RPR)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> tk0 = Token('true', TokenType.TRU)
        >>> tk1 = Token('or', TokenType.ORX)
        >>> tk2 = Token('false', TokenType.FLS)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> tk0 = Token('true', TokenType.TRU)
        >>> tk1 = Token('and', TokenType.AND)
        >>> tk2 = Token('false', TokenType.FLS)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'bool'>

        >>> t0 = Token('let', TokenType.LET)
        >>> t1 = Token('v', TokenType.VAR)
        >>> t2 = Token(':', TokenType.COL)
        >>> t3 = Token('int', TokenType.INT)
        >>> t4 = Token('<-', TokenType.ASN)
        >>> t5 = Token('42', TokenType.NUM)
        >>> t6 = Token('in', TokenType.INX)
        >>> t7 = Token('v', TokenType.VAR)
        >>> t8 = Token('end', TokenType.END)
        >>> parser = Parser([t0, t1, t2, t3, t4, t5, t6, t7, t8])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, {})
        <class 'int'>

        >>> t0 = Token('let', TokenType.LET)
        >>> t1 = Token('v', TokenType.VAR)
        >>> t2 = Token(':', TokenType.COL)
        >>> t3 = Token('int', TokenType.INT)
        >>> t4 = Token('<-', TokenType.ASN)
        >>> t5 = Token('21', TokenType.NUM)
        >>> t6 = Token('in', TokenType.INX)
        >>> t7 = Token('v', TokenType.VAR)
        >>> t8 = Token('+', TokenType.ADD)
        >>> t9 = Token('v', TokenType.VAR)
        >>> tA = Token('end', TokenType.END)
        >>> parser = Parser([t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, tA])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, {})
        <class 'int'>

        >>> tk0 = Token('if', TokenType.IFX)
        >>> tk1 = Token('2', TokenType.NUM)
        >>> tk2 = Token('<', TokenType.LTH)
        >>> tk3 = Token('3', TokenType.NUM)
        >>> tk4 = Token('then', TokenType.THN)
        >>> tk5 = Token('1', TokenType.NUM)
        >>> tk6 = Token('else', TokenType.ELS)
        >>> tk7 = Token('2', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5, tk6, tk7])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('if', TokenType.IFX)
        >>> tk1 = Token('false', TokenType.FLS)
        >>> tk2 = Token('then', TokenType.THN)
        >>> tk3 = Token('1', TokenType.NUM)
        >>> tk4 = Token('else', TokenType.ELS)
        >>> tk5 = Token('2', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, None)
        <class 'int'>

        >>> tk0 = Token('fn', TokenType.FNX)
        >>> tk1 = Token('v', TokenType.VAR)
        >>> tk2 = Token(':', TokenType.COL)
        >>> tk3 = Token('int', TokenType.INT)
        >>> tk4 = Token('=>', TokenType.ARW)
        >>> tk5 = Token('v', TokenType.VAR)
        >>> tk6 = Token('+', TokenType.ADD)
        >>> tk7 = Token('1', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5, tk6, tk7])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> print(exp.accept(ev, {}))
        <class 'int'> -> <class 'int'>

        >>> t0 = Token('(', TokenType.LPR)
        >>> t1 = Token('fn', TokenType.FNX)
        >>> t2 = Token('v', TokenType.VAR)
        >>> t3 = Token(':', TokenType.COL)
        >>> t4 = Token('int', TokenType.INT)
        >>> t5 = Token('=>', TokenType.ARW)
        >>> t6 = Token('v', TokenType.VAR)
        >>> t7 = Token('+', TokenType.ADD)
        >>> t8 = Token('1', TokenType.NUM)
        >>> t9 = Token(')', TokenType.RPR)
        >>> tA = Token('2', TokenType.NUM)
        >>> parser = Parser([t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, tA])
        >>> exp = parser.parse()
        >>> ev = TypeCheckVisitor()
        >>> exp.accept(ev, {})
        <class 'int'>
        """
        return self.parse_fn()
    
    def parse_fn(self):
        """
        Função para definição de função anônima.
        """
        if self.current_token() and self.current_token().kind == TokenType.FNX:
            self.consume_token()
            var = self.consume_token()
            
            if var.kind != TokenType.VAR:
                raise ValueError("Expected variable name after 'fn'")
            
            var_name = var.text

            colon = self.consume_token()
            if colon.kind != TokenType.COL:
                raise ValueError("Expected ':' after variable name in 'fn' expression")
            
            # Parsing do tipo usando ArrowType
            var_type = self.parse_type()
            
            arrow_token = self.consume_token()
            if arrow_token.kind != TokenType.ARW:
                raise ValueError("Expected '=>' after variable type in 'fn' expression")
            
            body = self.parse_fn()
            return Fn(var_name, var_type, body)
        
        return self.parse_if_then_else()
    
    def parse_if_then_else(self):
        """
        Nível de precedência para a expressão 'if-then-else'.
        """
        if self.current_token() and self.current_token().kind == TokenType.IFX:
            self.consume_token()  # Consome 'if'
            
            # Parse da condição
            condition = self.parse_fn()

            # Verifica e consome 'then'
            if self.current_token() and self.current_token().kind == TokenType.THN:
                self.consume_token()
            else:
                sys.exit("Parse error")

            # Parse da expressão do ramo 'then'
            then_branch = self.parse_fn()

            # Verifica e consome 'else'
            if self.current_token() and self.current_token().kind == TokenType.ELS:
                self.consume_token()
            else:
                sys.exit("Parse error")

            # Parse da expressão do ramo 'else'
            else_branch = self.parse_fn()

            return IfThenElse(condition, then_branch, else_branch)

        # Se não for uma expressão 'if-then-else', segue para o próximo nível de precedência
        return self.parse_or()
    
    def parse_or(self):
        """
        Nível de precedência para or.
        """
        left = self.parse_and()

        if self.current_token() and self.current_token().kind == TokenType.ORX:
            self.consume_token()
            right = self.parse_and()
            left = Or(left, right)
        return left
    
    def parse_and(self):
        """
        Nível de precedência para and.
        """
        left = self.parse_comparison()

        if self.current_token() and self.current_token().kind == TokenType.AND:
            self.consume_token()
            right = self.parse_comparison()
            left = And(left, right)
        return left
    
    def parse_comparison(self):
        """
        Nível de precedência para igualdade e operadores de comparação.
        """
        left = self.parse_term()
        
        while self.current_token() and self.current_token().kind in (TokenType.EQL, TokenType.LEQ, TokenType.LTH):
            operator = self.consume_token()
            right = self.parse_term()
            if operator.kind == TokenType.EQL:
                left = Eql(left, right)
            elif operator.kind == TokenType.LEQ:
                left = Leq(left, right)
            elif operator.kind == TokenType.LTH:
                left = Lth(left, right)
        
        return left
    
    def parse_term(self):
        """
        Nível de precedência para adição e subtração.
        """
        left = self.parse_factor()

        while self.current_token() and self.current_token().kind in (TokenType.ADD, TokenType.SUB):
            operator = self.consume_token()
            right = self.parse_factor()
            if operator.kind == TokenType.ADD:
                left = Add(left, right)
            elif operator.kind == TokenType.SUB:
                left = Sub(left, right)

        return left

    def parse_factor(self):
        """
        Nível de precedência para multiplicação e divisão.
        """
        left = self.parse_unary()

        while self.current_token() and self.current_token().kind in (TokenType.MUL, TokenType.DIV):
            operator = self.consume_token()
            right = self.parse_unary()
            if operator.kind == TokenType.MUL:
                left = Mul(left, right)
            elif operator.kind == TokenType.DIV:
                left = Div(left, right)

        return left

    def parse_unary(self):
        """
        Nível de precedência para operadores unários, como ~ (negação).
        """
        if self.current_token():
            if self.current_token().kind == TokenType.NEG:
                self.consume_token()
                operand = self.parse_unary()
                return Neg(operand)
            
        return self.parse_not_expression()
    
    def parse_not_expression(self):
        """
        Nível de precedência para o operador 'not'.
        """
        if self.current_token() and self.current_token().kind == TokenType.NOT:
            self.consume_token()
            operand = self.parse_let_expression()
            return Not(operand)
        
        return self.parse_let_expression()
    
    def parse_let_expression(self):
        """
        Parsing da expressão 'let-in-end', com suporte a tipos compostos.
        """
        if self.current_token() and self.current_token().kind == TokenType.LET:
            self.consume_token()

            var_name = self.consume_token()
            if var_name.kind != TokenType.VAR:
                raise ValueError("Expected variable name after 'let'")
            
            colon = self.consume_token()
            if colon.kind != TokenType.COL:
                raise ValueError("Expected ':' after variable name in 'let' expression")
            
            # Parsing do tipo usando ArrowType
            var_type = self.parse_type()
            
            assign_op = self.consume_token()
            if assign_op.kind != TokenType.ASN:
                raise ValueError("Expected '<-' after variable type in 'let' expression")
            
            expr_value = self.parse_fn()
            
            in_token = self.consume_token()
            if in_token.kind != TokenType.INX:
                raise ValueError("Expected 'in' after assignment in 'let' expression")
            
            body = self.parse_fn()
            
            end_token = self.consume_token()
            if end_token.kind != TokenType.END:
                raise ValueError("Expected 'end' at the end of 'let' expression")
            
            return Let(var_name.text, var_type, expr_value, body)
        
        return self.parse_app()
    
    def parse_app(self):
        """
        Função de aplicação.
        """
        # A expressão inicial a ser usada como base
        left = self.parse_primary()

        # Continuação enquanto houver tokens primários adicionais para formar uma aplicação
        while self.current_token() and (self.current_token().kind in [TokenType.VAR, TokenType.NUM, TokenType.LPR, TokenType.TRU, TokenType.FLS]):
            # O próximo argumento da aplicação é analisado como uma nova expressão primária
            right = self.parse_primary()
            left = App(left, right)  # Cria uma aplicação com a expressão anterior e o novo termo

        return left

    def parse_primary(self):
        """
        Lê números, booleanos e expressões entre parênteses.
        """
        token = self.consume_token()
        
        if token.kind == TokenType.NUM:
            return Num(int(token.text))
        elif token.kind == TokenType.TRU:
            return Bln(True)
        elif token.kind == TokenType.FLS:
            return Bln(False)
        elif token.kind == TokenType.LPR:
            expr = self.parse_fn()
            if self.current_token() and self.current_token().kind == TokenType.RPR:
                self.consume_token()  # Consome o parêntese direito
            return expr
        elif token.kind == TokenType.VAR:
            return Var(token.text)
        else:
            sys.exit("Parse error")

    def parse_type(self):
        """
        Parsing de tipos compostos como int -> int -> bool.
        """
        if self.current_token().kind == TokenType.LPR:
            self.consume_token()  # Consome '('
            inner_type = self.parse_type()
            self.consume_token()  # Consome ')'
            base_type = inner_type
        else:
            token = self.consume_token()
            if token.kind == TokenType.INT:
                base_type = int
            elif token.kind == TokenType.LGC:
                base_type = bool
            else:
                raise ValueError("Expected a valid type (int or bool)")

        # Verifica flechas para tipos compostos
        while self.current_token() and self.current_token().kind == TokenType.TPF:
            self.consume_token()  # Consome '->'
            base_type = ArrowType(base_type, self.parse_type())
        
        return base_type