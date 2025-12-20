#pragma once
#include <utils.hpp>
#include <lexer.hpp>
#include <value_object.hpp>
#include <mnemonic_code.hpp>
#include <vector>
#include <map>
#include <queue>
using namespace std;

namespace Emile {
  class Node {
  public:
    virtual void compile(vector<MnemonicCode>&) {}
  };

  class ExpressionNode: public Node {
  public:
    virtual ~ExpressionNode() {}

    virtual void compile(vector<MnemonicCode>&) = 0;
    virtual void lcompile(vector<MnemonicCode>& codes) = 0;
  };

  class BinaryExpressionNode: public ExpressionNode {
  protected:
    GETTER(ExpressionNode *, LExp, NULL)
    GETTER(ExpressionNode *, RExp, NULL)
  public:
    BinaryExpressionNode(ExpressionNode *lexp, ExpressionNode *rexp) {
      setLExp(lexp);
      setRExp(rexp);
    }

    ~BinaryExpressionNode() override {
      delete _LExp;
      delete _RExp;
    }
  };

  class AssignmentExpressionNode: public BinaryExpressionNode {
  public:
    AssignmentExpressionNode(ExpressionNode *lexp, ExpressionNode *rexp): 
      BinaryExpressionNode(lexp, rexp){
    }

    ~AssignmentExpressionNode() override {
      BinaryExpressionNode::~BinaryExpressionNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
    virtual void lcompile(vector<MnemonicCode>& codes) override;
  };

  class ComparisonExpressionNode: public BinaryExpressionNode {
    PRIVATE_PROPERTY(Type, Type, Type::NONE)
  public:
    ComparisonExpressionNode(ExpressionNode *lexp, ExpressionNode *rexp, Type type): 
    BinaryExpressionNode(lexp, rexp){
      setType(type);
    }

    ~ComparisonExpressionNode() override {
      BinaryExpressionNode::~BinaryExpressionNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
    virtual void lcompile(vector<MnemonicCode>& codes) override;
  };

  class AddExpressionNode: public BinaryExpressionNode {
    PRIVATE_PROPERTY(Type, Type, Type::NONE)
  public:
    AddExpressionNode(ExpressionNode *lexp, ExpressionNode *rexp, Type type): 
    BinaryExpressionNode(lexp, rexp){
      setType(type);
    }

    ~AddExpressionNode() override {
      BinaryExpressionNode::~BinaryExpressionNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
    virtual void lcompile(vector<MnemonicCode>& codes) override;
  };

  class MulExpressionNode: public BinaryExpressionNode {
    PRIVATE_PROPERTY(Type, Type, Type::NONE)
  public:
    MulExpressionNode(ExpressionNode *lexp, ExpressionNode *rexp, Type type): 
    BinaryExpressionNode(lexp, rexp){
      setType(type);
    }

    ~MulExpressionNode() override {
      BinaryExpressionNode::~BinaryExpressionNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
    virtual void lcompile(vector<MnemonicCode>& codes) override;
  };

  class TermNode: public ExpressionNode {
    PRIVATE_PROPERTY(Object, Value, Object::createNone())
  public:
    TermNode(Object value) {
      setValue(value);
    }

    ~TermNode() override {

    }

    virtual void compile(vector<MnemonicCode>& codes) override;
    virtual void lcompile(vector<MnemonicCode>& codes) override;
  };

  class ArgumentNode: public Node {
    PRIVATE_PROPERTY(vector<ExpressionNode *>, Arguments, vector<ExpressionNode *>())
  public:
    ArgumentNode(vector<ExpressionNode *> arguments) {
      setArguments(arguments);
    }
    
    virtual void compile(vector<MnemonicCode>&) override;

    int size() {
      return getArguments().size();
    }
  };

  class CallFunctionNode: public ExpressionNode {
    PRIVATE_PROPERTY(string, Name, "")
    PRIVATE_PROPERTY(ArgumentNode *, Argument, NULL)
  public:
    CallFunctionNode(string name, ArgumentNode *argument) {
      setName(name);
      setArgument(argument);
    }

    virtual void compile(vector<MnemonicCode>&) override;
    virtual void lcompile(vector<MnemonicCode>& codes) override;
  };

  class SubscriptOperatorNode: Node {
    PRIVATE_PROPERTY(vector<string>, Subscripts, vector<string>());
  public:
    SubscriptOperatorNode(vector<string> subscripts) {
      setSubscripts(subscripts);
    }

    virtual void compile(vector<MnemonicCode>&) override;
  };

  class VariableNode: public ExpressionNode {
    PRIVATE_PROPERTY(Object, Name, Object::createNone())
    PRIVATE_PROPERTY(SubscriptOperatorNode *, 
      SubscriptOperator, 
      NULL)
  public:
    VariableNode(Object name, 
      SubscriptOperatorNode *subscriptOperator) {
      setName(name);
      setSubscriptOperator(subscriptOperator);
    }

    ~VariableNode() override {}

    virtual void compile(vector<MnemonicCode>& codes) override;
    virtual void lcompile(vector<MnemonicCode>& codes) override;
  };

  class StatementNode: public Node {
  public:
    virtual ~StatementNode() {}

    virtual void compile(vector<MnemonicCode>& codes) = 0;
  };

  class WhileStatementNode: public StatementNode {
    PRIVATE_PROPERTY(ExpressionNode *, Condition, NULL)
    PRIVATE_PROPERTY(StatementNode *, Body, NULL)
  public:
    WhileStatementNode(
      ExpressionNode *condition,
      StatementNode *body) {
        setCondition(condition);
        setBody(body);
    }

    virtual ~WhileStatementNode() override {
      StatementNode::~StatementNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
  };

  class BlockStatementNode: public StatementNode {
    PRIVATE_PROPERTY(vector<StatementNode *>, Statements, vector<StatementNode *>())
  public:
    BlockStatementNode(
      vector<StatementNode *> statements) {
        setStatements(statements);
    }

    virtual ~BlockStatementNode() override {
      StatementNode::~StatementNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
  };

  class ElifStatementNode: public StatementNode {
    GETTER(ExpressionNode *, Condition, NULL)
    GETTER(StatementNode *, TrueStatement, NULL)
  public:
    ElifStatementNode(
      ExpressionNode *condition, 
      StatementNode *trueStatement) {
        setCondition(condition);
        setTrueStatement(trueStatement);
    }

    virtual ~ElifStatementNode() override {
      StatementNode::~StatementNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
  };

  class IfStatementNode: public StatementNode {
    PRIVATE_PROPERTY(ExpressionNode *, Condition, NULL)
    PRIVATE_PROPERTY(StatementNode *, TrueStatement, NULL)
    PRIVATE_PROPERTY(vector<ElifStatementNode *>, ElifStatements, vector<ElifStatementNode *>())
    PRIVATE_PROPERTY(StatementNode *, ElseStatement, NULL)
  public:
    IfStatementNode(ExpressionNode *condition, 
      StatementNode *trueStatement, 
      vector<ElifStatementNode *> elifStatements,
      StatementNode *elseStatement) {
        setCondition(condition);
        setTrueStatement(trueStatement);
        setElifStatements(elifStatements);
        setElseStatement(elseStatement);
    }

    virtual ~IfStatementNode() override {
      StatementNode::~StatementNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
  };

  class ExpressionStatementNode: public StatementNode {
    PRIVATE_PROPERTY(ExpressionNode *, Expr, NULL);
  public:
    ExpressionStatementNode(ExpressionNode *node): _Expr(node) {}

    virtual ~ExpressionStatementNode() override {
      delete _Expr;
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
  };


  class DefineFunctionNode: public StatementNode {
    PRIVATE_PROPERTY(Object, Name, Object::createNone())
    PRIVATE_PROPERTY(ArgumentNode *, Argument, NULL)
    PRIVATE_PROPERTY(StatementNode*, Block, NULL)
  public:
    DefineFunctionNode(
      Object name,
      ArgumentNode *argument,
      StatementNode *block) {
        setName(name);
        setArgument(argument);
        setBlock(block);
    }

    virtual ~DefineFunctionNode() override {
      StatementNode::~StatementNode();
    }

    virtual void compile(vector<MnemonicCode>& codes) override;
  };

  class RootNode: public Node {
    GETTER(vector<StatementNode *>,
      Statemants,
      vector<StatementNode *>())
  public:
    ~RootNode() {
      for(auto node: _Statemants) {
        delete node;
      }
    }

    virtual void compile(vector<MnemonicCode>&) override;
  };
}
