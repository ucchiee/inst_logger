#include "util.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

string get_date_time_str() {
  time_t t = time(nullptr);
  const tm *localTime = localtime(&t);
  stringstream s;
  s << localTime->tm_year + 1900;
  // zero fill
  s << setw(2) << setfill('0') << localTime->tm_mon + 1;
  s << setw(2) << setfill('0') << localTime->tm_mday;
  s << setw(2) << setfill('0') << localTime->tm_hour;
  s << setw(2) << setfill('0') << localTime->tm_min;
  s << setw(2) << setfill('0') << localTime->tm_sec;

  return s.str();
}

BOOL INS_InMain(INS ins) {
  ADDRINT inst_addr = INS_Address(ins);
  IMG img = IMG_FindByAddress(inst_addr);
  return IMG_Valid(img) && IMG_IsMainExecutable(img);
}

ADDRINT INS_RelAddress(INS ins) {
  ADDRINT inst_addr = INS_Address(ins);
  IMG img = IMG_FindByAddress(inst_addr);
  return inst_addr - IMG_LowAddress(img);
}
