#!/usr/bin/env ruby

puts "EXPORTS"
ARGV.each do |file|
  content = File.read(file)
  content.scan(/^\w(?-m:.+)\s+\*?((?:|g|gdk)cut_[a-z_]+)\s*(?:\(|$)/m) do
    function = $1
    case function
    when /u?int(max|ptr)/
    else
      puts "\t#{function}"
    end
  end
end
