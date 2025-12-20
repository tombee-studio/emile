#include <parser.hpp>
using namespace Emile;

RootNode* 
Parser::parse() {
  try {
    int index = 0;
    auto root = new RootNode();
    DefineFunctionNode *statement;
    while((statement = parseDefineFunctionStatement()) != NULL) {
      root->getDefineFunctions().push_back(statement);
    }
    return root;
  } catch(runtime_error& er) {
    cerr << er.what() << endl;
    exit(-1);
  }
}

DefineFunctionNode*
Parser::parseDefineFunctionStatement() {
  if(!isValidAt(getIndex(), Type::KW_FUNC)) {
    return NULL;
  }
  consumeNext();

  if(!isValidAt(getIndex(), Type::ID)) {
    return NULL;
  }
  Token token = consumeNext();

  auto name = Object::createValueFrom(token);
  auto argument = parseArgumentNode();
  auto block = parseBlock();

  return new DefineFunctionNode(name, argument, block);
}

StatementNode* 
Parser::parseStatement() {
  StatementNode *statement = NULL;
  if((statement = parseExpressionStatement()) != NULL) {
    return statement;
  }
  if((statement = parseIfStatement()) != NULL) {
    return statement;
  }
  if((statement = parseBlock()) != NULL) {
    return statement;
  }
  if((statement = parseWhileStatement()) != NULL) {
    return statement;
  }
  return NULL;
}

StatementNode*
Parser::parseWhileStatement() {
  if(!isValidAt(getIndex(), Type::KW_WHILE)) {
    return NULL;
  }
  consumeNext();

  auto condition = parseExpressionNode();
  auto block = parseBlock();

  return new WhileStatementNode(condition, block);
}

StatementNode*
Parser::parseBlock() {
  if(!isValidAt(getIndex(), Type::KW_COLON)) {
    return NULL;
  }
  consumeNext();
  vector<StatementNode *> statements;
  while(!(isValidAt(getIndex(), Type::KW_END) || 
    isValidAt(getIndex(), Type::KW_ELIF) || 
    isValidAt(getIndex(), Type::KW_ELSE))) {
      statements.push_back(parseStatement());
    }
  if(isValidAt(getIndex(), Type::KW_END)) {
    consumeNext();
  }
  return new BlockStatementNode(statements);
}

StatementNode* 
Parser::parseIfStatement() {
  StatementNode *statement = NULL;
  if(!isValidAt(getIndex(), Type::KW_IF)) {
    return NULL;
  }
  consumeNext();

  auto condition = parseExpressionNode();
  StatementNode* trueStatement = parseBlock();
  vector<ElifStatementNode *> elifStatements;
  StatementNode* elseStatement = NULL;

  while(isValidAt(getIndex(), Type::KW_ELIF)) {
    consumeNext();
    auto _expr = parseExpressionNode();
    elifStatements.push_back(
      new ElifStatementNode(_expr, parseBlock()));
  }
  if(isValidAt(getIndex(), Type::KW_ELSE)) {
    consumeNext();
    elseStatement = parseBlock();
  }
  return new IfStatementNode(condition, trueStatement, elifStatements, elseStatement);
}

ExpressionStatementNode* 
Parser::parseExpressionStatement() {
  ExpressionNode *node = NULL;
  if((node = parseExpressionNode()) != NULL) {
    if(!isValidAt(getIndex(), Type::KW_SEMICOLON)) {
      throw runtime_error("expected token: ';'");
    }
    consumeNext();
    return new ExpressionStatementNode(node);
  }
  return NULL;
}

ExpressionNode*
Parser::parseExpressionNode() {
  ExpressionNode *node = NULL;
  if((node = parseAssignmentNode()) != NULL) {
    return node;
  }
}

ExpressionNode *
Parser::parseAssignmentNode() {
  auto left = parseComparisonNode();
  if(!isValidAt(getIndex(), Type::KW_EQUAL)) {
    return left;
  }
  Token token = consumeNext();
  auto right = parseComparisonNode();
  return new AssignmentExpressionNode(left, right);
}

ExpressionNode *
Parser::parseComparisonNode() {
  auto left = parseAddNode();
  while(isValidAt(getIndex(), Type::KW_EQ) ||
    isValidAt(getIndex(), Type::KW_NE) ||
    isValidAt(getIndex(), Type::KW_GE) ||
    isValidAt(getIndex(), Type::KW_GT) ||
    isValidAt(getIndex(), Type::KW_LE) ||
    isValidAt(getIndex(), Type::KW_LT)) {
    Token token = consumeNext();
    auto right = parseAddNode();
    left = new ComparisonExpressionNode(left, right, token.getType());
  }
  return left;
}

ExpressionNode *
Parser::parseAddNode() {
  auto left = parseMulNode();
  while(isValidAt(getIndex(), Type::KW_ADD) ||
    isValidAt(getIndex(), Type::KW_SUB)) {
    Token token = consumeNext();
    auto right = parseMulNode();
    left = new AddExpressionNode(left, right, token.getType());
  }
  return left;
}

ExpressionNode *
Parser::parseMulNode() {
  auto left = parseTermNode();
  while(isValidAt(getIndex(), Type::KW_MUL) ||
    isValidAt(getIndex(), Type::KW_DIV) ||
    isValidAt(getIndex(), Type::KW_MOD)) {
    Token token = consumeNext();
    auto right = parseTermNode();
    left = new MulExpressionNode(left, right, token.getType());
  }
  return left;
}

ArgumentNode *
Parser::parseArgumentNode() {
  vector<ExpressionNode *> arguments;
  if(!isValidAt(getIndex(), Type::KW_LPAREN)) {
    return NULL;
  }
  consumeNext();
  while(true) {
    arguments.push_back(parseExpressionNode());
    if(isValidAt(getIndex(), Type::KW_COMMA)) {
      consumeNext();
      continue;
    }
    if(isValidAt(getIndex(), Type::KW_RPAREN)) {
      consumeNext();
      break;
    }
  }
  return new ArgumentNode(arguments);
}

SubscriptOperatorNode *
Parser::parseSubscriptOperator() {
  vector<string> subscripts;
  if(!isValidAt(getIndex(), Type::KW_LBRACKET)) {
    return NULL;
  }
  while(isValidAt(getIndex(), Type::KW_LBRACKET)) {
    consumeNext();
    auto token = consume(Type::STRING);
    subscripts.push_back(token.getStringVal());
    if(!isValidAt(getIndex(), Type::KW_RBRACKET)) {
      throw runtime_error("expected ']'");
    }
    consumeNext();
  }
  return new SubscriptOperatorNode(subscripts);
}

ExpressionNode *
Parser::parseTermNode() {
  if(isValidAt(getIndex(), Type::INT) ||
    isValidAt(getIndex(), Type::STRING) ||
    isValidAt(getIndex(), Type::DOUBLE)) {
      Token token = consumeValue();
      return new TermNode(
        Object::createValueFrom(token));
  }

  if(isValidAt(getIndex(), Type::ID)) {
    Token token = consumeNext();
    auto argument = parseArgumentNode();
    if(argument == NULL) {
      return new VariableNode(
        Object::createValueFrom(token),
        parseSubscriptOperator());
    } else {
      return new CallFunctionNode(token.getId(), argument);
    }
  }

  if(isValidAt(getIndex(), Type::KW_LPAREN)) {
    consume(Type::KW_LPAREN);
  } else {
    return NULL;
  }

  auto node = parseExpressionNode();
  if(isValidAt(getIndex(), Type::KW_RPAREN)) {
    consume(Type::KW_RPAREN);
  } else {
    throw runtime_error("expected '(' at " + getIndex());
  }
  return node;
}

bool
Parser::isValidAt(int index, Type type) {
  return getTokens()[index].getType() == type;
}

Token
Parser::consume(Type type) {
  auto tokens = getTokens();
  auto index = getIndex();
  if(tokens[index].getType() == type) {
    Token token = tokens[index];
    getIndex()++;
    return token;
  }
  throw runtime_error("");
}

Token
Parser::consumeNext() {
  Token token = getTokens()[getIndex()];
  getIndex()++;
  return token;
}

Token
Parser::consumeValue() {
  auto tokens = getTokens();
  auto index = getIndex();
  if(tokens[index].getType() == Type::ID || 
    tokens[index].getType() == Type::INT ||
    tokens[index].getType() == Type::DOUBLE ||
    tokens[index].getType() == Type::STRING) {
    Token token = tokens[index];
    getIndex()++;
    return token;
  }
  throw runtime_error("");
}
