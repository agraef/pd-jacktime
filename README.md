# pd-jacktime

This external provides a basic interface to the [Jack transport client API][1].
The `jacktime` object reads and reports the current frame, status
(stopped/rolling) and, if available, BBT (bars-beats-ticks, meter, ppq, bpm)
information. This information can then be used to synchronize Pd with a Jack
transport master like Ardour.

Please check the included help patch for usage information and the comments in
the jacktime.pd_lua script for technical details.

[1]: http://jackaudio.org/files/docs/html/transport-design.html

## Requirements and Installation

The external is written in Lua (5.3), so pd-lua is required (available
at <https://github.com/agraef/pd-lua>). Run `make` to compile the C part of
the external (Lua module jtime.c) and then `make install` to install the
external under /usr/lib/pd-externals. Finally, put the
/usr/lib/pd-externals/jacktime folder on your Pd library path and you should
be set.
