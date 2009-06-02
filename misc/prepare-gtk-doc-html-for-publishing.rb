#!/usr/bin/env ruby

require 'optparse'

template_directory = File.dirname(__FILE__)
template_suffix = nil
target_language = nil
site_title = nil
opts = OptionParser.new do |opts|
  opts.on("--help", "Show this message") do
    puts opts
    exit 0
  end

  opts.on("--template-directory=DIRECTORY", "Template directory") do |dir|
    template_directory = dir
  end

  opts.on("--suffix=SUFFIX", "Template file suffix") do |suffix|
    template_suffix = suffix
  end

  opts.on("--language=LANGUAGE", "Target language") do |language|
    target_language = language
  end

  opts.on("--title=TITLE", "Site title") do |title|
    site_title = title
  end
end
opts.parse!

def read_template(template_directory, type, suffix, language)
  file = [type, suffix, language].compact.join("-") + ".html"
  File.read(File.join(template_directory, file))
end

head = read_template(template_directory, "head",
                     template_suffix, target_language)
header = read_template(template_directory, "header",
                       template_suffix, target_language)
footer = read_template(template_directory, "footer",
                       template_suffix, target_language)
ARGV.each do |target|
  File.open(target, "r+") do |input|
    content = input.read
    content = content.sub(/(<\/title>)/, " - #{site_title}\\1") if site_title
    content = content.sub(/(<\/head>)/, head + "\n\\1")
    content = content.sub(/(<body\s.+?>)/, "\\1\n" + header)
    content = content.sub(/(<\/body>)/, footer + "\n\\1")
    input.seek(0)
    input.print(content)
  end
end
