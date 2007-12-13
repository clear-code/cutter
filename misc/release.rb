#!/usr/bin/env ruby

require 'rubygems'
require 'mechanize'
require 'logger'

require 'net/ftp'

if ARGV.size < 3
  puts "Usage: #{$0} SF_USER_NAME PROJECT_NAME RELEASE_NAME FILE_NAME"
  puts " e.g.: #{$0} ktou cutter 0.3.0 cutter-0.3.0.tar.gz"
  exit(1)
end

sf_user_name, project_name, release_name, file_name, = ARGV

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

  login_form = page.form("login")
  login_form.form_loginname = user_name
  login_form.form_pw = password || yield

  page = agent.submit(login_form, login_form.buttons.first)
  raise "login failed" unless /Personal Page/ =~ page.title
  page
end

def go_project_page(agent, my_page, project_name)
  agent.click(my_page.links.text(/\A#{Regexp.escape(project_name)}\z/i))
end

def upload_file(file)
  Net::FTP.open("upload.sourceforge.net") do |ftp|
    ftp.login
    ftp.chdir("/incoming")
    ftp.putbinaryfile(file)
  end
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
    edit_release_link = WWW::Mechanize::Link.new(edit_release_link,
                                                 file_releases_page.mech,
                                                 file_releases_page)
    agent.click(edit_release_link)
  end
end

def register_file(agent, edit_release_page, file_name)
  add_file_form = page.forms.action(/editreleases/)[1]
  add_file_form["file_list[]"] = file_name

  agent.submit(add_file_form, add_file_form.buttons.first)
end

def select_option(select_field, option_text)
  option = select_field.options.find do |option|
    option.text == "Platform-Independent"
  end
  select_field.value = option.value
end

def set_release_property(agent, edit_release_page)
  edit_file_form = edit_release_page.forms.action(/editreleases/)[2]
  select_option(edit_file_form.field("processor_id"), "Platform-Independent")
  select_option(edit_file_form.field("type_id"), "Source .gz")
  agent.submit(edit_file_form, edit_file_form.buttons.first)
end

def main(sf_user_name, project_name, release_name, file_name)
  agent = WWW::Mechanize.new
  my_page = login(agent, sf_user_name) do
    read_password("SF.net password for [#{sf_user_name}]: ")
  end

  project_page = go_project_page(agent, my_page, project_name)
  file_releases_page = go_file_releases_page(agent, project_page)
  upload_file(file_name)
  edit_release_page = go_edit_release_page(agent, file_releases_page,
                                           release_name)
return
  edit_release_page = register_file(agent, edit_release_page, file_name)
  set_release_property(agent, edit_release_page)
end

main(sf_user_name, project_name, release_name, file_name)
