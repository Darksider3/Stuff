from config import *
import sys
import os


def restart_program():
    """Restarts the current program.
    Note: this function does not return. Any cleanup action (like
    saving data) must be done before calling this function."""
    python = sys.executable
    os.execl(python, python, * sys.argv)

def outcomeToNumber(outcome: str):
    if outcome == WIN_STR:
        return 1
    else:
        return -1
