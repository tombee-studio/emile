#include <environment.hpp>
using namespace Emile;

void
Environment::addMnemonicCode(MnemonicCode code) {
  getFunctions()[getCurrentFunctionName().getId()].push_back(code);
}

int
Environment::getMnemonicCodeSize() {
  return getFunctions()[getCurrentFunctionName().getId()].size();
}

void
Environment::setMnemonicCode(int index, MnemonicCode code) {
  getFunctions()[getCurrentFunctionName().getId()][index] = code;
}
