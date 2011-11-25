# -*- ruby -*-

task :default => :test

task :build do
  sh "./autogen.sh"
  sh "./configure --disable-gtk-doc"
end

task :test => :build do
  sh "test/run-test.sh"
end
