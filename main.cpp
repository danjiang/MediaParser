#include "AACParser.h"
#include "H264Parser.h"
#include "FLVParser.h"

int main() {
//  auto parser = new AACParser();
//  std::string file ("/Users/danjiang/Downloads/audio.aac");
//  return parser->Parse(file);

//  auto parser = new H264Parser();
//  std::string file ("/Users/danjiang/Downloads/video.h264");
//  return parser->Parse(file);

  auto parser = new FLVParser();
  std::string file ("/Users/danjiang/Downloads/video.flv");
  return parser->Parse(file);
}
