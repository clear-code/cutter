#!/usr/bin/env ruby

require 'fileutils'

source_dir = ARGV.shift
output_dir = ARGV.shift

FileUtils.mkdir_p(output_dir)

ARGV.each do |file|
  base = file.sub(/\.la$/, '')
  File.open(File.join(output_dir, "#{base}.def"), "w") do |def_file|
    c_source = File.join(source_dir, "#{base}.c")
    if File.exist?(c_source)
      content = File.read(c_source)
      def_file.puts("EXPORTS")
      content.scan(/^void\s+([a-z_]+)\s*\(/) do
        function = $1
        case function
        when /u?int(max|ptr)/
        else
          def_file.puts("\t#{function}")
        end
      end
    end
  end
end
