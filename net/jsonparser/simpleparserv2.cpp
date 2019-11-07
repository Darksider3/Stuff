#include "simpleparserv2.h"

simpleparserv2::simpleparserv2(const std::string& t) : tok{t}
{}
void simpleparserv2::PrintAST(void (*function_hook)(std::vector<JSON_Object>&))
{
  std::cout << "AST Size: " << H.size() << "\n";
  int indent = 0;
  auto indenter=[&indent]()
  {
    for(int j = indent; j != 0; --j)
      std::cout <<'\t';
  };
  auto printStringResult = [&](const GenericResult& res) {
    indenter();
    std::cout << "StringDump: Str: '" << res.String << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
  };
  auto printNumberResult = [&](const GenericResult& res) {
    indenter();
    std::cout << "NumberDump: Str: '" << res.String << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
  };
  auto printFloatResult = [&](const GenericResult& res) {
    indenter();
    std::cout << "FloatDump: Str: '" << res.String << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
  };

  std::string endStr = ", hurray!\n";
  std::vector<JSON_Object> ASTCop(H);
  if(function_hook != nullptr)
  {
    // @TODO Functor! Filters for example!
    (*function_hook)(ASTCop);
  }
  size_t i = 0;
  for(JSON_Object &c: ASTCop)
  {
    ++i;
    switch(c.Sym)
    {
      case JSON_SYM::value_string:
        printStringResult(*(c).value);
        break;
      case JSON_SYM::value_number:
        printNumberResult(*(c).value);
        break;
      case JSON_SYM::value_float:
        printFloatResult(*(c).value);
        break;
      case JSON_SYM::literal_true:
        indenter();
        std::cout << "literal_true at " << c.Pos << ". \n";
        break;
      case JSON_SYM::literal_false:
        indenter();
        std::cout << "literal_false at " << c.Pos << ". \n";
        break;
      case JSON_SYM::literal_null:
        indenter();
        std::cout << "literal_null at " << c.Pos << ". \n";
        break;
      case JSON_SYM::begin_object:
        indenter();
        ++indent;
        std::cout << "begin_object at " << c.Pos << ". \n";
        break;
      case JSON_SYM::begin_array:
        indenter();
        ++indent;
        std::cout << "begin_array at " << c.Pos << ". \n";
        break;
      case JSON_SYM::end_array:
        --indent;
        indenter();
        std::cout << "end_array at " << c.Pos << ". \n";
        break;
      case JSON_SYM::end_object:
        --indent;
        indenter();
        std::cout << "end_object at " << c.Pos << ". \n";
        break;
      case JSON_SYM::value_separator:
        indenter();
        std::cout << "name_seperator at " << c.Pos << ": ";
        break;
      case JSON_SYM::member_seperator:
        indenter();
        std::cout << "member_seperator at " << c.Pos << ",\n";
        break;
      case JSON_SYM::parse_error:
      case JSON_SYM::Unknown:
        indenter();
        std::cout << "UNKNOWN at " << c.Pos << ". \n";
        break;
      case JSON_SYM::end_of_input:
        if(indent!=0)
          std::cout << "huh....\n";
        std::cout << "End at "<< c.Pos << ".\n";
        break;
    }
  }
  std::cout << "Saw " << i << " objects " << std::endl;
}

void simpleparserv2::parse()
{
  auto emit = [&](auto Type)
  {
    JSON_Object tmp;
    tmp.Sym = Type;
    tmp.Pos = static_cast<int>(tok.getPos());
    H.emplace_back(tmp);
  };
  while(!tok.eof())
  {
    char cur = tok.next();
    if(cur == '\0')
    {
      emit(JSON_SYM::end_of_input);
      break;
    }
    if(cur == '{')
    {
      emit(JSON_SYM::begin_object);
      continue;
    }
    if(cur == '}')
    {
      emit(JSON_SYM::end_object);
      continue;
    }
    if(cur == '[')
    {
      emit(JSON_SYM::begin_array);
      continue;
    }
    if(cur == ']')
    {
      emit(JSON_SYM::end_array);
      continue;
    }
    if(cur == ',')
    {
      emit(JSON_SYM::member_seperator);
      continue;
    }
    if(cur == ':')
    {
      emit(JSON_SYM::value_separator);
      continue;
    }
    if(cur == 'f' && tok.peek("alse"))
    {
      emit(JSON_SYM::literal_false);
      tok.setPos(tok.getPos()+4);
      continue;
    }
    if(cur == 't' && tok.peek("rue"))
    {
      emit(JSON_SYM::literal_true);
      tok.setPos(tok.getPos()+3);
      continue;
    }
    if(cur == 'n' && tok.peek("ull"))
    {
      emit(JSON_SYM::literal_null);
      tok.setPos(tok.getPos()+3);
      continue;
    }
    if(cur == '"' || cur == '\'')
    {
      JSON_Object found_str;
      StringResult *tmp = new StringResult(tok.getStrOnPos());
      found_str.Pos = static_cast<int>(tok.getPos()-1);
      found_str.Sym = JSON_SYM::value_string;
      found_str.value = tmp;
      H.emplace_back(found_str);
      tok.setPos(tok.getPos()+tmp->String.size()+1); // we didnt include the quotes - so size is already(due to being not null-indexed) +1 off, but we need +1 for the second one
      continue;
    }
    if(std::isdigit(cur) || (cur == '-' && std::isdigit(tok.peek())))
    {
      struct NumberResult *num;
      struct JSON_Object found_num;
      num = new NumberResult(tok.getNumber());
      assert(!num->String.empty() && "How is this empty?");
      if(num->hasDot)
      {
        found_num.Sym = JSON_SYM::value_float;
      }
      else
      {
        found_num.Sym = JSON_SYM::value_number;
      }
      num->Number = std::stod(num->String);
      found_num.value = num;
      H.emplace_back(found_num);
      tok.setPos(tok.getPos()+num->String.size()-1); // -1 here because size() is not null-indexed, but numbers don't start with an own identifier...
      continue;
    }
    std::cout << "UNKNOWN CHAR : " << cur << "\n";
  }
  return;
}
simpleparserv2::~simpleparserv2()
{
  for(size_t i = 0; i <= H.size()-1; ++i)
  {

    if(H[i].Sym == JSON_SYM::value_string)
    {
      assert(H[i].value != nullptr && "This CANT be the case! Just use that when you allocate something bro!");
      delete H[i].value;
    }
    if(H[i].Sym == JSON_SYM::value_number || H[i].Sym == JSON_SYM::value_float )
    {
      assert(H[i].value != nullptr && "This CANT Be the case! Just use that when you allocate something bro!");
      delete H[i].value;
    }
  }
}
/*
void Functor(std::vector<JSON_Object>& s)
{
  for(auto &c: s)
  {
    if(c.Sym == JSON_SYM::end_of_input)
    {
      JSON_Object tmp;
      GenericResult *bla=new StringResult;
      bla->String = "HAHAHAHA THIS FUNCTOR WORKS FUCK YE";
      tmp.Pos = -1;
      tmp.Type = JSON_Object::Typename::String;
      tmp.Sym = JSON_SYM::value_string;
      tmp.value = bla;
      s.emplace_back(tmp);
      return;
    }
  }
}
*/
bool isValidValue(JSON_Object p)
{
  return (p.Sym == JSON_SYM::value_string ||
          p.Sym == JSON_SYM::value_number ||
          p.Sym == JSON_SYM::literal_null ||
          p.Sym == JSON_SYM::literal_true ||
          p.Sym == JSON_SYM::literal_false);
}

bool isString(JSON_Object p)
{
  return(p.Sym == JSON_SYM::value_string);
}

size_t simpleparserv2::turn(std::vector<JSON_Object> Head, std::vector<JSON_Object> Dest, size_t p, size_t destp)
{
  size_t i = p;
  size_t arrayAccessPointer = 0;
  for(;i <= Head.size(); ++i)
  {
    Dest = std::vector<JSON_Object>(1);

    // encountered value seperator(:)
    if(Head[i].Sym == JSON_SYM::value_separator)
    {
      // valid value ahead!
      if(isValidValue(Head[i+1]))
      {
        std::cout << "VALID! \n";
        //...do things...
      }
      // named objects
      if(Head[i+1].Sym == JSON_SYM::begin_object)
      {
        // if not a string, error.
        if(isString(Head[i-1]))
        {
          // FIXME Go down the paaaath!
        }
      }

    }
    if(Head[i].Sym == JSON_SYM::begin_array || Head[i].Sym == JSON_SYM::begin_object)
    {
      std::cout << "here\n";
      ++i;
      i = turn(Head, Dest[destp].ArryObj, i, destp);
      ++destp;
      continue;
    }
    if(Head[i].Sym == JSON_SYM::end_array || Head[i].Sym == JSON_SYM::end_object)
    {
      std::cout << "end\n";
      return i;
    }
  }
  assert(false && "SHOULDNT REACH THAT! -.-");
}
int main()
{
  simpleparserv2 Test{"./example.json"};
  Test.parse();
  Test.PrintAST(); // Functor
  std::vector<JSON_Object> tmp;
  //Test.turn(Test.H, tmp);
  return 0;
}
