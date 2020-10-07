module RepositoryHelper
  private
  def osdn_user
    env_value("OSDN_USER")
  end

  def repository_rsync_base_path
    "#{osdn_user}@storage.osdn.net:/storage/groups/c/cu/cutter"
  end

  def repository_gpg_key_ids
    @repository_gpg_key_ids ||= read_repository_gpg_key_ids
  end

  def read_repository_gpg_key_ids
    top_directory = File.join(__dir__, "..")
    [
      File.read(File.join(top_directory, "gpg_uid_rsa4096")).strip,
      File.read(File.join(top_directory, "gpg_uid_dsa1024")).strip,
    ]
  end

  def repository_version
    "2020.10.7"
  end

  def repository_name
    "cutter"
  end

  def repository_label
    "Cutter"
  end

  def repository_description
    "Cutter related packages"
  end

  def repository_url
    "https://osdn.net/projects/cutter/storage"
  end
end
