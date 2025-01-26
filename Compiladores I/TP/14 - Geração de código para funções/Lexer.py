import sys
import enum

class Token:
    """
    This class contains the definition of Tokens. A token has two fields: its
    text and its kind. The "kind" of a token is a constant that identifies it
    uniquely. See the TokenType to know the possible identifiers (if you want).
    You don't need to change this class.
    """
    def __init__(self, tokenText, tokenKind):
        # The token's actual text. Used for identifiers, strings, and numbers.
        self.text = tokenText
        # The TokenType that this token is classified as.
        self.kind = tokenKind

class TokenType(enum.Enum):
    """
    These are the possible tokens. You don't need to change this class at all.
    """
    EOF = -1  # End of file
    NLN = 0  # New line
    WSP = 1  # White Space
    COM = 2  # Comment
    NUM = 3  # Number (integers)
    STR = 4  # Strings
    TRU = 5  # The constant true
    FLS = 6  # The constant false
    VAR = 7  # An identifier
    LET = 8  # The 'let' of the let expression
    INX = 9  # The 'in' of the let expression
    END = 10  # The 'end' of the let expression
    EQL = 201  # x = y
    ADD = 202  # x + y
    SUB = 203  # x - y
    MUL = 204  # x * y
    DIV = 205  # x / y
    LEQ = 206  # x <= y
    LTH = 207  # x < y
    NEG = 208  # ~x
    NOT = 209  # not x
    LPR = 210  # (
    RPR = 211  # )
    ASN = 212  # The assignment '<-' operator
    ORX = 213  # x or y
    AND = 214  # x and y
    IFX = 215  # The 'if' of a conditional expression
    THN = 216  # The 'then' of a conditional expression
    ELS = 217  # The 'else' of a conditional expression
    FNX = 218  # The 'fn' that declares an anonymous function
    ARW = 219  # The '=>' that separates the parameter from the body of function

class Lexer:
    
    def __init__(self, source):
        """
        The constructor of the lexer. It receives the string that shall be
        scanned.
        """
        self.source = source
        self.position = 0
        self.length = len(source)

    def tokens(self):
        """
        This method is a token generator: it converts the string encapsulated
        into this object into a sequence of Tokens. Notice that this method
        filters out three kinds of tokens: white-spaces, comments and new lines.

        Examples:

        >>> l = Lexer("1 + 3")
        >>> [tk.kind for tk in l.tokens()]
        [<TokenType.NUM: 3>, <TokenType.ADD: 202>, <TokenType.NUM: 3>]

        >>> l = Lexer('1 * 2\\n')
        >>> [tk.kind for tk in l.tokens()]
        [<TokenType.NUM: 3>, <TokenType.MUL: 204>, <TokenType.NUM: 3>]

        >>> l = Lexer('1 * 2 -- 3\\n')
        >>> [tk.kind for tk in l.tokens()]
        [<TokenType.NUM: 3>, <TokenType.MUL: 204>, <TokenType.NUM: 3>]

        >>> l = Lexer("1 + var")
        >>> [tk.kind for tk in l.tokens()]
        [<TokenType.NUM: 3>, <TokenType.ADD: 202>, <TokenType.VAR: 7>]

        >>> l = Lexer("let v <- 2 in v end")
        >>> [tk.kind.name for tk in l.tokens()]
        ['LET', 'VAR', 'ASN', 'NUM', 'INX', 'VAR', 'END']
        """
        token = self.getToken()
        while token.kind != TokenType.EOF:
            if (
                token.kind != TokenType.WSP
                and token.kind != TokenType.COM
                and token.kind != TokenType.NLN
            ):
                yield token
            token = self.getToken()

    def getToken(self):
        """
        Return the next token.
        TODO: Implement this method (you can reuse Lab 5: Visitors)!
        """
        if self.position >= self.length:
            return Token("EOF", TokenType.EOF)
        
        current_char = self.source[self.position]
        
        # Ignora espaços em branco e nova linha
        if current_char.isspace():
            if current_char == '\n':
                self.position += 1
                return Token("\n", TokenType.NLN)
            self.position += 1
            return self.getToken()
        
        # Reconhece números
        if current_char.isdigit():
            return self.extractNumber()
        
        # Operadores simples
        if current_char == '+':
            self.position += 1
            return Token("+", TokenType.ADD)
        elif current_char == '-':
            # Verifica se é um comentário de linha "--"
            if self.peek() == '-':
                self.position += 2
                # Ignora todos os caracteres até a próxima nova linha
                while self.position < self.length and self.source[self.position] != '\n':
                    self.position += 1
                # Ignora também a nova linha
                self.position += 1
                return self.getToken()  # Ignora o comentário e busca o próximo token
            else:
                self.position += 1
                return Token("-", TokenType.SUB)
        elif current_char == '*':
            self.position += 1
            return Token("*", TokenType.MUL)
        elif current_char == '/':
            self.position += 1
            return Token("/", TokenType.DIV)
        elif current_char == '(':
            # Verifica se é o início de um comentário de bloco
            if self.peek() == '*':
                self.position += 2
                while not (self.source[self.position] == '*' and self.peek() == ')'):
                    self.position += 1
                self.position += 2
                return self.getToken()
            self.position += 1
            return Token("(", TokenType.LPR)
        elif current_char == ')':
            self.position += 1
            return Token(")", TokenType.RPR)
        elif current_char == '=':
            if self.peek() == '>':
                self.position += 2
                return Token("=>", TokenType.ARW)
            else:
                self.position += 1
                return Token("=", TokenType.EQL)
        elif current_char == '<':
            if self.peek() == '=':
                self.position += 2
                return Token("<=", TokenType.LEQ)
            elif self.peek() == '-':
                self.position += 2
                return Token("<-", TokenType.ASN)
            else:
                self.position += 1
                return Token("<", TokenType.LTH)
        elif current_char == '~':
            self.position += 1
            return Token("~", TokenType.NEG)
        
        # Palavras-chave
        if current_char.isalpha():
            return self.extractKeywordOrIdentifier()

        self.position += 1
        return Token(current_char, TokenType.EOF)  

    # Verifica o próximo caractere sem mover o índice position.
    def peek(self):
        if self.position + 1 >= self.length:
            return None
        return self.source[self.position + 1]
    
    # Lê todos os caracteres numéricos consecutivos para formar um número.
    def extractNumber(self):
        start_pos = self.position
        while self.position < self.length and self.source[self.position].isdigit():
            self.position += 1
        return Token(self.source[start_pos:self.position], TokenType.NUM)
    
    # Lê identificadores (palavras) e verifica se eles correspondem a palavras-chave específicas (not, true, false).
    def extractKeywordOrIdentifier(self):
        start_pos = self.position
        while self.position < self.length and self.source[self.position].isalpha():
            self.position += 1
        text = self.source[start_pos:self.position]

        if text == "let":
            return Token("let", TokenType.LET)
        elif text == "in":
            return Token("in", TokenType.INX)
        elif text == "end":
            return Token("end", TokenType.END)
        elif text == "not":
            return Token("not", TokenType.NOT)
        elif text == "true":
            return Token("true", TokenType.TRU)
        elif text == "false":
            return Token("false", TokenType.FLS)
        elif text == "or":
            return Token("or", TokenType.ORX)
        elif text == "and":
            return Token("and", TokenType.AND)
        elif text == "if":
            return Token("if", TokenType.IFX)
        elif text == "then":
            return Token("then", TokenType.THN)
        elif text == "else":
            return Token("else", TokenType.ELS)
        elif text == "fn":
            return Token("fn", TokenType.FNX)
        
        if text == "f" and self.source[self.position].isdigit():
            while self.position < self.length and not self.source[self.position].isspace():
                self.position += 1
            text = self.source[start_pos:self.position]
        
        return Token(text, TokenType.VAR)