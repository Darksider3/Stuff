from config import *
import sys
import os
from config import *


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


def get_name_list(sql_dir):
    name_list = list()
    for key in sql_dir:
        if key["againstName"] not in name_list:
            name_list.append(key["againstName"])

    return name_list


def outcome_dict(sql_dir, name_list=0):
    if type(name_list) is not list:
        name_list = get_name_list(sql_dir)
    outcome_ret = dict((element, 0) for element in name_list)

    for key in sql_dir:
        user = key["againstName"]
        state = outcomeToNumber(key["outcome"])
        outcome_ret[user] += state

    return outcome_ret


def percentageDict(sql_dir, name_list=0, outcomes=0):
    if type(name_list) is not list:
        name_list = get_name_list(sql_dir)
    if type(outcomes) is not dict:
        outcomes = outcome_dict(sql_dir, name_list)
    percentage_ret = dict((element, 0) for element in name_list)
    for key in name_list:
        percentage_ret[key] = dict({"losses": 0, "wins": 0, "total": 0,
                                    "win_rate": 0.00, "win_loss_ratio": 0.00})

    for key in sql_dir:
        user = key["againstName"]
        state = outcomeToNumber(key["outcome"])
        if state == -1:
            percentage_ret[user]["losses"] += 1
        if state == 1:
            percentage_ret[user]["wins"] += 1
        percentage_ret[user]["total"] += 1
    
    for key in outcomes:
        if percentage_ret[key]["wins"] == 0 or percentage_ret[key]["losses"] == 0:
            continue
        # win/loss ratio => win/loss
        # Win-Rate => Wins / ChancesTotal * 100
        percentage_ret[key]["win_loss_ratio"] = percentage_ret[key]["wins"] / percentage_ret[key]["losses"]
        percentage_ret[key]["win_rate"] = round((percentage_ret[key]["wins"] / percentage_ret[key]["total"]) * 100,
                                                WIN_RATE_ROUND)
    return percentage_ret


def outcomePerUser(sql_dir):
    value_list = list()
    key_list = get_name_list(sql_dir)

    outcome_dicts = outcome_dict(sql_dir)
    percentage_dict = percentageDict(sql_dir)

    for key in outcome_dicts:
        value_list.append(outcome_dicts[key])

    return outcome_dicts, key_list, value_list, percentage_dict
