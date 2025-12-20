#pragma once
#include <node.hpp>

namespace Emile {
  class Parser {
    PRIVATE_PROPERTY(int, Index, 0);
    PRIVATE_PROPERTY(vector<Token>, Tokens, vector<Token>())

    Token none;
  public:
    Parser(vector<Token> tokens) {
      setTokens(tokens);
    }

    RootNode* parse();
  private:
    bool isValidAt(int, Type type);

    Token consume(Type);
    Token consumeNext();
    Token consumeValue();

    RootNode* parseRoot();
    StatementNode* parseDefineFunctionStatement();
    StatementNode* parseStatement();
    StatementNode* parseBlock();
    StatementNode* parseIfStatement();
    StatementNode* parseWhileStatement();
    ExpressionStatementNode* parseExpressionStatement();

    ExpressionNode* parseExpressionNode();
    ExpressionNode* parseAssignmentNode();
    ExpressionNode* parseComparisonNode();
    ExpressionNode* parseAddNode();
    ExpressionNode* parseMulNode();
    ExpressionNode* parseTermNode();
  
    ArgumentNode* parseArgumentNode();
    SubscriptOperatorNode *parseSubscriptOperator();
  };
}
