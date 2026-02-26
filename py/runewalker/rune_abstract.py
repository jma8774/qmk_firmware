from abc import ABC, abstractmethod


class RuneWalkerPilot(ABC):
    """
    Abstract base for character-specific movement used by RuneWalker.
    Subclasses implement these with the concrete key sequences for their class
    (Marksman flash-jump, Evan teleport, etc.).
    """

    @abstractmethod
    def rune_flash_jump(self, direction=None):
        """Execute a flash jump (or equivalent mobility skill) in *direction* ('left'/'right'/None)."""
        ...

    @abstractmethod
    def rune_jump(self):
        """Basic jump."""
        ...

    @abstractmethod
    def rune_rope(self):
        """Climb upward (rope / teleport-up)."""
        ...

    @abstractmethod
    def rune_jump_down(self):
        """Drop through a platform."""
        ...

    @abstractmethod
    def rune_protect(self):
        """Activate a protection skill before interacting with the rune."""
        ...

    @abstractmethod
    def rune_interact(self):
        """Press the interaction key to activate the rune."""
        ...

    @abstractmethod
    def done(self):
        """Done with the rune."""
        ...
