#ifndef __CmdManager_h__
#define __CmdManager_h__

#include "Arduino.h"

class Mirobot;
class CmdManager;

#include "Mirobot.h"

#define INPUT_BUFFER_LENGTH 180
#define CMD_COUNT 30

typedef enum {JSON_EXPECT_JSON_ATTR, JSON_ATTR, JSON_JSON_DELIM, JSON_VAL} jsonParseState_t;


struct CmdResult {
  const char *msg;
  bool immediate;
};

typedef CmdResult (Mirobot::*MirobotMemFn)(char &);
typedef void (* fp) (void *, char *);
typedef boolean (* fp_ready) (void *);

struct Cmd {
  const char *cmd;
  MirobotMemFn func;
};

class CmdManager {
  public:
    CmdManager();
    //void addCmd(const char cmd[], Mirobot &m, MirobotMemFn func, char arg_count);
    void addCmd(const char cmd[], MirobotMemFn func);
    void addStream(Stream &s);
    void process();
    void collideNotify(const char msg[]);
    void followNotify(int state);
    void setMirobot(Mirobot &);
  private:
    boolean processLine();
    void extractAttr(const char attr[4], char *json, char *output, char len);
    void processCmd(char &cmd, char &arg, char &id);
    void runCmd(char &cmd, char &arg, char &id);
    void sendResponse(const char state[], const char msg[], char &id);
    Stream* _s;
    char webSocketKey[61];
    char input_buffer[INPUT_BUFFER_LENGTH];
    byte input_buffer_pos;
    boolean in_process;
    char current_id[11];
    unsigned long last_char;
    boolean processJSON();
    Mirobot* _m;
    Cmd _cmds[CMD_COUNT];
    int cmd_counter;
};


#endif