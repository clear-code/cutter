#!/usr/bin/env ruby

require 'English'
require 'rubygems'
require 'mechanize'
require 'logger'

if ARGV.size < 5
  puts "Usage: #{$0} " +
         "SF_USER_NAME PROJECT_NAME PACKAGE_NAME RELEASE_NAME FILE_NAME README NEWS"
  puts " e.g.: #{$0} ktou Cutter cutter 0.3.0 cutter-0.3.0.tar.gz README NEWS"
  exit(1)
end

sf_user_name, project_name, package_name, release_name, file_name, \
  readme, news, = ARGV

def read_password(prompt, input=$stdin, output=$stdout)
  output.print(prompt)
  system("/bin/stty -echo") if input.tty?
  input.gets.chomp
ensure
  system("/bin/stty echo") if input.tty?
  output.puts
end

def login(agent, user_name, password=nil)
  page = agent.get("https://sourceforge.net/account/login.php")

  login_form = page.form("login_userpw")
  login_form.form_loginname = user_name
  login_form.form_pw = password || yield

  begin
    page = agent.submit(login_form, login_form.buttons.first)
  rescue WWW::Mechanize::ResponseCodeError
    page = agent.get("https://sourceforge.net/my/")
  end
  raise "login failed" unless /Personal Page/ =~ page.title
  page
end

def go_project_page(agent, my_page, project_name)
  project_page_link = my_page.links.find do |link|
    /\A#{Regexp.escape(project_name)}\z/i =~ link.text
  end
  agent.click(project_page_link)
end

def upload_file(agent, file, sf_user_name, password)
  agent.basic_auth(sf_user_name, password)

  page = agent.get("https://frs.sourceforge.net/webupload")
  upload_form = page.forms[0]
  upload_form.file_uploads[0].file_name = file

  agent.submit(upload_form, upload_form.buttons.first)
end

def go_file_releases_page(agent, project_page)
  download_page_link = project_page.links.find do |link|
    /\ADownload\z/ =~ link.text
  end
  download_page = agent.click(download_page_link)

  nbsp = "\302\240"
  file_releases_page_link = download_page.links.find do |link|
    "Manage#{nbsp}Packages#{nbsp}/#{nbsp}Releases" == link.text
  end
  agent.click(file_releases_page_link)
end

def find_target_release_link(file_releases_page, package_name, label)
  target_release_row = (file_releases_page / "tr").find do |row|
    (row / "input").find {|input| input["value"] == package_name}
  end
  raise "can't find package ID" if /package_id=(\d+)/ !~ target_release_row.to_s
  package_id = $1
  file_releases_page.links.find do |link|
    label =~ link.text and /package_id=#{package_id}/ =~ link.href
  end
end

def add_release(agent, file_releases_page, package_name, release_name)
  add_release_link = find_target_release_link(file_releases_page,
                                              package_name,
                                              /\[Add Release\]/)
  add_release_page = agent.click(add_release_link)

  create_file_release_form = add_release_page.forms.last
  create_file_release_form.release_name = release_name

  agent.submit(create_file_release_form,
               create_file_release_form.buttons.first)
end

def go_edit_release_page(agent, file_releases_page, package_name, release_name)
  edit_release_link = find_target_release_link(file_releases_page,
                                               package_name,
                                               /\[Edit Releases\]/)
  release_list_page = agent.click(edit_release_link)
  edit_current_release_row = (release_list_page / "td").find do |row|
    /#{Regexp.escape(release_name)}/ =~ row.text
  end
  if edit_current_release_row.nil?
    add_release(agent, file_releases_page, package_name, release_name)
  else
    edit_release_link = (edit_current_release_row / "a")[0]
    edit_release_link = WWW::Mechanize::Page::Link.new(edit_release_link,
                                                       file_releases_page.mech,
                                                       file_releases_page)
    agent.click(edit_release_link)
  end
end

def extract_sections(file)
  File.read(file).gsub(/==+\n.*\n==+\n/, '').split(/.*\n^==.*\n\n\n*/)
end

def guess_target_index(file, default_index)
  index = default_index
  if /:(\d+)\Z/ =~ file
    file = $PREMATCH
    index = $1.to_i
  end
  [file, index]
end

def project_summary(readme)
  readme, index = guess_target_index(readme, 4)
  extract_sections(readme)[index].split(/\n\n/)[0].chomp
end

def latest_release_changes(news)
  news, index = guess_target_index(news, 1)
  extract_sections(news)[index].chomp
end

def remove_rd_link_markup(text)
  text.gsub(/\(\(<(.*?)(?:\|.*?)?>\)\)/m, '\1')
end

def update_release_info(agent, edit_release_page, news)
  edit_release_info_form = edit_release_page.forms.find do |form|
    /editreleases/ =~ form.action
  end
  edit_release_info_form.release_changes = latest_release_changes(news)

  agent.submit(edit_release_info_form, edit_release_info_form.buttons.first)
end

def register_file(agent, edit_release_page, file_name)
  add_file_form = edit_release_page.forms.find_all do |form|
    /editreleases/ =~ form.action
  end[1]
  add_file_form["file_list[]"] = file_name

  agent.submit(add_file_form, add_file_form.buttons.first)
end

def select_option(select_field, option_text)
  option = select_field.options.find do |option|
    option.text == option_text
  end
  select_field.value = option.value
end

def set_release_property(agent, edit_release_page)
  edit_file_form = edit_release_page.forms.find_all do |form|
    /editreleases/ =~ form.action
  end[2]
  puts edit_release_page if edit_file_form.nil?
  select_option(edit_file_form.field("processor_id"), "Platform-Independent")
  select_option(edit_file_form.field("type_id"), "Source .gz")
  agent.submit(edit_file_form, edit_file_form.buttons.first)
end

def notify_release(agent, edit_release_page)
  edit_file_form = edit_release_page.forms.find_all do |form|
    /editreleases/ =~ form.action
  end[3]
  return if edit_file_form.nil?
  sure_check_bock = edit_file_form.checkbox("sure")
  return if sure_check_bock.nil?
  sure_check_bock.check
  agent.submit(edit_file_form, edit_file_form.buttons.first)
end

def go_news_page(agent, project_page)
  agent.click(project_page.links.find {|link| /\ANews\z/ =~ link.text})
end

def go_submit_news_page(agent, news_page)
  submit_news_page_link = news_page.links.find do |link|
    /\bnews\b/ =~ link.href and /\ASubmit\z/ =~ link.text
  end
  agent.click(submit_news_page_link)
end

def submit_news(agent, submit_news_page, project_name, package_name,
                release_name, readme, news)
  submit_news_form = submit_news_page.forms.find do |form|
    /\bnews\b/ =~ form.action
  end
  summary = "#{project_name}: #{package_name} #{release_name} Released"
  submit_news_form.summary = summary
  details = [project_summary(readme),
             latest_release_changes(news)].join("\n\n")
  submit_news_form.details = remove_rd_link_markup(details).gsub(/\n/, "\r\n")
  agent.submit(submit_news_form, submit_news_form.buttons.first)
end

def main(sf_user_name, project_name, package_name, release_name, file_name,
         readme, news)
  agent = WWW::Mechanize.new do |_agent|
    # _agent.log = Logger.new(STDOUT)
  end
  password = read_password("SF.net password for [#{sf_user_name}]: ")
  my_page = login(agent, sf_user_name, password)

  project_page = go_project_page(agent, my_page, project_name)
  file_releases_page = go_file_releases_page(agent, project_page)
  upload_file(agent, file_name, sf_user_name, password)
  edit_release_page = go_edit_release_page(agent, file_releases_page,
                                           package_name, release_name)
  edit_release_page = update_release_info(agent, edit_release_page, news)
  edit_release_page = register_file(agent, edit_release_page,
                                    File.basename(file_name))
  edit_release_page = set_release_property(agent, edit_release_page)
  edit_release_page = notify_release(agent, edit_release_page)

  news_page = go_news_page(agent, project_page)
  submit_news_page = go_submit_news_page(agent, news_page)
  submit_news(agent, submit_news_page, project_name, package_name,
              release_name, readme, news)
end

main(sf_user_name, project_name, package_name, release_name, file_name,
     readme, news)
