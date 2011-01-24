= cut-diff / Cutter / Cutter's manual

== NAME

cut-diff - show difference between 2 files with color

== SYNOPSIS

(({cut-diff})) [((*option ...*))] ((*file1*)) ((*file2*))

== DESCRIPTION

cut-diff is a diff command that uses diff feature in Cutter.
It shows difference with color.

It's recommended that you use a normal diff(1) when you want
to use with patch(1) or you don't need color.

== OPTIONS

: --version

   cut-diff shows its own version and exits.

: -c [yes|true|no|false|auto], --color=[yes|true|no|false|auto]

   If 'yes' or 'true' is specified, cut-diff uses colorized
   output by escape sequence. If 'no' or 'false' is
   specified, cut-diff never use colorized output. If 'auto'
   or the option is omitted, cut-diff uses colorized output
   if available.

   The default is auto.

: -u, --unified

   cut-diff uses unified diff format.

: --context-lines=LINES

   Shows diff context around (({LINES})).

   All lines are shown by default. When unified diff format
   is used, 3 lines are shown by default.

: --label=LABEL, -L=LABEL

   Uses (({LABEL})) as a header label. The first
   ((*--label*)) option value is used as ((*file1*))'s label
   and the second ((*--label*)) option value is used as
   ((*file2*))'s label.

   Labels are the same as file names by default.

== EXIT STATUS

The exit status is 0 for success, non-0 otherwise.

TODO: 0 for non-difference, 1 for difference and non-0 for
errors.

== EXAMPLE

In the following example, cut-diff shows difference
between (({file1})) and (({file2})):

  % cut-diff file1 file2

In the following example, cut-diff shows difference
between (({file1})) and (({file2})) with unified diff format:

  % cut-diff -u file1 file2

== SEE ALSO

diff(1)
