import sqlite3
from config import *


class SqliteHandler:
    Con: sqlite3.Connection
    Curs: sqlite3.Cursor

    def __init__(self, path: str = DB_PATH):
        try:
            self.Con = sqlite3.connect(path)
            self.Curs = self.Con.cursor()
            self.Curs.row_factory = sqlite3.Row
        except sqlite3.Error as e:
            print(f"Couldn't connect to database... {e}")

    def insert(self, against: str, outcome: str):
        insertion: str = "INSERT INTO WinRate (againstName, outcome) VALUES (?, ?);"
        self.Curs.execute(insertion, tuple([against, outcome]))
        self.Con.commit()

    def getNameCompleteList(self):
        completer_list_query: str = "SELECT `againstName` FROM WinRate"
        completer_query_result = self.Curs.execute(completer_list_query)
        completer_query_result = completer_query_result.fetchall()
        self.Con.commit()
        return_list = list()
        for key in completer_query_result:
            if not key["againstName"] in return_list:
                return_list.append(key["againstName"])

        return return_list

    def __del__(self):
        try:
            self.Con.commit()
            self.Con.close()
        except sqlite3.Error as e:
            print(f"Couldn't close DB {e}")
