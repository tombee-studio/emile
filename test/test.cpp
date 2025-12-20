#include <cassert>
#include <lexer.hpp>
#include <parser.hpp>
#include <interpreter.hpp>
#include <initializer_list>
using namespace Emile;
using namespace std;

void
testToken(string input, Type type) {
  auto tokens = Lexer(input).lex();
  assert(tokens[0].getType() == type);

  cout << "OK! " << input << endl;
}

void
testLex(string input, initializer_list<Type> tokenTypes) {
  auto tokens = Lexer(input).lex();
  assert(tokens.size() == tokenTypes.size());
  for(int i = 0; i < tokenTypes.size(); i++) {
    auto targetTokenType = *(tokenTypes.begin() + i);
    assert(tokens[i].getType() == targetTokenType);
  }
  cout << "OK! " << input << endl;
}

void
testInterpreter(initializer_list<MnemonicCode> array, Object target) {
  vector<MnemonicCode> codes;
  for(int i = 0; i < array.size(); i++) {
    codes.push_back(*(array.begin() + i));
  }
  Interpreter interpreter;
  interpreter.setMnemonics(codes);
  interpreter.run();

  auto value = interpreter.getStack().top();
  assert(value.getType() == target.getType());
  switch(value.getType()) {
  case ValueType::INT:
    assert(value.getIntValue() == target.getIntValue());
    break;
  default:
    break;
  }
  cout << "OK!" << endl;
}

void
testParser(string text) {
  auto tokens = Lexer(text).lex();
  Parser parser(tokens);
  auto root = parser.parse();
  cout << "OK! " << text << endl;
}

Object
test(vector<Object> arguments) {
  auto value = arguments[0];
  return value.mul(Object::createIntValue(3));
}

void
testInterpreterRun(string source, Object target) {
  auto tokens = Lexer(source).lex();
  Parser parser(tokens);
  auto root = parser.parse();
  Environment env;
  root->compile(env);
  
  Interpreter interpreter;
  interpreter.getFunctionTable()["test"] = test;
  interpreter.setMnemonics(env.getFunctions()["main"]);
  interpreter.run();
  assert(interpreter.getReturnValue().eq(target).getIntValue() == 1);
  cout << "OK! " << source << endl;
}

int
main() {
  testToken("[", Type::KW_LBRACKET);
  testToken("]", Type::KW_RBRACKET);
  testToken(":", Type::KW_COLON);
  testToken(";", Type::KW_SEMICOLON);
  testToken(".", Type::KW_DOT);
  testToken(",", Type::KW_COMMA);
  testToken("(", Type::KW_LPAREN);
  testToken(")", Type::KW_RPAREN);
  testToken("+", Type::KW_ADD);
  testToken("-", Type::KW_SUB);
  testToken("*", Type::KW_MUL);
  testToken("/", Type::KW_DIV);
  testToken("%", Type::KW_MOD);
  testToken(">", Type::KW_GT);
  testToken("<", Type::KW_LT);
  testToken("==", Type::KW_EQ);
  testToken("!=", Type::KW_NE);
  testToken(">=", Type::KW_GE);
  testToken("<=", Type::KW_LE);
  testToken("!", Type::KW_NOT);
  testToken("\"Hello, World!\"", Type::STRING);
  testToken("\"こんにちは、世界。\"", Type::STRING);
  testToken("1", Type::INT);
  testToken("1234567890", Type::INT);
  testToken("1.01", Type::DOUBLE);
  testToken("123456789.0123456789", Type::DOUBLE);
  testToken("abc", Type::ID);
  testToken("true", Type::KW_TRUE);
  testToken("false", Type::KW_FALSE);
  testToken("while", Type::KW_WHILE);
  testToken("func", Type::KW_FUNC);

  testLex("abc abc abc", { Type::ID, Type::ID, Type::ID, Type::NONE });
  testLex("abc 1000 1234567890.0123456789", { Type::ID, Type::INT, Type::DOUBLE, Type::NONE });
  testLex(
    "(1, 10.3, 1)", 
    { Type::KW_LPAREN, Type::INT, Type::KW_COMMA, Type::DOUBLE, Type::KW_COMMA, Type::INT, Type::KW_RPAREN, Type::NONE });
  testLex(
    ">=><=<!+-*/%!====", 
    { Type::KW_GE, Type::KW_GT, Type::KW_LE, Type::KW_LT, Type::KW_NOT, Type::KW_ADD,
      Type::KW_SUB, Type::KW_MUL, Type::KW_DIV, Type::KW_MOD, Type::KW_NE, Type::KW_EQ, Type::KW_EQUAL, 
      Type::NONE });
  testLex(
    "a = \"Hello, World!\";",
    { Type::ID, Type::KW_EQUAL, Type::STRING, Type::KW_SEMICOLON, Type::NONE }
  );
  testLex(
    "obj[\"引数\"]",
    { Type::ID, Type::KW_LBRACKET, Type::STRING, Type::KW_RBRACKET, Type::NONE }
  );
  testLex(
    "obj0123456789",
    { Type::ID, Type::NONE }
  );
  testLex(
    "0123456789obj",
    { Type::INT, Type::ID, Type::NONE }
  );
  testLex(
    "true1",
    { Type::KW_TRUE, Type::INT, Type::NONE }
  );
  testLex(
    "(3)",
    { Type::KW_LPAREN, Type::INT, Type::KW_RPAREN, Type::NONE }
  );
  testLex(
    "\"体力\" \"テスト\"",
    { Type::STRING, Type::STRING, Type::NONE }
  );
  testLex(
    "\"\" \"テスト\"",
    { Type::STRING, Type::STRING, Type::NONE }
  );
  testLex("", { Type::NONE });

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(1)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(2)),
    MnemonicCode(Mnemonic::ADD),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(3));
  
  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(2)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(1)),
    MnemonicCode(Mnemonic::SUB),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(1));

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(2)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(1)),
    MnemonicCode(Mnemonic::MUL),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(2));

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(4)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(2)),
    MnemonicCode(Mnemonic::DIV),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(2));

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(2)),
    MnemonicCode(Mnemonic::MOD),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(1));

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(2)),
    MnemonicCode(Mnemonic::EQ),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(0));

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::EQ),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(1));

  Object targetValue1;
  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::PUSH, Object::createPointer(&targetValue1)),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::MOV),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(3));
  assert(targetValue1.getIntValue() == 3);

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::VAR),
    MnemonicCode(Mnemonic::REF, Object::createIdValue("test")),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::MOV),
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::VAR),
    MnemonicCode(Mnemonic::REF, Object::createIdValue("test")),
    MnemonicCode(Mnemonic::GET),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(3));
  assert(targetValue1.getIntValue() == 3);

  testInterpreter({
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::VAR),
    MnemonicCode(Mnemonic::REF, Object::createIdValue("test")),
    MnemonicCode(Mnemonic::REF, Object::createIdValue("test")),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(5)),
    MnemonicCode(Mnemonic::MOV),
    MnemonicCode(Mnemonic::POP),
    MnemonicCode(Mnemonic::VAR),
    MnemonicCode(Mnemonic::REF, Object::createIdValue("test")),
    MnemonicCode(Mnemonic::REF, Object::createIdValue("test")),
    MnemonicCode(Mnemonic::GET),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(5));
  assert(targetValue1.getIntValue() == 3);

  testInterpreter({
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::PUSH, Object::createBoolValue(true)),
    MnemonicCode(Mnemonic::JE, Object::createIntValue(5)),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(3));

  testInterpreter({
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::JMP, Object::createIntValue(5)),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::PUSH, Object::createIntValue(3)),
    MnemonicCode(Mnemonic::NOP),
    MnemonicCode(Mnemonic::EXIT)
  }, Object::createIntValue(3));

  testParser("3;");
  testParser("a;");
  testParser("\"Hello, World\";");
  testParser("1.3;");
  testParser("(3);");
  testParser("a;");
  testParser("3*2;");
  testParser("3/2;");
  testParser("3%2;");
  testParser("3 * 2;");
  testParser("3 * 2 / 5 % 2;");
  testParser("3 * 2 / 5 % 2 + 1 - 2;");
  testParser("3 * 2 == 36 / 6;");
  testParser("a = 2; if a == 2: a = 3; else: a = 4; end a;");
  testParser("func main(): a = 2; if a == 2: a = 3; else: a = 4; end a; end");

  testInterpreterRun("func main(): 3; end", Object::createIntValue(3));
  testInterpreterRun("func main(): (1); end", Object::createIntValue(1));
  testInterpreterRun("func main(): 3 * 2; end", Object::createIntValue(6));
  testInterpreterRun("func main(): 15 / 5; end", Object::createIntValue(3));
  testInterpreterRun("func main(): 15 / 5 + 3; end", Object::createIntValue(6));
  testInterpreterRun("func main(): 3 % 2; end", Object::createIntValue(1));
  testInterpreterRun("func main(): 3 % 2 + 1; end", Object::createIntValue(2));
  testInterpreterRun("func main(): 3 * 2 == 36 / 6; end", Object::createIntValue(1));
  testInterpreterRun("func main(): a = 3; a; end", Object::createIntValue(3));
  testInterpreterRun("func main(): test(2); end", Object::createIntValue(6));
  testInterpreterRun("func main(): a = 3; test(a + 1); end", Object::createIntValue(12));
  testInterpreterRun(
    "func main(): a = 2; if a == 2: a = 3; else: a = 1; end a; end", 
    Object::createIntValue(3));
  testInterpreterRun(
    "func main(): a = 1; if a == 1: b = 4; b = 7; elif a == 3: b = 5; else: b = 6; end b; end", 
    Object::createIntValue(7));
  testInterpreterRun(
    "func main(): a = 1; while a < 5: a = a + 1; end a; end", 
    Object::createIntValue(5));
  testInterpreterRun(
    "func main(): a[\"テスト\"] = 5; a[\"テスト\"]; end", 
    Object::createIntValue(5));

  return 0;
}
