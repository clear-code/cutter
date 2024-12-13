groonga_repository = ENV["GROONGA_REPOSITORY"]
if groonga_repository.nil?
  raise "Specify GROONGA_REPOSITORY environment variable"
end
require "#{groonga_repository}/packages/launchpad-helper"

apache_arrow_repository = ENV["APACHE_ARROW_REPOSITORY"]
if apache_arrow_repository.nil?
  raise "Specify APACHE_ARROW_REPOSITORY environment variable"
end
require "#{apache_arrow_repository}/dev/tasks/linux-packages/package-task"

require_relative "repository-helper"

class CutterOSDNJpPackageTask < PackageTask
  include LaunchpadHelper
  include RepositoryHelper

  def define
    super
    define_release_tasks
    define_ubuntu_tasks
  end

  private
  def detect_release_time
    release_time_env = ENV["RELEASE_TIME"]
    if release_time_env
      Time.parse(release_time_env).utc
    else
      Time.now.utc
    end
  end

  def use_built_package?
    false
  end

  def built_package_url(target_namespace, target)
    raise NotImplementedError
  end

  def built_package_n_split_components
    0
  end

  def release(target_namespace)
    base_dir = __send__("#{target_namespace}_dir")
    repositories_dir = "#{base_dir}/repositories"
    sh("rsync",
       "-av",
       "#{repositories_dir}/",
       "#{repository_rsync_base_path}/")
  end

  def define_release_tasks
    [:apt, :yum].each do |target_namespace|
      tasks = []
      namespace target_namespace do
        enabled = __send__("enable_#{target_namespace}?")
        if use_built_package?
          target_task = Rake.application[target_namespace]
          target_task.prerequisites.delete("#{target_namespace}:build")
          desc "Download #{target_namespace} packages"
          task :download do
            download_packages(target_namespace) if enabled
          end
          tasks << "#{target_namespace}:download"
        end

        desc "Release #{target_namespace} packages"
        task :release do
          release(target_namespace) if enabled
        end
        tasks << "#{target_namespace}:release"
      end
      task target_namespace => tasks
    end
  end

  def dput_configuration_name
    ENV["DPUT_CONFIGUARATION_NAME"] || "cutter-ppa"
  end

  def dput_incoming
    ENV["DPUT_INCOMING"] || "~cutter-testing-framework/ppa/ubuntu/"
  end
end
