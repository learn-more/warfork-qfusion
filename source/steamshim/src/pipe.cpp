#include <cstdint>
#include <stddef.h>
#include <cassert>
#include <cstring>
#include "os.h"

PipeType GPipeRead = NULLPIPE;
PipeType GPipeWrite = NULLPIPE;

#define MESSAGE_MAX 1024

class pipebuff_t
{
  public:
  char buffer[MESSAGE_MAX];
  unsigned int cursize = 0;
  unsigned int br = 0;
  bool hasmsg = false;
  

  // cursize, data, whatever is in pipebuff_t now
  pipebuff_t()
  {
    // init all crap
  }
  ~pipebuff_t()
  {
    // destroy all crap
  }

  void WriteData(void* val, size_t vallen)
  {
    assert(cursize + vallen < MESSAGE_MAX);
    memcpy(buffer + cursize, val, vallen);
    cursize += vallen;
  }

  void WriteByte(char val)
  {
    WriteData(&val, sizeof val);
  }

  void WriteInt(int val)
  {
    WriteData(&val, sizeof val);
  }

  void WriteFloat(float val)
  {
    WriteData(&val, sizeof val);
  }

  void WriteLong(long long val){
    WriteData(&val, sizeof val);
  }

  void *ReadData(size_t vallen)
  {
    assert(cursize + vallen < MESSAGE_MAX);
    void* val = cursize + buffer;
    cursize += vallen;
    return val;
  }

  char *ReadString()
  {
    char *str = cursize + buffer;
    unsigned int len = strlen(str);
    cursize +=len;

    return str;
  }

  char ReadByte(){
    return *(char*)ReadData(sizeof(char));
  }

  int ReadInt(){
    return *(int*)ReadData(sizeof(int));
  }

  int ReadFloat(){
    return *(float*)ReadData(sizeof(float));
  }

  int ReadLong(){
    return *(long long*)ReadData(sizeof(long long));
  }




  int Transmit()
  {
    writePipe(GPipeWrite, &cursize, sizeof cursize);
    return writePipe(GPipeWrite, buffer, cursize);
  }

  int Recieve()
  {
    int evlen = (br > 0) ? (*(uint32_t*) &buffer[0]) : 0;

    if (br <= evlen)  /* we have an incomplete commmand. Try to read more. */
    {
        if (pipeReady(GPipeRead))
        {
            const int morebr = readPipe(GPipeRead, buffer + br, sizeof (buffer) - br);

            if (morebr > 0)
                br += morebr;
            else  /* uh oh */
            {
              return 0;
            }
        }
    }

    if (evlen && (br > evlen))
    {
        printf("GOTMESSAGE \n");
        write(91, buffer,evlen+sizeof(uint32_t));
        br -= evlen + sizeof(uint32_t);
        if (br > 0)
            memmove(buffer, buffer+evlen+sizeof(uint32_t), br);

    }

    return 1;
  }
};


int Write1ByteMessage(const uint8_t message){
  pipebuff_t buf;
  buf.WriteByte(message);
  return buf.Transmit();
}

//
// //???
int what(){
  pipebuff_t wtf;
  wtf.Recieve();
  wtf.ReadByte();
  wtf.ReadInt();
  wtf.WriteLong(0);
  wtf.ReadString();
  wtf.ReadLong();
}
