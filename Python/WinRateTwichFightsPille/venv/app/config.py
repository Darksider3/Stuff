DBG_ENABLED = True
DBG_PP_DICT = True

WIN_RATE_ROUND = 4
WIN_STR = "won"
LOST_STR = "lost"

DB_PATH = "/home/darksider3/Desktop/winrate.db"

DB_CREATION = """
CREATE TABLE "WinRate" (
	"id"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	"time"	INTEGER DEFAULT CURRENT_TIMESTAMP,
	"againstName"	TEXT,
	"outcome"	TEXT DEFAULT 'win'
)"""
