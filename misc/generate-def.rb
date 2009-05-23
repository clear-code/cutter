#!/usr/bin/env ruby

puts "EXPORTS"
ARGF.each_line do |line|
  case line
  when /^\w.+\s+\*?((?:|g|gdk)cut_[a-z_]+)\s*\(/
    puts "\t#{$1}"
  end
end
