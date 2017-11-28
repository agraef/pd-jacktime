# pd-jacktime

This external provides a basic interface to the [Jack transport][1] client
API. The `jacktime` object observes and reports the current frame, status
(stopped/rolling) and, if available, BBT (bars-beats-ticks, as well as meter,
ppq, bpm) data. This information can then be used to synchronize Pd with a
Jack transport master like Ardour.

[1]: http://jackaudio.org/files/docs/html/transport-design.html

## Requirements and Installation

The external is written in Lua, so pd-lua is required. Lua 5.3 has been
tested, earlier Lua versions might require some fiddling with the Lua source
in jacktime.pd_lua and/or the C part of the module in jtime.c. Lua can be
found in many Linux distributions and on MacPorts. A version of pd-lua
compatible with Lua 5.3 is available from <https://github.com/agraef/pd-lua>.

Run `make` to compile the Lua module jtime.c and then `make install` to
install the external under /usr/lib/pd-externals. Finally, put the
/usr/lib/pd-externals/jacktime folder on your Pd library path and you should
be set.

## Usage

Please check the included help patch for usage information and the comments in
the jacktime.pd_lua script for technical details.

Also included is a little GOP abstraction timebase.pd which can either produce
an internal clock pulse using metro, or sync to an external time source using
Jack. In either case it generates on its leftmost outlet a sequence of pulse
numbers counting from 0 to num-1, where `num` is the numerator of the current
meter. The tempo is determined by the `bpm` value which as usual specifies the
number of quarter pulses per minute, and the `unit` (denominator) of the meter
(the latter customarily is a power of 2, but you can specify any positive
integer there).

It is also possible to generate fractional pulses (subdivisions of the base
pulse) by setting the `div` parameter to a value > 1. You can change the meter
and the tempo at any time using the provided controls, or you can feed a
message of the form `num unit` or `num unit div` to the leftmost inlet of the
abstraction, and set a `bpm` value on the second inlet. The defaults are meter
= 4 4 1 (common time without subdivisions) and bpm = 120. You can also switch
between Jack sync and internal time at any time, either with the red and green
toggles of the abstraction or by sending a corresponding message to the third
inlet.

In addition, the third inlet also accepts any other message which is simply
forwarded to the embedded `jacktime` object, like the Jack transport messages
`start`, `stop` and `locate pos`. (The transport messages also work if
transport information reporting is off. Please check the jacktime-help.pd
patch for more information on these.) Please also check the timebase-help.pd
patch for an example which produces metronome clicks for illustration
purposes.

Once pd-jacktime is installed, to give it a go launch Jack and open the
timebase-help.pd patch. (Note that it doesn't matter whether Pd itself is
configured to use Jack, as the jacktime object uses its own internal Jack
transport client.) Click the green toggle in the timebase abstraction and turn
up the volume in the click subpatch to listen to the metronome click. (You can
also click on the toggle in the click subpatch to enable MIDI output, if you
have a GM-compatible synthesizer like fluidsynth hooked up to Pd's MIDI
output.) Try changing the meter by clicking on one of the sample messages at
the top of the patch.

If all is well, you should hear the metronome clicks as determined by the
current meter and bpm settings. Now try engaging Jack sync by clicking on the
red toggle in the abstraction. Then launch your favorite Jack sequencer
application, such as Ardour or Hydrogen. Configure it as a Jack time master
and press Play. Jack transport should be rolling now and the metronome clicks
produced by Pd should be exactly in sync with the Jack application.

Note that once the timebase abstraction has been switched to Jack, meter and
tempo are determined solely by the Jack time master and pulses will be
produced only when the transport is rolling. (However, you can still set any
desired subdivisions using the `div` value, which isn't controlled by Jack.)

If all works well, congrats! You can now start building your own Jack time
clients in Pd, e.g., starting from the the timebase-help.pd patch.

Here are some things you can try if things are not working well. If you're
getting sound from timebase-help.pd, but Jack sync (the red toggle) doesn't
appear to work, double-check to make sure that your Jack sequencer application
is really configured as a Jack time master. If you're not getting any sound
from timebase-help.pd even when using the internal clock (green toggle), but
the pulses seem to be "rolling" on the first outlet of the time abstraction,
please double-check that Pd is actually hooked up to the right sound device
(employing Pd's "Test Audio and Midi" patch) and that the volume in the click
subpatch and your speakers is turned up. (Yeah, I know, you're an audio
expert, it's such a basic thing, who'd make such a silly mistake!
Nevertheless it happens all the time.)

## Reporting Bugs etc.

If you followed all instructions and it still doesn't work, well, you just
might have run into a bug in pd-jacktime. We'd like to hear about those, so
please head over to the bug tracker
at <https://github.com/agraef/pd-jacktime/issues> to report the
problem. Describe the symptoms and provide as much detail as possible, such as
error messages in Pd's main window. If you have any other comments,
suggestions or code contributions, please use the bug tracker, toss me a pull
request, or mail me at <aggraef@gmail.com>.
