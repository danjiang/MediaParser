//
// Created by Dan Jiang on 2020/7/6.
//

#include "AACParser.h"

#include <iostream>

int AACParser::Parse(std::string &file) {
  FILE *fp = fopen(file.c_str(), "rb");
  if (!fp) {
    std::cerr << "can not open file " << file << std::endl;
    exit(1);
  }

  int num = 0;
  int data_size = 0;
  int data_offset = 0;
  int frame_length = 0;

  auto aac_buffer = (unsigned char *)malloc(1024 * 1024);
  auto aac_frame = (unsigned char *)malloc(1024 * 5);

  printf("  Num|     Profile| Frequency|  Channel| Size|\n");

  while (!feof(fp)) {
    data_size = fread(aac_buffer + data_offset, 1, 1024 * 1024 - data_offset, fp);
    auto aac_data = aac_buffer;

    while (true) {
      auto ret = GetADTSFrame(aac_data, data_size, aac_frame, &frame_length);
      if (ret == -1) {
        break;
      } else if (ret == 1) {
        memcpy(aac_buffer, aac_data, data_size);
        data_offset = data_size;
        break;
      }

      unsigned char profile = aac_frame[2]&0xC0;
      profile = profile >> 6;
      char profile_str[12] = {0};
      switch(profile){
        case 0: sprintf(profile_str, "AAC Main"); break;
        case 1: sprintf(profile_str, "AAC LC"); break;
        case 2: sprintf(profile_str, "AAC LTR"); break;
        case 3: sprintf(profile_str, "SBR"); break;
        case 4: sprintf(profile_str, "AAC scalable"); break;
      }

      unsigned char sampling_frequency_index = aac_frame[2]&0x3C;
      sampling_frequency_index = sampling_frequency_index >> 2;
      char frequence_str[7] = {0};
      switch(sampling_frequency_index){
        case 0: sprintf(frequence_str, "96000Hz"); break;
        case 1: sprintf(frequence_str, "88000Hz"); break;
        case 2: sprintf(frequence_str, "64000Hz"); break;
        case 3: sprintf(frequence_str, "48000Hz"); break;
        case 4: sprintf(frequence_str, "44100Hz"); break;
        case 5: sprintf(frequence_str, "32000Hz"); break;
        case 6: sprintf(frequence_str, "24000Hz"); break;
        case 7: sprintf(frequence_str, "22000Hz"); break;
        case 8: sprintf(frequence_str, "16000Hz"); break;
        case 9: sprintf(frequence_str, "12000Hz"); break;
        case 10: sprintf(frequence_str, "11025Hz"); break;
        case 11: sprintf(frequence_str, "8000Hz"); break;
        case 12: sprintf(frequence_str, "7350Hz"); break;
      }

      int channel = 0;
      channel |= ((aac_frame[2] & 0x01) << 2); // high 1 bit
      channel |= ((aac_frame[3] & 0x80) >> 6); // low 2 bits

      printf("%5d|%12s|%10s|%9d|%5d|\n", num, profile_str ,frequence_str, channel, frame_length);

      data_size -= frame_length;
      aac_data += frame_length;
      num++;
    }
  }

  return 0;
}

int AACParser::GetADTSFrame(unsigned char *data, int data_size, unsigned char *frame, int *frame_length) {
  int length = 0; // ADTS Frame 长度，包括头和实际裸流数据

  if (!data || !frame || !frame_length) {
    return -1;
  }

  while (true) {
    if(data_size  < 7) { // ADTS Header 可能为 7 个字节或 9 个字节
      return -1;
    }
    if((data[0] == 0xff) && ((data[1] & 0xf0) == 0xf0)) { // first 12 bits is syncword 0xFFF
      length |= ((data[3] & 0x03) << 11); // high 2 bits
      length |= data[4] << 3; // middle 8 bits
      length |= ((data[5] & 0xe0) >> 5); // low 3 bits
      break;
    }
    --data_size;
    ++data;
  }

  if (data_size < length) { // 数据大小不足一个 ADTS Frame 长度，需要更多的数据
    return 1;
  }

  memcpy(frame, data, length);
  *frame_length = length;

  return 0;
}
