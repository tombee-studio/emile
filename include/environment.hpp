#pragma once

#include <utils.hpp>
#include <value_object.hpp>
#include <mnemonic_code.hpp>

namespace Emile {
  class Environment {
    GETTER(vector<MnemonicCode>, Codes, vector<MnemonicCode>())
  public:
    void addMnemonicCode(MnemonicCode code);
    void setMnemonicCode(int index, MnemonicCode code);
    int getMnemonicCodeSize();
  };
}
