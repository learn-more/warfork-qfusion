#include <cstdint>
#include <stddef.h>
#include <cassert>
#include <cstring>
#include "os.h"

PipeType GPipeRead = NULLPIPE;
PipeType GPipeWrite = NULLPIPE;



class pipebuff_t
{
  public:
  char buffer[1024];
  unsigned int cursize = 0;
  unsigned int br = 0;
  

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
    assert(cursize + vallen < 1024);
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
        br -= evlen + sizeof(uint32_t);
        if (br > 0)
            memmove(buffer, buffer+evlen+sizeof(uint32_t), br);

        printf("GOTMESSAGE\n");
    }

    return 1;
  }
};


int Write1ByteMessage(const uint8_t message){
  pipebuff_t buf;
  buf.WriteByte(message);
  return buf.Transmit();
}


//???
int what(){
  pipebuff_t wtf;
  wtf.Recieve();
}
