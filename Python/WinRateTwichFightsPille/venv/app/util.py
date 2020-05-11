from config import *
import sys
import os
from config import *


# @TODO: rename sql_dir -> dbDict
# @TODO: overall to big function
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


def outcomePerUser(sql_dir):
    # @TODO: outsource. key_list -> keylist() method
    key_list = list()
    # @TODO: outsource as well, value_list = WinLoss -> WinLossSubtraction()
    value_list = list()

    # @TODO: Evaluate other methods to gather a list of keynames
    for key in sql_dir:
        if key["againstName"] not in key_list:
            key_list.append(key["againstName"])

    outcome_dict = dict((el, 0) for el in key_list)

    # @TODO: Extract and put into own method(percentage_dict)
    percentage_dict = dict((el, 0) for el in key_list)
    for key in key_list:
        percentage_dict[key] = dict({"losses": 0, "wins": 0,"total": 0,
                                    "win_rate": 0.00, "win_loss_ratio": 0.00})

    for key in sql_dir:
        user = key["againstName"]
        state = outcomeToNumber(key["outcome"])
        outcome_dict[user] += state
        if state == -1:
            percentage_dict[user]["losses"] += 1
        elif state == 1:
            percentage_dict[user]["wins"] += 1
        percentage_dict[user]["total"] += 1

    for key in outcome_dict:
        value_list.append(outcome_dict[key])

    for key in outcome_dict:
        if percentage_dict[key]["wins"] == 0 or percentage_dict[key]["losses"] == 0:
            continue
        # win/loss ratio => win/loss
        # Win-Rate => Wins / ChancesTotal * 100
        percentage_dict[key]["win_loss_ratio"] = percentage_dict[key]["wins"] / percentage_dict[key]["losses"]
        percentage_dict[key]["win_rate"] = round((percentage_dict[key]["wins"] / percentage_dict[key]["total"]) * 100,
                                                 WIN_RATE_ROUND)

    return outcome_dict, key_list, value_list, percentage_dict