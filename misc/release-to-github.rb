#! /usr/bin/env ruby

begin
  require "bundler/setup"
rescue LoadError
end

require "octokit"
require "optparse"

def extract_sections(file)
  normalized_text = File.read(file).gsub(/==+\n.*\n==+\n/, '')
  normalized_text.split(/.*\n^(?:==\s+|=+$).*\n\n\n*/)
end

def main
  github_repository = nil
  github_tag = nil
  github_release_body = nil
  github_release_asset_file = nil
  github_access_token = nil

  parser = OptionParser.new
  parser.on("--repository=OWNER/REPO_NAME", "GitHub repository name") do |repo|
    github_repository = repo
  end
  parser.on("--tag=TAG", "Specify tag name") do |tag|
    github_tag = tag
  end
  parser.on("--news-file=FILE", "NEWS file") do |file|
    latest_release_changes = extract_sections(file)[1].chomp
    github_release_body = latest_release_changes.gsub(/^(=+)/){ $1.tr("=", "#") }
  end
  parser.on("--asset-file=FILE", "Asset file") do |file|
    github_release_asset_file = file
  end
  parser.on("--access-token-file=FILE", "Access token file") do |file|
    github_access_token = File.read(file).chomp
  end

  begin
    parser.parse!
  rescue OptionParser::ParseError => ex
    warn ex.message
  end

  unless github_repository
    warn parser.help
    exit false
  end
  unless github_tag
    warn parser.help
    exit false
  end
  unless github_release_body
    warn parser.help
    exit false
  end
  unless github_release_asset_file
    warn parser.help
    exit false
  end
  unless github_access_token
    warn parser.help
    exit false
  end

  client = Octokit::Client.new(access_token: github_access_token)

  new_release = client.create_release(github_repository, github_tag, body: github_release_body)
  client.upload_asset(new_release[:url], github_release_asset_file)
end

main
