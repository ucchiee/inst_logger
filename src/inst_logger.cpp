/*-
 * Copyright (c) 2010, 2011, 2012, 2013, Columbia University
 * All rights reserved.
 *
 * This software was developed by Vasileios P. Kemerlis <vpk@cs.columbia.edu>
 * at Columbia University, New York, NY, USA, in June 2010.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Columbia University nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <types.h>

#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "pin.H"
#include "util.hpp"

using namespace std;

static map<ADDRINT, string> inst_map;
// Addresses of instructions (dynamically executed inst)
static vector<ADDRINT> inst_vec;
ostream *out = &cerr;

KNOB<BOOL> KnobMainOnly(KNOB_MODE_WRITEONCE, "pintool", "main_only", "1",
                        "Log only in main binary.");
KNOB<string> KnobOutputName(KNOB_MODE_WRITEONCE, "pintool", "o", "",
                            "Output Name");
KNOB<BOOL> KnobRelativeAddress(
    KNOB_MODE_WRITEONCE, "pintool", "rel_addr", "0",
    "Output relative address instead of abstract one.");

static void log_executing_inst(ADDRINT addr) { inst_vec.push_back(addr); }

static void trace_inspect(TRACE trace, VOID *v) {
  /* iterators */
  BBL bbl;
  INS ins;
  ADDRINT addr;

  /* traverse all the BBLs in the trace */
  for (bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
    /* traverse all the instructions in the BBL */
    for (ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)) {
      if (KnobMainOnly.Value() && !INS_InMain(ins)) {
        continue;
      }
      if (KnobRelativeAddress.Value()) {
        addr = INS_RelAddress(ins);
      } else {
        addr = INS_Address(ins);
      }
      inst_map[addr] = INS_Disassemble(ins);
      INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)log_executing_inst,
                               IARG_ADDRINT, addr, IARG_END);
    }
  }
}

VOID Fini(INT32 code, VOID *v) {
  string disasm;
  UINT64 num_executed = 0;

  /* Count up instructions */
  for (ADDRINT addr : inst_vec) {
    num_executed++;
    disasm = inst_map[addr];
    *out << hexstr(addr) << "," << disasm << endl;
  }
}

int main(int argc, char **argv) {
  /* initialize symbol processing */
  PIN_InitSymbols();

  /* initialize PIN */
  if (PIN_Init(argc, argv)) {
    cerr << "Sth error in PIN_Init. Plz use the right command line options."
         << endl;
    return -1;
  }

  /* Output to file if specified */
  if (!KnobOutputName.Value().empty()) {
    out = new ofstream(KnobOutputName.Value().c_str());
  } else {
    *out << "Plz specify output filename with -o option." << endl;
    return -1;
  }

  /* Validate rel_addr option */
  if (KnobRelativeAddress.Value() && !KnobMainOnly.Value()) {
    cerr << "-rel_addr can be enabled only when -main_only is enabled.";
    return -1;
  }

  /* register trace_ins() to be called for every trace */
  TRACE_AddInstrumentFunction(trace_inspect, NULL);

  PIN_AddFiniFunction(Fini, 0);

  PIN_StartProgram();

  return 0;
}
