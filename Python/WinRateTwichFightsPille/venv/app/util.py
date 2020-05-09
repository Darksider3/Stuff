from config import *


def outcomeToNumber(outcome: str):
    if outcome == WIN_STR:
        return 1
    else:
        return -1
