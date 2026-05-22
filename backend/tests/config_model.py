from dataclasses import dataclass

# O decorador @dataclass(frozen=True) cria a classe, o __init__, __repr__ e outros
# métodos automaticamente. O "frozen=True" torna o objeto imutável após a criação,
# o que é uma excelente prática para objetos de configuração, evitando alterações acidentais.
@dataclass(frozen=True)
class ConfiguracaoEsteira:
    # A declaração dos campos com tipos é o que permite ao `asdict` funcionar.
    velocidade_motor: int
    cor_alvo: str
    tolerancia_opencv: float

    def __post_init__(self):
        """
        Este método especial é chamado pelo @dataclass logo após a
        inicialização do objeto. É o lugar ideal para colocar validações.
        """
        if self.velocidade_motor <= 0:
            raise ValueError("A velocidade do motor deve ser um inteiro positivo.")

        if not self.cor_alvo:
            raise ValueError("A cor alvo não pode ser uma string vazia.")

        if not (0 <= self.tolerancia_opencv <= 1):
            raise ValueError("A tolerância deve ser um número entre 0 e 1.")
        
        
"""from dataclasses import dataclass

# O decorador @dataclass cria toda a estrutura da classe automaticamente
@dataclass
class ConfiguracaoEsteira:
    velocidade_motor: int
    cor_alvo: str
    tolerancia_opencv: float
    """