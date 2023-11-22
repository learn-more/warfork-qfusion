#include "os.h"
#include <cstdint>

typedef enum ShimCmd
{
    SHIMCMD_BYE,
    SHIMCMD_PUMP,
    SHIMCMD_REQUESTSTATS,
    SHIMCMD_STORESTATS,
    SHIMCMD_SETACHIEVEMENT,
    SHIMCMD_GETACHIEVEMENT,
    SHIMCMD_RESETSTATS,
    SHIMCMD_SETSTATI,
    SHIMCMD_GETSTATI,
    SHIMCMD_SETSTATF,
    SHIMCMD_GETSTATF,
    SHIMCMD_REQUESTSTEAMID,
    SHIMCMD_REQUESTPERSONANAME,
    SHIMCMD_SETRICHPRESENCE,
    SHIMCMD_REQUESTAUTHSESSIONTICKET,
    SHIMCMD_BEGINAUTHSESSION,
} ShimCmd;


extern PipeType GPipeRead;
extern PipeType GPipeWrite;

class pipebuff_t
{
  public:
  char buffer[1024];
  unsigned int cursize;
  pipebuff_t();
  ~pipebuff_t();

  void WriteData(void* val, size_t vallen);
  void WriteByte(char val);
  void WriteInt(int val);
  void WriteFloat(float val);
  int Transmit();
  int Recieve();

};

int ReadMessage(char* buf);
int Write1ByteMessage(const uint8_t message);
