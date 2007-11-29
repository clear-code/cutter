#!/usr/bin/env ruby

require 'net/ftp'

Net::FTP.open("upload.sourceforge.net") do |ftp|
  ftp.login
  ftp.chdir("/incoming")
  ARGV.each do |file|
    ftp.putbinaryfile(file)
  end
end
