
/* jtime.c: access to Jack transport in Lua (AG, 2017-11-05)

   Copyright (c) 2017 by Albert Graef <aggraef@gmail.com>

   Copying and distribution of this file, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  This file is offered as-is,
   without any warranty. */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <jack/jack.h>
#include <jack/transport.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* This just creates a dummy Jack client used to access the current Jack
   transport information with the jtime() Lua function. Most of the basic code
   comes from the showtime.c example included in the Jack sources. */

jack_client_t *client;

static void jtime_shutdown(void *arg);

static int jtime_init(void)
{
  if (client) return 0; // client already running
  if ((client = jack_client_open("jtime", JackNullOption, NULL)) == 0) {
    fprintf(stderr, "jack server not running?\n");
    return -1;
  } else {
    jack_on_shutdown(client, jtime_shutdown, 0);
    return 0;
  }
}

static void jtime_fini(void)
{
  jack_client_close(client);
  client = NULL;
}

static void jtime_shutdown(void *arg)
{
  fprintf(stderr, "jack shutting down\n");
  jtime_fini();
}

static int l_jtime(lua_State *L)
{
  jack_position_t current;
  jack_transport_state_t transport_state;
  jack_nframes_t frame_time;
  const char *st;

  // bail out if Jack can't be initialized
  if (jtime_init()) return 0;

  transport_state = jack_transport_query(client, &current);
  frame_time = jack_frame_time(client);

  switch (transport_state) {
  case JackTransportStopped:
    st = "stopped";
    break;
  case JackTransportRolling:
    st = "rolling";
    break;
  case JackTransportStarting:
    st = "starting";
    break;
  default:
    st = "unknown";
  }
	
  // first three return values are the current frame, time and transport status
  lua_pushinteger(L, current.frame);
  lua_pushinteger(L, frame_time);
  lua_pushstring(L, st);

  // The transport information may be available in different formats. Here we
  // only return the BBT, if available, as a table with 7 values (bars, beats
  // and ticks, the meter as a pair of numerator and denominator, number of
  // ticks per beat, and the current tempo as a BPM value), or nil otherwise.
  // NB: We currently do not report the bar_start_tick field available in
  // Jack's BBT struct whose meaning is unclear. Also note that not all Jack
  // transport servers may set all of these values, or any at all. (Ardour
  // does.)
  if (current.valid & JackPositionBBT) {
    int i = 0;
    lua_createtable(L, 7, 0);
    lua_pushinteger(L, ++i);
    lua_pushinteger(L, current.bar);
    lua_settable(L, -3);
    lua_pushinteger(L, ++i);
    lua_pushinteger(L, current.beat);
    lua_settable(L, -3);
    lua_pushinteger(L, ++i);
    lua_pushinteger(L, current.tick);
    lua_settable(L, -3);
    lua_pushinteger(L, ++i);
    lua_pushnumber(L, current.beats_per_bar);
    lua_settable(L, -3);
    lua_pushinteger(L, ++i);
    lua_pushnumber(L, current.beat_type);
    lua_settable(L, -3);
    lua_pushinteger(L, ++i);
    lua_pushnumber(L, current.ticks_per_beat);
    lua_settable(L, -3);
    lua_pushinteger(L, ++i);
    lua_pushnumber(L, current.beats_per_minute);
    lua_settable(L, -3);
  } else {
    lua_pushnil(L);
  }

  return 4;
}

/* transport control: start, stop and seek to a given position */

static int l_jtime_start(lua_State *L)
{
  if (jtime_init()) return 0;
  jack_transport_start(client);
  return 0;
}

static int l_jtime_stop(lua_State *L)
{
  if (jtime_init()) return 0;
  jack_transport_stop(client);
  return 0;
}

static int l_jtime_locate(lua_State *L)
{
  lua_Number pos = luaL_checknumber(L, 1);
  if (jtime_init()) return 0;
  jack_transport_locate(client, pos);
  return 0;
}

static const struct luaL_Reg jtime [] = {
  {"jtime", l_jtime},
  {"jtime_start", l_jtime_start},
  {"jtime_stop", l_jtime_stop},
  {"jtime_locate", l_jtime_locate},
  {NULL, NULL}  /* sentinel */
};

int luaopen_jtime (lua_State *L) {
  luaL_newlib(L, jtime);
  jtime_init();
  return 1;
}
