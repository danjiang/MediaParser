//
// Created by Dan Jiang on 2020/7/6.
//

#ifndef MEDIAPARSER__AACPARSER_H_
#define MEDIAPARSER__AACPARSER_H_

#include <string>

class AACParser {
 public:
  int Parse(std::string &file);
 private:
  int GetADTSFrame(unsigned char *data, int data_size, unsigned char *frame, int *frame_length);
};

#endif //MEDIAPARSER__AACPARSER_H_
