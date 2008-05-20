#!/usr/bin/env ruby

require 'rubygems'
require 'mechanize'
require 'logger'

if ARGV.size < 5
  puts "Usage: #{$0} " +
         "SF_USER_NAME PROJECT_NAME PACKAGE_NAME RELEASE_NAME FILE_NAME README NEWS"
  puts " e.g.: #{$0} ktou cutter Cutter 0.3.0 cutter-0.3.0.tar.gz README NEWS"
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

  page = agent.submit(login_form, login_form.buttons.first)
  raise "login failed" unless /Personal Page/ =~ page.title
  page
end

def go_project_page(agent, my_page, project_name)
  agent.click(my_page.links.text(/\A#{Regexp.escape(project_name)}\z/i))
end

def upload_file(agent, file, sf_user_name, password)
  agent.user = sf_user_name
  agent.password = password

  page = agent.get("https://frs.sourceforge.net/")
  upload_form = page.forms[0]
  upload_form.upload = file

  agent.submit(upload_form, upload_form.buttons.first)
end

def go_file_releases_page(agent, project_page)
  agent.click(project_page.links.text(/\AFile Releases\z/))
end

def add_release(agent, file_releases_page, release_name)
  add_release_link = file_releases_page.links.text(/\[Add Release\]/)
  add_release_page = agent.click(add_release_link)

  create_file_release_form = add_release_page.forms.last
  create_file_release_form.release_name = release_name

  agent.submit(create_file_release_form,
               create_file_release_form.buttons.first)
end

def go_edit_release_page(agent, file_releases_page, release_name)
  release_list_link = file_releases_page.links.text(/\[Edit Releases\]/)
  release_list_page = agent.click(release_list_link)
  edit_current_release_row = release_list_page / "td[text() ~= #{release_name}]"
  if edit_current_release_row.empty?
    add_release(agent, file_releases_page, release_name)
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

def project_summary(readme)
  extract_sections(readme)[4].split(/\n\n/)[0].chomp
end

def latest_release_changes(news)
  extract_sections(news)[1].chomp
end

def update_release_info(agent, edit_release_page, news)
  edit_release_info_form = edit_release_page.forms.action(/editreleases/)[0]
  edit_release_info_form.release_changes = latest_release_changes(news)

  agent.submit(edit_release_info_form, edit_release_info_form.buttons.first)
end

def register_file(agent, edit_release_page, file_name)
  add_file_form = edit_release_page.forms.action(/editreleases/)[1]
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
  edit_file_form = edit_release_page.forms.action(/editreleases/)[2]
  puts edit_release_page if edit_file_form.nil?
  select_option(edit_file_form.field("processor_id"), "Platform-Independent")
  select_option(edit_file_form.field("type_id"), "Source .gz")
  agent.submit(edit_file_form, edit_file_form.buttons.first)
end

def go_news_page(agent, project_page)
  agent.click(project_page.links.text(/\ANews\z/))
end

def go_submit_news_page(agent, news_page)
  agent.click(news_page.links.text(/\ASubmit\z/))
end

def submit_news(agent, submit_news_page, project_name, package_name,
                release_name, readme, news)
  submit_news_form = submit_news_page.forms.action(/\bnews\b/)[0]
  summary = "#{project_name}: #{package_name} #{release_name} Released"
  submit_news_form.summary = summary
  details = [project_summary(readme),
             latest_release_changes(news)].join("\n\n")
  submit_news_form.details = details.gsub(/\n/, "\r\n")
  agent.submit(submit_news_form, submit_news_form.buttons.first)
end

def main(sf_user_name, project_name, package_name, release_name, file_name,
         readme, news)
  agent = WWW::Mechanize.new
  password = read_password("SF.net password for [#{sf_user_name}]: ")
  my_page = login(agent, sf_user_name, password)

  project_page = go_project_page(agent, my_page, project_name)
  file_releases_page = go_file_releases_page(agent, project_page)
  upload_file(agent, file_name, sf_user_name, password)
  edit_release_page = go_edit_release_page(agent, file_releases_page,
                                           release_name)
  edit_release_page = update_release_info(agent, edit_release_page, news)
  edit_release_page = register_file(agent, edit_release_page,
                                    File.basename(file_name))
  set_release_property(agent, edit_release_page)

  news_page = go_news_page(agent, project_page)
  submit_news_page = go_submit_news_page(agent, news_page)
  submit_news(agent, submit_news_page, project_name, package_name,
              release_name, readme, news)
end

main(sf_user_name, project_name, package_name, release_name, file_name,
     readme, news)
