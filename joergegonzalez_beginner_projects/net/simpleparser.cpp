#include <iostream>
#define SEPERATOR ','

/*
 ******************************IS_* functions
 */
bool isDigit();
bool isFloating_point_digit();
bool isSeperator();
bool isBracket();
bool isEscape();
bool isQuote();

/*
 ******************************IN_* functions
 */

bool inQuote();
bool inString();
bool inNum(); //either integer-digit or floating-digit!

/*
 *******************************SPECIAL
 */

int scopeLevel();
bool isSomeScopeDangling();


char next();
void openFile();



bool is_seperator(char ch)
{
  switch(ch)
  {
    case ',':
    case '=':
    case ';':
      return true;
  }
  return false;
}
