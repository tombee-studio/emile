#include <node.hpp>
using namespace Emile;

void
AssignmentExpressionNode::compile(vector<MnemonicCode>& codes) {
  getLExp()->lcompile(codes);
  getRExp()->compile(codes);
  codes.push_back(MnemonicCode(Mnemonic::MOV));
}

void
AssignmentExpressionNode::lcompile(vector<MnemonicCode>& codes) {
  compile(codes);
}

void
ComparisonExpressionNode::compile(vector<MnemonicCode>& codes) {
  getLExp()->compile(codes);
  getRExp()->compile(codes);
  switch(getType()) {
  case Type::KW_EQ:
    codes.push_back(MnemonicCode(Mnemonic::EQ));
    return;
  case Type::KW_NE:
    codes.push_back(MnemonicCode(Mnemonic::NE));
    return;
  case Type::KW_GE:
    codes.push_back(MnemonicCode(Mnemonic::GE));
    return;
  case Type::KW_GT:
    codes.push_back(MnemonicCode(Mnemonic::GT));
    return;
  case Type::KW_LE:
    codes.push_back(MnemonicCode(Mnemonic::LE));
    return;
  case Type::KW_LT:
    codes.push_back(MnemonicCode(Mnemonic::LT));
    return;
  default:
    throw runtime_error("unsupported operator");
  }
}

void
ComparisonExpressionNode::lcompile(vector<MnemonicCode>& codes) {
  compile(codes);
}

void
AddExpressionNode::compile(vector<MnemonicCode>& codes) {
  getLExp()->compile(codes);
  getRExp()->compile(codes);
  switch(getType()) {
  case Type::KW_ADD:
    codes.push_back(MnemonicCode(Mnemonic::ADD));
    return;
  case Type::KW_SUB:
    codes.push_back(MnemonicCode(Mnemonic::SUB));
    return;
  default:
    throw runtime_error("unsupported operator");
  }
}

void
AddExpressionNode::lcompile(vector<MnemonicCode>& codes) {
  compile(codes);
}

void
MulExpressionNode::compile(vector<MnemonicCode>& codes) {
  getLExp()->compile(codes);
  getRExp()->compile(codes);
  switch(getType()) {
  case Type::KW_MUL:
    codes.push_back(MnemonicCode(Mnemonic::MUL));
    return;
  case Type::KW_DIV:
    codes.push_back(MnemonicCode(Mnemonic::DIV));
    return;
  case Type::KW_MOD:
    codes.push_back(MnemonicCode(Mnemonic::MOD));
    return;
  default:
    throw runtime_error("unsupported operator");
  }
}

void
MulExpressionNode::lcompile(vector<MnemonicCode>& codes) {
  compile(codes);
}

void
TermNode::compile(vector<MnemonicCode>& codes) {
  codes.push_back(MnemonicCode(Mnemonic::PUSH, getValue()));
}

void
TermNode::lcompile(vector<MnemonicCode>& codes) {
  compile(codes);
}

void
ArgumentNode::compile(vector<MnemonicCode>& codes) {
  for(auto arg: getArguments()) {
    arg->compile(codes);
  }
}

void
CallFunctionNode::compile(vector<MnemonicCode>& codes) {
  getArgument()->compile(codes);
  codes.push_back(MnemonicCode(
    Mnemonic::CALL, 
    Object::createIdValue(getName()),
    Object::createIntValue(getArgument()->size())));
}

void
CallFunctionNode::lcompile(vector<MnemonicCode>& codes) {
  compile(codes);
}

void
SubscriptOperatorNode::compile(vector<MnemonicCode>& codes) {
  for(auto subscript: getSubscripts()) {
    codes.push_back(MnemonicCode(Mnemonic::REF, Object::createIdValue(subscript)));
  }
}

void
VariableNode::compile(vector<MnemonicCode>& codes) {
  codes.push_back(MnemonicCode(Mnemonic::VAR));
  codes.push_back(MnemonicCode(Mnemonic::REF, getName()));
  if(getSubscriptOperator() != NULL) {
    getSubscriptOperator()->compile(codes);
  }
  codes.push_back(MnemonicCode(Mnemonic::GET));
}

void
VariableNode::lcompile(vector<MnemonicCode>& codes) {
  codes.push_back(MnemonicCode(Mnemonic::VAR));
  codes.push_back(MnemonicCode(Mnemonic::REF, getName()));
  if(getSubscriptOperator() != NULL) {
    getSubscriptOperator()->compile(codes);
  }
}

void
BlockStatementNode::compile(vector<MnemonicCode>& codes) {
  for(auto statement: getStatements()) {
    statement->compile(codes);
  }
}

void
WhileStatementNode::compile(vector<MnemonicCode>& codes) {
  codes.push_back(MnemonicCode(Mnemonic::POP));

  int startIndex = codes.size();
  getCondition()->compile(codes);

  int jneIndex = codes.size();
  codes.push_back(MnemonicCode(Mnemonic::JNE, Object::createIntValue(0)));
  getBody()->compile(codes);
  codes.push_back(MnemonicCode(Mnemonic::JMP, Object::createIntValue(startIndex)));
  codes[jneIndex] = MnemonicCode(Mnemonic::JNE, Object::createIntValue(codes.size()));
}

void
IfStatementNode::compile(vector<MnemonicCode>& codes) {
  auto skipIndexes = vector<int>();
  codes.push_back(MnemonicCode(Mnemonic::POP));
  getCondition()->compile(codes);

  int startIndex = codes.size();
  codes.push_back(MnemonicCode(Mnemonic::JNE, Object::createIntValue(0)));
  getTrueStatement()->compile(codes);
  skipIndexes.push_back(codes.size());
  codes.push_back(MnemonicCode(Mnemonic::JMP, Object::createIntValue(0)));
  for(auto statement: getElifStatements()) {
    codes[startIndex] = MnemonicCode(Mnemonic::JNE, Object::createIntValue(codes.size()));
    codes.push_back(MnemonicCode(Mnemonic::POP));
    statement->getCondition()->compile(codes);
    startIndex = codes.size();
    codes.push_back(MnemonicCode(Mnemonic::JNE, Object::createIntValue(0)));
    statement->getTrueStatement()->compile(codes);
    skipIndexes.push_back(codes.size());
    codes.push_back(MnemonicCode(Mnemonic::JMP, Object::createIntValue(0)));
  }
  codes[startIndex] = MnemonicCode(Mnemonic::JNE, Object::createIntValue(codes.size()));
  if(getElseStatement() != NULL) {
    getElseStatement()->compile(codes);
  }
  for(auto skipIndex: skipIndexes) {
    codes[skipIndex] = MnemonicCode(Mnemonic::JMP, Object::createIntValue(codes.size()));
  }
}

void
ElifStatementNode::compile(vector<MnemonicCode>& codes) {
}

void
ExpressionStatementNode::compile(vector<MnemonicCode>& codes) {
  codes.push_back(MnemonicCode(Mnemonic::POP));
  getExpr()->compile(codes);
}

void
DefineFunctionNode::compile(vector<MnemonicCode>& codes) {
  codes.push_back(MnemonicCode(Mnemonic::PUSH, Object::createNone()));
  getBlock()->compile(codes);
  codes.push_back(MnemonicCode(Mnemonic::EXIT, Object::createNone()));
}

void
RootNode::compile(vector<MnemonicCode>& codes) {
  codes.push_back(MnemonicCode(Mnemonic::PUSH, Object::createNone()));
  for(auto statement: getStatemants()) {
    statement->compile(codes);
  }
  codes.push_back(MnemonicCode(Mnemonic::EXIT, Object::createNone()));
}
