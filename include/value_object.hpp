#pragma once
#include <utils.hpp>
#include <map>
#include <value_type.hpp>
#include <token.hpp>
#include <icalculatable.hpp>
using namespace std;

namespace Effie {
  class ValueObject: public ICalculatable<ValueObject> {
    typedef map<string, ValueObject> Dictionary;

    PROPERTY(ValueType, Type, ValueType::DICTIONARY)
    PROPERTY(int, IntValue, 0)
    PROPERTY(double, DoubleValue, 0.0)
    PROPERTY(string, Id, "")
    PROPERTY(Dictionary, Dictionary, Dictionary())
    PROPERTY(ValueObject *, Pointer, NULL);

  public:
    static ValueObject createNone() {
      ValueObject value;
      value.setType(ValueType::DICTIONARY);
      return value;
    }

    static ValueObject createIntValue(int ivalue) {
      ValueObject value;
      value.setIntValue(ivalue);
      value.setType(ValueType::INT);
      return value;
    }

    static ValueObject createPointer(ValueObject *pointer) {
      ValueObject value;
      value.setPointer(pointer);
      value.setType(ValueType::POINTER);
      return value;
    }

    static ValueObject createValueFrom(Token token) {
      ValueObject value;
      switch(token.getType()) {
      case Type::INT:
        value.setType(ValueType::INT);
        value.setIntValue(token.getIntVal());
        break;
      case Type::DOUBLE:
        value.setType(ValueType::DOUBLE);
        value.setDoubleValue(token.getDoubleVal());
        break;
      case Type::ID:
        value.setType(ValueType::STRING);
        value.setId(token.getId());
        break;
      default:
        value.setType(ValueType::DICTIONARY);
        break;
      }
      return value;
    }
    
    virtual ValueObject add(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }
      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() + value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject sub(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() - value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject mul(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() * value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject div(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() / value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject mod(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() % value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject eq(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() == value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject ne(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() != value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }
    virtual ValueObject ge(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() >= value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }
    virtual ValueObject gt(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() > value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject le(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() <= value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject lt(ValueObject value) {
      if(getType() != value.getType()) {
        throw runtime_error("型の不一致");
      }

      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(getIntValue() < value.getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }

    virtual ValueObject nt() {
      switch(getType()) {
      case ValueType::INT:
        return ValueObject::createIntValue(!getIntValue());
      default:
        throw runtime_error("Unsupported operation");
      }
    }
  };
}
