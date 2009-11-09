#!/usr/bin/env ruby

puts "#{ARGV.shift} = \\\n"
prefix = ARGV.shift
files = ARGV.collect do |file|
  "\t#{prefix}#{file.sub(/\.c/, '.obj')}"
end.join(" \\\n")
puts files
