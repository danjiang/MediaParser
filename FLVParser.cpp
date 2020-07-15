//
// Created by Dan Jiang on 2020/7/15.
//

#include "FLVParser.h"

#define TAG_TYPE_SCRIPT 18
#define TAG_TYPE_AUDIO  8
#define TAG_TYPE_VIDEO  9

#define AVC_PACKET_TYPE_SPS_PPS 0
#define AVC_PACKET_TYPE_NALU  1

int FLVParser::Parse(std::string &file) {
  FILE *inFile = fopen(file.c_str(), "rb+");
  if (inFile == NULL) {
    printf("Failed to open files!\n");
    return -1;
  }

  unsigned char signature1 = fgetc(inFile);
  unsigned char signature2 = fgetc(inFile);
  unsigned char signature3 = fgetc(inFile);
  unsigned char version = fgetc(inFile);
  unsigned char flags = fgetc(inFile);
  unsigned int headerSize = ReadSize(inFile, 4);

  printf("============== FLV Header ==============\n");
  printf("Signature:  0x %c %c %c\n", signature1, signature2, signature3);
  printf("Version:    0x %X\n", version);
  printf("Flags  :    0x %X\n", flags);
  printf("HeaderSize: 0x %X\n", headerSize);
  printf("========================================\n");

  do {
    unsigned int previousTagSize = ReadSize(inFile, 4);
    printf("PreviousTagSize: %d Position: %ld\n", previousTagSize, ftell(inFile));

    // read tag header
    int tagHeaderType;
    if ((tagHeaderType = fgetc(inFile)) == EOF) {
      break;
    }
    unsigned int tagHeaderDataSize = ReadSize(inFile, 3);
    unsigned int tagHeaderTimeStamp = ReadSize(inFile, 3);
    fseek(inFile, 4, SEEK_CUR); // skip time stamp ex and stream id
    char tagType[10];
    switch (tagHeaderType) {
      case TAG_TYPE_AUDIO:
        sprintf(tagType, "AUDIO");
        break;
      case TAG_TYPE_VIDEO:
        sprintf(tagType, "VIDEO");
        break;
      case TAG_TYPE_SCRIPT:
        sprintf(tagType, "SCRIPT");
        break;
      default:
        sprintf(tagType, "UNKNOWN");
        break;
    }
    printf("[%6s] %6d %6d |", tagType, tagHeaderDataSize, tagHeaderTimeStamp);

    // read tag data
    switch (tagHeaderType) {
      case TAG_TYPE_VIDEO: {
        char videoTagString[100] = {0};

        strcat(videoTagString, "| ");
        unsigned char tagDataFirstByte = fgetc(inFile);
        int x = tagDataFirstByte & 0xF0; // frame type
        x= x >> 4;
        switch (x) {
          case 1:
            strcat(videoTagString, "key frame");
            break;
          case 2:
            strcat(videoTagString, "inter frame");
            break;
          case 3:
            strcat(videoTagString, "disposable inter frame");
            break;
          case 4:
            strcat(videoTagString, "generated keyframe");
            break;
          case 5:
            strcat(videoTagString, "video info/command frame");
            break;
          default:
            strcat(videoTagString, "UNKNOWN");
            break;
        }

        strcat(videoTagString, " | ");
        x = tagDataFirstByte & 0x0F; // code id
        switch (x) {
          case 1:
            strcat(videoTagString, "JPEG (currently unused)");
            break;
          case 2:
            strcat(videoTagString, "Sorenson H.263");
            break;
          case 3:
            strcat(videoTagString, "Screen video");
            break;
          case 4:
            strcat(videoTagString, "On2 VP6");
            break;
          case 5:
            strcat(videoTagString, "On2 VP6 with alpha channel");
            break;
          case 6:
            strcat(videoTagString, "Screen video version 2");
            break;
          case 7:
            strcat(videoTagString, "AVC");
            break;
          default:
            strcat(videoTagString, "UNKNOWN");
            break;
        }

        strcat(videoTagString, " | ");
        unsigned char packetType = fgetc(inFile); // avc packet type
        switch (packetType) {
          case AVC_PACKET_TYPE_SPS_PPS:
            strcat(videoTagString, "SPS PPS");
            break;
          case AVC_PACKET_TYPE_NALU:
            strcat(videoTagString, "NALU");
            break;
          default:
            strcat(videoTagString, "UNKNOWN");
            break;
        }

        fseek(inFile, 3, SEEK_CUR); // skip cts

        switch (packetType) {
          case AVC_PACKET_TYPE_NALU: {
            strcat(videoTagString, " | ");
            unsigned int naluLength = ReadSize(inFile, 4);
            char length[10];
            snprintf(length, 10, "%d", naluLength);
            strcat(videoTagString, length);
            fseek(inFile, -4 - 3 - 1 - 1, SEEK_CUR);
            break;
          }
          default:
            fseek(inFile, -3 - 1 - 1, SEEK_CUR);
            break;
        }

        printf("%s", videoTagString);
      }
      default:
        break;
    }
    //skip the data of this tag
    fseek(inFile, tagHeaderDataSize, SEEK_CUR);

    printf("\n");
  } while (!feof(inFile));

  fclose(inFile);

  return 0;
}

unsigned int FLVParser::ReadSize(FILE *inFile, int length) {
  unsigned char bytes[length];
  fread(&bytes, sizeof(bytes), 1, inFile);
  unsigned int size = ReverseBytes(bytes, sizeof(bytes));
  return size;
}

//reverse_bytes - turn a BigEndian byte array into a LittleEndian integer
unsigned int FLVParser::ReverseBytes(unsigned char *p, char c) {
  int r = 0;
  int i;
  for (i=0; i<c; i++)
    r |= ( *(p+i) << (((c-1)*8)-8*i));
  return r;
}
