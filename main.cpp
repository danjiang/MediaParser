#include <iostream>

#include "AACParser.h"

int main() {
  auto parser = new AACParser();
  std::string file ("/Users/danjiang/Downloads/audio.aac");
  return parser->Parse(file);
}
