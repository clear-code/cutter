#!/usr/bin/env ruby

puts "#{ARGV.shift} = \\\n"
prefix = ARGV.shift
files = ARGV.collect do |file|
  _prefix = prefix
  case file
  when /-enum-types/
    _prefix = _prefix.sub(/(\$\(top_builddir\))/, '\1/vcc/source')
  end
  "\t#{_prefix}#{file.sub(/\.c/, '.obj')}"
end.join(" \\\n")
puts files
