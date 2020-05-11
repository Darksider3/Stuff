import typing

from PyQt5.QtGui import QValidator

from config import WIN_STR, LOST_STR


class QFightStateValidator(QValidator):
    def __init__(self):
        super().__init__()

    def validate(self, input_str: str, position: int) -> typing.Tuple['QValidator.State', str, int]:
        if input_str not in WIN_STR and input_str not in LOST_STR:
            return QValidator.Invalid, input_str, position
        else:
            return QValidator.Acceptable, input_str, position

    def fixup(self, fix_str: str) -> str:
        for x in range(len(WIN_STR)):
            if fix_str[x] in WIN_STR and fix_str[x] in LOST_STR:
                continue
            elif fix_str[x] in WIN_STR:
                return WIN_STR
            elif fix_str[x] in LOST_STR:
                return LOST_STR
            else:
                continue