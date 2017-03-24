#!/usr/bin/env ruby

require 'time'

if ARGV.size < 4
  puts "Usage: #{$0} " +
         "PACKAGE " +
         "OLD_VERSION OLD_RELEASE_DATE " +
         "NEW_VERSION NEW_RELEASE_DATE " +
         "FILES..."
  puts " e.g.: #{$0} cutter 1.0.9 2009-10-20 1.1.0 2009-11-29 " +
         " html/index.html html/index.html.ja rpm/fedora/cutter.spec.in"
  exit(1)
end

package, old_version, old_release_date, new_version, new_release_date, *files =
  ARGV

def git_user_name
  `git config --get user.name`.chomp
end

def git_user_email
  `git config --get user.email`.chomp
end

name = ENV['DEBFULLNAME'] || ENV['NAME'] || git_user_name
email = ENV['DEBEMAIL'] || ENV['EMAIL'] || git_user_email

files.each do |file|
  content = replaced_content = File.read(file)
  case file
  when /\.spec(?:\.in)?\z/
    date = Time.parse(new_release_date).strftime("%a %b %d %Y")
    if content !~ /#{Regexp.escape(new_version)}/
      replaced_content = content.sub(/^(%changelog\n)/, <<-EOC)
%changelog
* #{date} #{name} <#{email}> - #{new_version}-1
- new upstream release.

      EOC
    end
    replaced_content = replaced_content.sub(/^(Release:\s+)\d+/,
                                            "\\11")
  when /(debian|jessie)[^\/]*\/changelog\z/
    date = Time.parse(new_release_date).rfc2822
    if content !~ /#{Regexp.escape(new_version)}/
      replaced_content = content.sub(/\A/, <<-EOC)
#{package} (#{new_version}-1) unstable; urgency=low

  * New upstream release.

 -- #{name} <#{email}>  #{date}

      EOC
    end
  else
    [[old_version, new_version],
     [old_release_date, new_release_date]].each do |old, new|
      replaced_content = replaced_content.gsub(/#{Regexp.escape(old)}/, new)
      if /\./ =~ old
        old_underscore = old.gsub(/\./, '-')
        new_underscore = new.gsub(/\./, '-')
        replaced_content =
          replaced_content.gsub(/#{Regexp.escape(old_underscore)}/,
                                new_underscore)
      end
    end
  end

  next if replaced_content == content
  File.open(file, "w") do |output|
    output.print(replaced_content)
  end
end
