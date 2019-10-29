#include "simpleparserv2.h"

simpleparserv2::simpleparserv2(const std::string& t) : tok{t}
{}

void simpleparserv2::parse()
{
  auto emit = [&](auto Type,  JSON_Object::Value::Typename T = JSON_Object::Value::Typename::None)
  {
    JSON_Object tmp;
    tmp.value.Type = T;
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
      emit(JSON_SYM::literal_false, JSON_Object::Value::Typename::Boolean);
      tok.setPos(tok.getPos()+4);
      continue;
    }
    if(cur == 't' && tok.peek("rue"))
    {
      emit(JSON_SYM::literal_true, JSON_Object::Value::Typename::Boolean);
      tok.setPos(tok.getPos()+3);
      continue;
    }
    if(cur == 'n' && tok.peek("ull"))
    {
      emit(JSON_SYM::literal_null, JSON_Object::Value::Typename::Null);
      tok.setPos(tok.getPos()+3);
      continue;
    }
    if(cur == '"' || cur == '\'')
    {
      JSON_Object found_str;
      StringResult tmp = tok.getStrOnPos();
      found_str.Pos = static_cast<int>(tok.getPos()-1);
      found_str.Sym = JSON_SYM::value_string;
      found_str.value.String = tmp;
      found_str.value.Type = JSON_Object::Value::Typename::String;
      H.emplace_back(found_str);
      tok.setPos(tok.getPos()+tmp.str.size()+1); // we didnt include the quotes - so size is already +1 off, but we need +1 for the second one
      continue;
    }
    if(std::isdigit(cur))
    {
      struct NumberResult num;
      struct JSON_Object found_num;
      num = tok.getNumber();
      assert(!num.str.empty() && "How is this empty?");
      found_num.value.Type = JSON_Object::Value::Typename::Number;
      if(num.hasDot)
      {
        found_num.Sym = JSON_SYM::value_float;
      }
      else
      {
        found_num.Sym = JSON_SYM::value_number;
      }
      num.Number = std::stod(num.str);
      found_num.value.number = num;
      H.emplace_back(found_num);
      tok.setPos(tok.getPos()+num.str.size()-1); // -1 here because size() is not null-indexed, but numbers don't start with an own identifier...
      continue;
    }
    std::cout << "UNKNOWN CHAR : " << cur << "\n";
  }
  return;
}

int main()
{
  simpleparserv2 Test{"./example.json"};
  Test.parse();
}
