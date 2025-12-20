#include <environment.hpp>
using namespace Emile;

void
Environment::addMnemonicCode(MnemonicCode code) {
  getCodes().push_back(code);
}

int
Environment::getMnemonicCodeSize() {
  return getCodes().size();
}

void
Environment::setMnemonicCode(int index, MnemonicCode code) {
  getCodes()[index] = code;
}
