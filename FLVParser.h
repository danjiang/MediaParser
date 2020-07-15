//
// Created by Dan Jiang on 2020/7/15.
//

#ifndef MEDIAPARSER__FLVPARSER_H_
#define MEDIAPARSER__FLVPARSER_H_

#include <string>

class FLVParser {
 public:
  int Parse(std::string &file);
 private:
  unsigned int ReadSize(FILE *inFile, int length);
  unsigned int ReverseBytes(unsigned char *p, char c);
};

#endif //MEDIAPARSER__FLVPARSER_H_
