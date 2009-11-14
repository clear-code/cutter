#!/usr/bin/env ruby

puts "#{ARGV.shift} = \\\n"
prefix = ARGV.shift
files = ARGV.collect do |file|
  _prefix = prefix
  case file
  when /(-enum-types|-marshalers)/
    _prefix = _prefix.sub(/(\$\(top_builddir\))/, '\1/vcc/source')
  end
  "\t#{_prefix}#{file.sub(/\.c(?:pp)?$/, '.obj')}".gsub(/\//, "\\")
end.join(" \\\n")
puts files
