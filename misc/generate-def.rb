#!/usr/bin/env ruby

puts "EXPORTS"
ARGV.each do |file|
  content = File.read(file)
  content.scan(/^\w(?-m:.+)\s+\*?
                ((?:|g|gdk|soup)cut(?:test)?_[a-z_]+)
                \s*(?:\(|$)/mx) do
    function = $1
    case function
    when /u?int(max|ptr)/
    else
      puts "\t#{function}"
    end
  end
end
