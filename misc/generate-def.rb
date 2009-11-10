#!/usr/bin/env ruby

puts "EXPORTS"
ARGF.each_line do |line|
  case line
  when /^\w.+\s+\*?((?:|g|gdk)cut_[a-z_]+)\s*(?:\(|\z)/
    function = $1
    case function
    when /u?int(max|ptr)/
    else
      puts "\t#{function}"
    end
  end
end
