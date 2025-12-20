#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
using namespace std;

Emile::Object
select(vector<Emile::Object> args) {
  int value;
  cin >> value;
  return Emile::Object::createIntValue(value);
}

Emile::Object
clear(vector<Emile::Object> args) {
  std::system("clear");
  return Emile::Object::createIntValue(0);
}

Emile::Object
println(vector<Emile::Object> args) {
  cout << args[0].getStringValue() << endl;
  return Emile::Object::createIntValue(0);
}

int
main(int argc, char *argv[]) {
  Emile::Interpreter interpreter;
  interpreter.getFunctionTable()["select"] = select;
  interpreter.getFunctionTable()["println"] = println;
  interpreter.getFunctionTable()["clear"] = clear;

  std::ifstream ifs(argv[1]); // example.txt を読み込みモードで開く
  std::string source;
  std::string line;
  // ファイルが開けたかチェック
  if (!ifs) {
      std::cerr << "ファイルを開けませんでした。" << std::endl;
      return 1; // エラー終了
  }

  // ファイルが終端に達するまで1行ずつ読み込む
  while (std::getline(ifs, line)) {
    source += line;
  }

  ifs.close();

  Emile::Environment env;
  Emile::Parser(
    Emile::Lexer(source).lex()).parse()->compile(env);

  interpreter.setMnemonics(env.getFunctions()["main"]);
  interpreter.run();

  return interpreter.getReturnValue().getIntValue();
}
