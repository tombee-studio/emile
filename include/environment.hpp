#pragma once

#include <map>
#include <utils.hpp>
#include <value_object.hpp>
#include <mnemonic_code.hpp>
using namespace std;

namespace Emile {

  typedef map<string, vector<MnemonicCode>> FunctionCode;

  class Environment {
    PROPERTY(Object, CurrentFunctionName, Object::createNone())
    GETTER(FunctionCode, Functions, FunctionCode())
  public:
    void addMnemonicCode(MnemonicCode code);
    void setMnemonicCode(int index, MnemonicCode code);
    int getMnemonicCodeSize();
  };
}
