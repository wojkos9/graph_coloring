#pragma once
#include "debug_stream.h"

extern DebugStream d_cout;

#define ccout(l) if (d_cout.level >= (l)) cout

#define BEGIN_DEBUG_STR "__begin__\n"
#define END_DEBUG_STR "__end__\n"