#!/usr/bin/env ruby

top_srcdir = ARGV.shift
target_path = ARGV.shift

resource_dir = "#{top_srcdir}\\vcc\\resource\\#{target_path.gsub(/\//, '\\')}"
makefile_path = "Makefile.#{target_path.split(/\//).join('.')}"

test_bases = ARGV.collect do |file|
  file.sub(/\.la$/, '')
end

test_dlls = test_bases.collect do |base|
  "#{base}.dll"
end

File.open(makefile_path, "w") do |makefile|
  makefile.puts((["tests = "] + test_dlls).join(" \\\n\t"))
  makefile.puts

  test_bases.each do |base|
    dll = "#{base}.dll"
    obj = "#{base}.obj"
    local_cflags = base.gsub(/-/, "_") + "_CFLAGS"
    local_ldflags = base.gsub(/-/, "_") + "_LDFLAGS"
    makefile.puts("#{dll}: #{obj}")
    makefile.puts("\t$(CC) $(CFLAGS) $(#{local_cflags}) -LD -Fe$@ #{obj} " +
                  "$(LIBS) $(LDFLAGS) $(#{local_ldflags}) " +
                  "/def:#{resource_dir}\\#{base}.def")
    makefile.puts
  end
end
