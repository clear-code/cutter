#! /usr/bin/env ruby
# See:
#   * https://osdn.net/swagger-ui/#/
#   * http://www.clear-code.com/blog/2016/4/5.html
#
#

begin
  require "bundler/setup"
rescue LoadError
end

require "optparse"
require "yaml"

require "osdn-client"

OSDNClient.configure do |config|
  # For debug
  # config.debugging = true
  # config.logger.level = :debug
end

class OSDNReleaseManager
  def initialize
    @project_name = nil
    @package_name = nil
    @new_version = nil
    @access_token = nil
    @refresh_token = nil
    @release_body = nil
    @asset_files = []

    @parser = OptionParser.new
    setup_option_parser
  end

  def run
    begin
      @parser.parse!
    rescue OptionParser::ParseError => ex
      error(ex.message)
    end

    error(@parser.help) unless @project_name
    error(@parser.help) unless @package_name
    error(@parser.help) unless @new_version
    error(@parser.help) unless @access_token
    error(@parser.help) unless @refresh_token
    error(@parser.help) unless @release_body
    error(@parser.help) if @asset_files.empty?

    OSDNClient.configure do |config|
      config.access_token = @access_token
    end

    project_api = OSDNClient::ProjectApi.new
    # project_api.get_project(@project_name)

    package = project_api.list_packages(@project_name).detect do |_package|
      _package.name == @package_name
    end
    new_release = project_api.create_release(@project_name, package.id, @new_version, release_note: @release_body)

    @asset_files.each do |asset_file|
      File.open(asset_file, "r") do |file|
        project_api.create_release_file(@project_name, package.id, new_release.id, file)
      end
    end

    title = "#{@project_name} #{@new_version} released!"
    # project_api.create_news does not work.
    project_api.create_news_0(@project_name, title, @release_body)
    puts "Created!"
  end

  private

  def setup_option_parser
    @parser.on("--credential=PATH", "Credential file") do |path|
      credential = YAML.load_file(path)
      @access_token = credential["access_token"]
      @refresh_token = credential["refresh_token"]
    end
    @parser.on("--project=NAME", "Project name") do |name|
      @project_name = name
    end
    @parser.on("--package=NAME", "Package name") do |name|
      @package_name = name
    end
    @parser.on("--new-version=VERSION", "New version") do |v|
      @new_version = v
    end
    @parser.on("--news-file=FILE", "NEWS file") do |file|
      latest_release_changes = extract_sections(file)[1].chomp
      @release_body = latest_release_changes
    end
    @parser.on("--asset-file=FILE", "Asset file") do |file|
      @asset_files << file
    end
  end

  def extract_sections(file)
    normalized_text = File.read(file).gsub(/==+\n.*\n==+\n/, '')
    normalized_text.split(/.*\n^(?:==\s+|=+$).*\n\n\n*/)
  end

  def error(message)
    $stderr.puts message
    exit(false)
  end
end

OSDNReleaseManager.new.run
