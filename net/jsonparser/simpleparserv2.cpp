#include "simpleparserv2.h"

simpleparserv2::simpleparserv2(const std::string& t) : tok{t}
{}

void simpleparserv2::parse()
{
  auto emit = [&](auto Type,  JSON_Object::Typename T = JSON_Object::Typename::None)
  {
    JSON_Object tmp;
    tmp.Type = T;
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
      emit(JSON_SYM::literal_false, JSON_Object::Typename::Boolean);
      tok.setPos(tok.getPos()+4);
      continue;
    }
    if(cur == 't' && tok.peek("rue"))
    {
      emit(JSON_SYM::literal_true, JSON_Object::Typename::Boolean);
      tok.setPos(tok.getPos()+3);
      continue;
    }
    if(cur == 'n' && tok.peek("ull"))
    {
      emit(JSON_SYM::literal_null, JSON_Object::Typename::Null);
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
      found_str.Type = JSON_Object::Typename::String;
      H.emplace_back(found_str);
      tok.setPos(tok.getPos()+tmp->str.size()+1); // we didnt include the quotes - so size is already +1 off, but we need +1 for the second one
      continue;
    }
    if(std::isdigit(cur))
    {
      struct NumberResult *num;
      struct JSON_Object found_num;
      num = new NumberResult(tok.getNumber());
      assert(!num->str.empty() && "How is this empty?");
      found_num.Type = JSON_Object::Typename::Number;
      if(num->hasDot)
      {
        found_num.Sym = JSON_SYM::value_float;
      }
      else
      {
        found_num.Sym = JSON_SYM::value_number;
      }
      num->Number = std::stod(num->str);
      found_num.value = num;
      H.emplace_back(found_num);
      tok.setPos(tok.getPos()+num->str.size()-1); // -1 here because size() is not null-indexed, but numbers don't start with an own identifier...
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

    if(H[i].Type == JSON_Object::Typename::String)
    {
      assert(H[i].value != nullptr && "This CANT be the case! Just use that when you allocate something bro!");
      delete H[i].value;
    }
    if(H[i].Type == JSON_Object::Typename::Number)
    {
      assert(H[i].value != nullptr && "This CANT Be the case! Just use that when you allocate something bro!");
      delete H[i].value;
    }
  }
}
int main()
{
  simpleparserv2 Test{"./example.json"};
  Test.parse();
  Test.PrintAST();
  return 0;
}
