from abc import ABC, abstractmethod
from Asm import *
import re

class Optimizer(ABC):
    """
    This class implements an "Optimization Pass". The pass receives a sequence
    of instructions stored in a program, and produces a new sequence of
    instructions.
    """

    def __init__(self, prog):
        self.prog = prog

    @abstractmethod
    def optimize(self):
        pass

class RegAllocator(Optimizer):
    """This file implements the register allocation pass."""

    def __init__(self, prog):
        
        super().__init__(prog)
        self.var_to_reg_or_mem = {}  # Mapeia variáveis para registradores ou memória
        self.registers = ["a0", "a1", "a2", "a3"]  # Registradores disponíveis
        self.register_usage_order = []  # Ordem de uso dos registradores para política LRU (Least Recently Used)
        self.stack_pointer = 0  # Ponteiro de pilha simulado para alocar memória

    def get_val(self, var):
        """
        Informs the value that is associated with the variable var within
        the program prog.
        """
        if var in self.var_to_reg_or_mem:
            location = self.var_to_reg_or_mem[var]
            if location.startswith("a"): # Caso esteja em um registrador
                return self.prog.get_val(location)
            else:  # Caso esteja na memória
                return self.prog.get_mem(int(location))
        raise ValueError(f"Variable {var} is not allocated.")
    
    def is_valid_variable(self, var):
        """
        Verifica se uma variável é válida. 
        Formatos válidos: 'v<number>', '<letra>_<número>' ou 'x0' (caso especial).
        """
        return (
            var == "x0" or  # Caso especial para 'x0'
            bool(re.fullmatch(r"v\d+", var)) or  # Formato 'v<number>'
            bool(re.fullmatch(r"[a-zA-Z]_\d+", var))  # Formato '<letra>_<número>'
        )

    def next_use_distance(self, var):
        """
        Calcula a distância (número de instruções) até o próximo uso da variável.
        """
        distance = 0
        for inst in self.prog.get_insts():
            distance += 1
            if getattr(inst, "rd", None) == var or \
            getattr(inst, "rs1", None) == var or \
            getattr(inst, "rs2", None) == var:
                return distance
        return float('inf')  # Retorna infinito se a variável não será usada novamente.
    
    def update_usage_order(self, reg):
        """
        Atualiza a ordem de uso do registrador para manter a política de LRU.
        """
        if reg in self.register_usage_order:
            self.register_usage_order.remove(reg)
        self.register_usage_order.append(reg)
    
    def allocate_register(self, var):
        """
        Aloca um registrador para a variável fornecida. 
        Caso a variável esteja na memória, carrega o valor para um registrador.
        """
        if var == "x0":
            return var, None, None
        
        if not self.is_valid_variable(var):
            raise ValueError(f"Invalid variable: {var}")

        if var in self.var_to_reg_or_mem:
            loc = self.var_to_reg_or_mem[var]
            if loc.startswith("a"):  # Já está em um registrador
                self.update_usage_order(loc)
                return loc, None, None
            
            # Caso esteja na memória, carrega para um registrador
            reg, escrita = self.registers.pop(0) if self.registers else self.spill_and_reuse_register(var)
            leitura = Lw("x0", int(loc), reg)  # Carrega da memória para o registrador
            self.var_to_reg_or_mem[var] = reg
            self.update_usage_order(reg)
            return reg, escrita, leitura

        # Aloca um novo registrador se disponível
        if len(self.registers) > 0:
            reg = self.registers.pop(0)
            self.var_to_reg_or_mem[var] = reg
            self.update_usage_order(reg)
            return reg, None, None

        # Realiza spill se não houver registradores disponíveis
        reg, escrita = self.spill_and_reuse_register(var)
        return reg, escrita, None
    
    def spill_and_reuse_register(self, var=None):
        """
        Realiza spill (esvaziar) de um registrador com base no tempo de vida da variável e reutiliza o registrador.
        """
        if var and not self.is_valid_variable(var):
            raise ValueError(f"Invalid variable: {var}")

        reg_to_spill = self.register_usage_order.pop(0)  # Pega o registrador menos usado recentemente
        var_to_spill = None
        for k, v in self.var_to_reg_or_mem.items():
            if v == reg_to_spill:
                var_to_spill = k
                break

        mem_loc = self.stack_pointer
        self.stack_pointer += 1
        self.var_to_reg_or_mem[var_to_spill] = str(mem_loc)
        escrita = Sw("x0", mem_loc, reg_to_spill)  # Armazena na memória

        self.var_to_reg_or_mem[var] = reg_to_spill  # Reutiliza o registrador
        self.register_usage_order.append(reg_to_spill)  # Atualiza a ordem de uso
        return reg_to_spill, escrita

    def optimize(self):
        """
        This function perform register allocation. It maps variables into
        memory, and changes instructions, so that they use one of the following
        registers:
        * x0: always the value zero. Can't change.
        * sp: the stack pointer. Starts with the memory size.
        * ra: the return address.
        * a0: function argument 0 (or return address)
        * a1: function argument 1
        * a2: function argument 2
        * a3: function argument 3

        Notice that next to each register we have suggested a usage. You can,
        of course, write on them and use them in other ways. But, at least x0
        and sp you should not overwrite. The first register you can't overwrite,
        actually. And sp is initialized with the number of memory addresses.
        It's good to use it to control the function stack.
        """
        """
        Examples:
        >>> insts = [Addi("a", "x0", 3)]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        3

        >>> insts = [Addi("a", "x0", 1), Slti("b", "a", 2)]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        1

        >>> insts = [Addi("a", "x0", 3), Slti("b", "a", 2), Xori("c", "b", 5)]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        5

        >>> insts = [Addi("sp", "sp", -1),Addi("a", "x0", 7),Sw("sp", 0, "a")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_mem(p.get_val("sp"))
        7

        >>> insts = [Addi("sp", "sp", -1),Addi("a", "x0", 7),Sw("sp", 0, "a")]
        >>> insts += [Lw("sp", 0, "b"), Addi("c", "b", 6)]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        13

        >>> insts = [Addi("a", "x0", 3),Addi("b", "x0", 4),Add("c", "a", "b")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        7

        >>> insts = [Addi("a", "x0", 28),Addi("b", "x0", 4),Div("c", "a", "b")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        7

        >>> insts = [Addi("a", "x0", 3),Addi("b", "x0", 4),Mul("c", "a", "b")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        12

        >>> insts = [Addi("a", "x0", 3),Addi("b", "x0", 4),Xor("c", "a", "b")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        7

        >>> insts = [Addi("a", "x0", 3),Addi("b", "x0", 4),Slt("c", "a", "b")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        1

        >>> insts = [Addi("a", "x0", 3),Addi("b", "x0", 4),Slt("c", "b", "a")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_val("a1")
        0
        
        If you want, you can allocate Jal/Jalr/Beq instructions, but that's not
        necessary for this exercise.

        >>> insts = [Jal("a", 30)]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> (p.get_pc(), p.get_val("a1") > 0)
        (30, True)

        >>> insts = [Addi("a", "x0", 30), Jalr("b", "a")]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> (p.get_pc(), p.get_val("a1") > 0)
        (30, True)

        >>> insts = [Addi("a", "x0", 3), Addi("b", "a", 0), Beq("a", "b", 30)]
        >>> p = Program(1000, env={}, insts=insts)
        >>> o = RegAllocator(p)
        >>> o.optimize()
        >>> p.eval()
        >>> p.get_pc()
        30
        """
        
        # Hints:
        # new_insts = []
        # for inst in self.prog.get_insts():
        #     action = self.alloc_action[inst.get_opcode()]
        #     last_insts = action(inst, self)
        #     new_insts += last_insts
        # self.prog.set_insts(new_insts)

        new_insts = []

        for inst in self.prog.get_insts():
            
            # Valida todas as variáveis usadas na instrução
            for operand in [getattr(inst, "rd", None), getattr(inst, "rs1", None), getattr(inst, "rs2", None)]:
                if operand and not self.is_valid_variable(operand):
                    raise ValueError(f"Variavel inexistente {operand}")

            # Obtém os operandos e aloca registradores se necessário
            opcode = inst.get_opcode()
            rd = getattr(inst, "rd", None)
            rs1 = getattr(inst, "rs1", None)
            rs2 = getattr(inst, "rs2", None)
            imm = getattr(inst, "imm", None)
            offset = getattr(inst, "offset", None)
            label = getattr(inst, "lab", None)
            
            if rs1 is not None:
                rs1_loc, escrita, leitura = self.allocate_register(rs1)
                if escrita:
                    new_insts.append(escrita)
                if leitura: 
                    new_insts.append(leitura)
            if rs2 is not None:
                rs2_loc, escrita, leitura = self.allocate_register(rs2)
                if escrita:
                    new_insts.append(escrita)
                if leitura: 
                    new_insts.append(leitura)
            if rd is not None:
                rd_loc, escrita, leitura = self.allocate_register(rd)
                if escrita:
                    new_insts.append(escrita)
                if leitura: 
                    new_insts.append(leitura)

            # Mapeia o opcode para a instrução correspondente
            if opcode == "add":
                new_insts.append(Add(rd_loc, rs1_loc, rs2_loc))

            elif opcode == "addi":
                new_insts.append(Addi(rd_loc, rs1_loc, imm))

            elif opcode == "mul":
                new_insts.append(Mul(rd_loc, rs1_loc, rs2_loc))

            elif opcode == "sub":
                new_insts.append(Sub(rd_loc, rs1_loc, rs2_loc))

            elif opcode == "xor":
                new_insts.append(Xor(rd_loc, rs1_loc, rs2_loc))

            elif opcode == "xori":
                new_insts.append(Xori(rd_loc, rs1_loc, imm))

            elif opcode == "div":
                new_insts.append(Div(rd_loc, rs1_loc, rs2_loc))

            elif opcode == "slt":
                new_insts.append(Slt(rd_loc, rs1_loc, rs2_loc))

            elif opcode == "slti":
                new_insts.append(Slti(rd_loc, rs1_loc, imm))

            elif opcode == "beq":
                new_insts.append(Beq(rs1_loc, rs2_loc, label))

            elif opcode == "jal":
                new_insts.append(Jal(rd_loc, label))

            elif opcode == "jalr":
                new_insts.append(Jalr(rd_loc, rs1_loc, offset))

            elif opcode == "sw":
                new_insts.append(Sw(rd_loc, offset, rs1_loc))

            elif opcode == "lw":
                new_insts.append(Lw(rd_loc, offset, rs1_loc))

            else:
                raise ValueError(f"Unsupported opcode: {opcode}")

        self.prog.set_insts(new_insts)