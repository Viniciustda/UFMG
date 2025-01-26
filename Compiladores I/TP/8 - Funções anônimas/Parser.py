import sys

from Expression import *
from Lexer import Token, TokenType

"""
This file implements a parser for SML with anonymous functions. The grammar is
as follows:

fn_exp  ::= fn <var> => fn_exp
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
let_exp ::= <let> <var> <- fn_exp <in> fn_exp <end>
          | val_exp
val_exp ::= val_tk (val_tk)*
val_tk ::= <var> | ( fn_exp ) | <num> | <true> | <false>

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
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        123

        >>> parser = Parser([Token('True', TokenType.TRU)])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        True

        >>> parser = Parser([Token('False', TokenType.FLS)])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        False

        >>> tk0 = Token('~', TokenType.NEG)
        >>> tk1 = Token('123', TokenType.NUM)
        >>> parser = Parser([tk0, tk1])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        -123

        >>> tk0 = Token('3', TokenType.NUM)
        >>> tk1 = Token('*', TokenType.MUL)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        12

        >>> tk0 = Token('3', TokenType.NUM)
        >>> tk1 = Token('*', TokenType.MUL)
        >>> tk2 = Token('~', TokenType.NEG)
        >>> tk3 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        -12

        >>> tk0 = Token('30', TokenType.NUM)
        >>> tk1 = Token('/', TokenType.DIV)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        7

        >>> tk0 = Token('3', TokenType.NUM)
        >>> tk1 = Token('+', TokenType.ADD)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        7

        >>> tk0 = Token('30', TokenType.NUM)
        >>> tk1 = Token('-', TokenType.SUB)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        26

        >>> tk0 = Token('2', TokenType.NUM)
        >>> tk1 = Token('*', TokenType.MUL)
        >>> tk2 = Token('(', TokenType.LPR)
        >>> tk3 = Token('3', TokenType.NUM)
        >>> tk4 = Token('+', TokenType.ADD)
        >>> tk5 = Token('4', TokenType.NUM)
        >>> tk6 = Token(')', TokenType.RPR)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5, tk6])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        14

        >>> tk0 = Token('4', TokenType.NUM)
        >>> tk1 = Token('==', TokenType.EQL)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        True

        >>> tk0 = Token('4', TokenType.NUM)
        >>> tk1 = Token('<=', TokenType.LEQ)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        True

        >>> tk0 = Token('4', TokenType.NUM)
        >>> tk1 = Token('<', TokenType.LTH)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        False

        >>> tk0 = Token('not', TokenType.NOT)
        >>> tk1 = Token('(', TokenType.LPR)
        >>> tk2 = Token('4', TokenType.NUM)
        >>> tk3 = Token('<', TokenType.LTH)
        >>> tk4 = Token('4', TokenType.NUM)
        >>> tk5 = Token(')', TokenType.RPR)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        True

        >>> tk0 = Token('true', TokenType.TRU)
        >>> tk1 = Token('or', TokenType.ORX)
        >>> tk2 = Token('false', TokenType.FLS)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        True

        >>> tk0 = Token('true', TokenType.TRU)
        >>> tk1 = Token('and', TokenType.AND)
        >>> tk2 = Token('false', TokenType.FLS)
        >>> parser = Parser([tk0, tk1, tk2])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        False

        >>> tk0 = Token('let', TokenType.LET)
        >>> tk1 = Token('v', TokenType.VAR)
        >>> tk2 = Token('<-', TokenType.ASN)
        >>> tk3 = Token('42', TokenType.NUM)
        >>> tk4 = Token('in', TokenType.INX)
        >>> tk5 = Token('v', TokenType.VAR)
        >>> tk6 = Token('end', TokenType.END)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5, tk6])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, {})
        42

        >>> tk0 = Token('let', TokenType.LET)
        >>> tk1 = Token('v', TokenType.VAR)
        >>> tk2 = Token('<-', TokenType.ASN)
        >>> tk3 = Token('21', TokenType.NUM)
        >>> tk4 = Token('in', TokenType.INX)
        >>> tk5 = Token('v', TokenType.VAR)
        >>> tk6 = Token('+', TokenType.ADD)
        >>> tk7 = Token('v', TokenType.VAR)
        >>> tk8 = Token('end', TokenType.END)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5, tk6, tk7, tk8])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, {})
        42

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
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        1

        >>> tk0 = Token('if', TokenType.IFX)
        >>> tk1 = Token('false', TokenType.FLS)
        >>> tk2 = Token('then', TokenType.THN)
        >>> tk3 = Token('1', TokenType.NUM)
        >>> tk4 = Token('else', TokenType.ELS)
        >>> tk5 = Token('2', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, None)
        2

        >>> tk0 = Token('fn', TokenType.FNX)
        >>> tk1 = Token('v', TokenType.VAR)
        >>> tk2 = Token('=>', TokenType.ARW)
        >>> tk3 = Token('v', TokenType.VAR)
        >>> tk4 = Token('+', TokenType.ADD)
        >>> tk5 = Token('1', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> print(exp.accept(ev, None))
        Fn(v)

        >>> tk0 = Token('(', TokenType.LPR)
        >>> tk1 = Token('fn', TokenType.FNX)
        >>> tk2 = Token('v', TokenType.VAR)
        >>> tk3 = Token('=>', TokenType.ARW)
        >>> tk4 = Token('v', TokenType.VAR)
        >>> tk5 = Token('+', TokenType.ADD)
        >>> tk6 = Token('1', TokenType.NUM)
        >>> tk7 = Token(')', TokenType.RPR)
        >>> tk8 = Token('2', TokenType.NUM)
        >>> parser = Parser([tk0, tk1, tk2, tk3, tk4, tk5, tk6, tk7, tk8])
        >>> exp = parser.parse()
        >>> ev = EvalVisitor()
        >>> exp.accept(ev, {})
        3
        """
        return self.parse_fn()
    
    def parse_fn(self):
        """
        Função para definição de função anônima.
        """
        if self.current_token() and self.current_token().kind == TokenType.FNX:
            self.consume_token() 
            param_token = self.consume_token()
            
            if param_token.kind != TokenType.VAR:
                raise ValueError("Expected variable name after 'fn'")
            
            param_name = param_token.text 
            
            arrow_token = self.consume_token()
            if arrow_token.kind != TokenType.ARW:
                raise ValueError("Expected '=>' after variable name in 'fn' expression")
            
            body = self.parse_fn()
            return Fn(param_name, body)
        
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
        Nível de precedência para a expressão 'let-in-end'.
        """
        if self.current_token() and self.current_token().kind == TokenType.LET:
            self.consume_token()  # Consome 'let'
            var_name = self.consume_token()  # Nome da variável
            if var_name.kind != TokenType.VAR:
                raise ValueError("Expected variable name after 'let'")
            
            assign_op = self.consume_token()
            if assign_op.kind != TokenType.ASN:
                raise ValueError("Expected '<-' after variable name in 'let' expression")
            
            expr_value = self.parse_fn()  # Avalia a expressão atribuída
            
            in_token = self.consume_token()
            if in_token.kind != TokenType.INX:
                raise ValueError("Expected 'in' after assignment in 'let' expression")
            
            body = self.parse_fn()  # Avalia a expressão do corpo
            
            end_token = self.consume_token()
            if end_token.kind != TokenType.END:
                raise ValueError("Expected 'end' at the end of 'let' expression")
            
            return Let(var_name.text, expr_value, body)
        
        return self.parse_app()
    
    def parse_app(self):
        """
        Função de aplicação.
        """
        # A expressão inicial a ser usada como base
        left = self.parse_primary()

        # Continuação enquanto houver tokens primários adicionais para formar uma aplicação
        while self.current_token() and (
            self.current_token().kind in [TokenType.VAR, TokenType.NUM, TokenType.LPR, TokenType.TRU, TokenType.FLS]
        ):
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