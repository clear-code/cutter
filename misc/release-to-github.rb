#! /usr/bin/env ruby

begin
  require "bundler/setup"
rescue LoadError
end

require "octokit"
require "optparse"

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
  parser.on("--body-file=FILE", "Body file") do |file|
    github_release_body = File.read(file)
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

  client = Octokit::Client.new(access_token: github_access_token)

  new_release = client.create_release(github_repository, github_tag, body: github_release_body)
  client.upload_asset(new_release[:url], github_release_asset_file)
end

main
