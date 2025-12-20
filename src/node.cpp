#include <node.hpp>
using namespace Emile;

void
AssignmentExpressionNode::compile(Environment& codes) {
  getLExp()->lcompile(codes);
  getRExp()->compile(codes);
  codes.addMnemonicCode(MnemonicCode(Mnemonic::MOV));
}

void
AssignmentExpressionNode::lcompile(Environment& codes) {
  compile(codes);
}

void
ComparisonExpressionNode::compile(Environment& codes) {
  getLExp()->compile(codes);
  getRExp()->compile(codes);
  switch(getType()) {
  case Type::KW_EQ:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::EQ));
    return;
  case Type::KW_NE:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::NE));
    return;
  case Type::KW_GE:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::GE));
    return;
  case Type::KW_GT:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::GT));
    return;
  case Type::KW_LE:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::LE));
    return;
  case Type::KW_LT:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::LT));
    return;
  default:
    throw runtime_error("unsupported operator");
  }
}

void
ComparisonExpressionNode::lcompile(Environment& codes) {
  compile(codes);
}

void
AddExpressionNode::compile(Environment& codes) {
  getLExp()->compile(codes);
  getRExp()->compile(codes);
  switch(getType()) {
  case Type::KW_ADD:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::ADD));
    return;
  case Type::KW_SUB:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::SUB));
    return;
  default:
    throw runtime_error("unsupported operator");
  }
}

void
AddExpressionNode::lcompile(Environment& codes) {
  compile(codes);
}

void
MulExpressionNode::compile(Environment& codes) {
  getLExp()->compile(codes);
  getRExp()->compile(codes);
  switch(getType()) {
  case Type::KW_MUL:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::MUL));
    return;
  case Type::KW_DIV:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::DIV));
    return;
  case Type::KW_MOD:
    codes.addMnemonicCode(MnemonicCode(Mnemonic::MOD));
    return;
  default:
    throw runtime_error("unsupported operator");
  }
}

void
MulExpressionNode::lcompile(Environment& codes) {
  compile(codes);
}

void
TermNode::compile(Environment& codes) {
  codes.addMnemonicCode(MnemonicCode(Mnemonic::PUSH, getValue()));
}

void
TermNode::lcompile(Environment& codes) {
  compile(codes);
}

void
ArgumentNode::compile(Environment& codes) {
  for(auto arg: getArguments()) {
    arg->compile(codes);
  }
}

void
CallFunctionNode::compile(Environment& codes) {
  getArgument()->compile(codes);
  codes.addMnemonicCode(MnemonicCode(
    Mnemonic::CALL, 
    Object::createIdValue(getName()),
    Object::createIntValue(getArgument()->size())));
}

void
CallFunctionNode::lcompile(Environment& codes) {
  compile(codes);
}

void
SubscriptOperatorNode::compile(Environment& codes) {
  for(auto subscript: getSubscripts()) {
    codes.addMnemonicCode(MnemonicCode(Mnemonic::REF, Object::createIdValue(subscript)));
  }
}

void
VariableNode::compile(Environment& codes) {
  codes.addMnemonicCode(MnemonicCode(Mnemonic::VAR));
  codes.addMnemonicCode(MnemonicCode(Mnemonic::REF, getName()));
  if(getSubscriptOperator() != NULL) {
    getSubscriptOperator()->compile(codes);
  }
  codes.addMnemonicCode(MnemonicCode(Mnemonic::GET));
}

void
VariableNode::lcompile(Environment& codes) {
  codes.addMnemonicCode(MnemonicCode(Mnemonic::VAR));
  codes.addMnemonicCode(MnemonicCode(Mnemonic::REF, getName()));
  if(getSubscriptOperator() != NULL) {
    getSubscriptOperator()->compile(codes);
  }
}

void
BlockStatementNode::compile(Environment& codes) {
  for(auto statement: getStatements()) {
    statement->compile(codes);
  }
}

void
WhileStatementNode::compile(Environment& codes) {
  codes.addMnemonicCode(MnemonicCode(Mnemonic::POP));

  int startIndex = codes.getMnemonicCodeSize();
  getCondition()->compile(codes);

  int jneIndex = codes.getMnemonicCodeSize();
  codes.addMnemonicCode(MnemonicCode(Mnemonic::JNE, Object::createIntValue(0)));
  getBody()->compile(codes);
  codes.addMnemonicCode(MnemonicCode(Mnemonic::JMP, Object::createIntValue(startIndex)));
  codes.setMnemonicCode(jneIndex, MnemonicCode(Mnemonic::JNE, Object::createIntValue(codes.getMnemonicCodeSize())));
}

void
IfStatementNode::compile(Environment& codes) {
  auto skipIndexes = vector<int>();
  codes.addMnemonicCode(MnemonicCode(Mnemonic::POP));
  getCondition()->compile(codes);

  int startIndex = codes.getMnemonicCodeSize();
  codes.addMnemonicCode(MnemonicCode(Mnemonic::JNE, Object::createIntValue(0)));
  getTrueStatement()->compile(codes);
  skipIndexes.push_back(codes.getMnemonicCodeSize());
  codes.addMnemonicCode(MnemonicCode(Mnemonic::JMP, Object::createIntValue(0)));
  for(auto statement: getElifStatements()) {
    codes.setMnemonicCode(startIndex, MnemonicCode(Mnemonic::JNE, Object::createIntValue(codes.getMnemonicCodeSize())));
    codes.addMnemonicCode(MnemonicCode(Mnemonic::POP));
    statement->getCondition()->compile(codes);
    startIndex = codes.getMnemonicCodeSize();
    codes.addMnemonicCode(MnemonicCode(Mnemonic::JNE, Object::createIntValue(0)));
    statement->getTrueStatement()->compile(codes);
    skipIndexes.push_back(codes.getMnemonicCodeSize());
    codes.addMnemonicCode(MnemonicCode(Mnemonic::JMP, Object::createIntValue(0)));
  }
  codes.setMnemonicCode(startIndex, MnemonicCode(Mnemonic::JNE, Object::createIntValue(codes.getMnemonicCodeSize())));
  if(getElseStatement() != NULL) {
    getElseStatement()->compile(codes);
  }
  for(auto skipIndex: skipIndexes) {
    codes.setMnemonicCode(skipIndex, MnemonicCode(Mnemonic::JMP, Object::createIntValue(codes.getMnemonicCodeSize())));
  }
}

void
ElifStatementNode::compile(Environment& codes) {
}

void
ExpressionStatementNode::compile(Environment& codes) {
  codes.addMnemonicCode(MnemonicCode(Mnemonic::POP));
  getExpr()->compile(codes);
}

void
DefineFunctionNode::compile(Environment& codes) {
  codes.setCurrentFunctionName(getName());
  codes.addMnemonicCode(MnemonicCode(Mnemonic::PUSH, Object::createNone()));
  getBlock()->compile(codes);
  codes.addMnemonicCode(MnemonicCode(Mnemonic::EXIT, Object::createNone()));
}

void
RootNode::compile(Environment& codes) {
  codes.addMnemonicCode(MnemonicCode(Mnemonic::PUSH, Object::createNone()));
  for(auto statement: getDefineFunctions()) {
    statement->compile(codes);
  }
  codes.addMnemonicCode(MnemonicCode(Mnemonic::EXIT, Object::createNone()));
}
