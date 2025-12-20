#include <lexer.hpp>
using namespace Emile;

vector<Token> 
Lexer::lex() {
  Token token;
  while(true) {
    if(_index >= _text.size()) {
      tokens.push_back(Token::createNone());
      return tokens;
    }
    if(skipSpace()) continue;
    if(lexKW("func", Type::KW_FUNC)) continue;
    if(lexKW("if", Type::KW_IF)) continue;
    if(lexKW("elif", Type::KW_ELIF)) continue;
    if(lexKW("else", Type::KW_ELSE)) continue;
    if(lexKW("while", Type::KW_WHILE)) continue;
    if(lexKW("end", Type::KW_END)) continue;
    if(lexKW("==", Type::KW_EQ)) continue;
    if(lexKW("true", Type::KW_TRUE)) continue;
    if(lexKW("false", Type::KW_FALSE)) continue;
    if(lexKW("!=", Type::KW_NE)) continue;
    if(lexKW(">=", Type::KW_GE)) continue;
    if(lexKW("<=", Type::KW_LE)) continue;
    if(lexKW(":", Type::KW_COLON)) continue;
    if(lexKW(";", Type::KW_SEMICOLON)) continue;
    if(lexKW("=", Type::KW_EQUAL)) continue;
    if(lexKW(".", Type::KW_DOT)) continue;
    if(lexKW(",", Type::KW_COMMA)) continue;
    if(lexKW("[", Type::KW_LBRACKET)) continue;
    if(lexKW("]", Type::KW_RBRACKET)) continue;
    if(lexKW("*", Type::KW_MUL)) continue;
    if(lexKW("/", Type::KW_DIV)) continue;
    if(lexKW("%", Type::KW_MOD)) continue;
    if(lexKW("+", Type::KW_ADD)) continue;
    if(lexKW("-", Type::KW_SUB)) continue;
    if(lexKW("(", Type::KW_LPAREN)) continue;
    if(lexKW(")", Type::KW_RPAREN)) continue;
    if(lexKW(">", Type::KW_GT)) continue;
    if(lexKW("<", Type::KW_LT)) continue;
    if(lexKW("!", Type::KW_NOT)) continue;
    if(lexString()) continue;
    if(lexDouble()) continue;
    if(lexInt()) continue;
    if(lexId()) continue;
    cerr << "can't lex word found: " << _cText[_index] << "@" << _index << endl;
    exit(-1);
  }
}

bool
Lexer::skipSpace() {
  if(_text[_index] == ' ' || 
    _text[_index] == '\t' || 
    _text[_index] == '\n' || 
    _text[_index] == '\r') {
    _index++;
    return true;
  }
  return false;
}

bool
Lexer::lexKW(string keyword, Type type) {
  if(strncmp(&(_text[_index]), keyword.data(), keyword.size()) == 0) {
    tokens.push_back(Token::createKeywordToken(type));
    _index += keyword.size();
    return true;
  }
  return false;
}

bool
Lexer::lexString() {
  if(_text[_index] != '\"') {
    return false;
  }
  
  int start = _index;
  int size = 1;
  while(_text[start + size] != '\"') {
    size++;
  }
  size += 1;

  char str[size - 1];
  for(int i = 1; i < size; i++) {
    str[i - 1] = _text[start + i];
    if(_text[start + i] =='\"' ) {
      str[i - 1] = 0;
      break;
    }
  }

  tokens.push_back(Token::createStringToken(str));
  _index = start + size;
  return true;
}

bool
Lexer::lexInt() {
  if(_text[_index] < '0' || '9' < _text[_index]) {
    return false;
  }
  
  int start = _index;
  int size = 1;
  while('0' <= _text[start + size] && _text[start + size] <= '9') {
    size++;
  }

  char str[size + 1];
  strncpy(str, &(_text[_index]), size);

  int value = atoi(str);
  tokens.push_back(Token::createIntToken(value));
  _index = start + size;
  return true;
}

bool
Lexer::lexDouble() {
  if(_text[_index] < '0' || '9' < _text[_index]) {
    return false;
  }
  
  bool hasDot = false;
  int start = _index;
  int size = 1;
  while(('0' <= _text[start + size] && _text[start + size] <= '9') ||
    _text[start + size] == '.') {
      if(_text[start + size] == '.') {
        hasDot = true;
      }
      size++;
  }

  if(!hasDot) {
    return false;
  }

  char str[size + 1];
  strncpy(str, &(_text[_index]), size);

  double value = stod(str);
  tokens.push_back(Token::createDoubleToken(value));
  _index = start + size;
  return true;
}

bool
Lexer::lexId() {
  if(!isIdentifier(_text[_index])) {
    return false;
  }
  
  int start = _index;
  int size = 1;
  while(isIdentifier(_text[start + size]) || isNumber(_text[start + size])) {
    size++;
  }

  char str[size + 1];
  str[size] = 0;
  strncpy(str, &(_text[_index]), size);
  tokens.push_back(Token::createIdToken(string(str)));
  _index = start + size;
  return true;
}

bool
Lexer::isIdentifier(char c) {
  return ('A' <= c && c <= 'Z') 
    || ('a' <= c && c <= 'z')
    || c == '_';
}

bool
Lexer::isNumber(char c) {
  return '0' <= c && c <= '9';
}
