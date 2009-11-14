#!/usr/bin/env ruby

resource_dir = ARGV.shift

test_bases = ARGV.collect do |file|
  file.sub(/\.la$/, '')
end

test_dlls = test_bases.collect do |base|
  "#{base}.dll"
end
puts (["tests = "] + test_dlls).join(" \\\n\t")
puts

test_bases.each do |base|
  dll = "#{base}.dll"
  obj = "#{base}.obj"
  local_cflags = base.gsub(/-/, "_") + "_CFLAGS"
  local_ldflags = base.gsub(/-/, "_") + "_LDFLAGS"
  puts("#{dll}: #{obj}")
  puts("\t$(CC) $(CFLAGS) $(#{local_cflags}) -LD -Fe$@ $(#{obj}) " +
       "$(LIBS) $(LDFLAGS) $(#{local_ldflags}) " +
       "/def:#{resource_dir}\\#{base}.def")
  puts
end
