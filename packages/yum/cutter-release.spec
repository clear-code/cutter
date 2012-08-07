Summary: cutter RPM repository configuration
Name: cutter-release
Version: 1.0.0
Release: 1
License: GPLv3+
URL: http://cutter.sourceforge.net/
Source: cutter-repository.tar.gz
Group: System Environment/Base
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)
BuildArchitectures: noarch
Obsoletes: cutter-repository < 1.0.0-1

%description
cutter RPM repository configuration.

%prep
%setup -c

%build

%install
%{__rm} -rf %{buildroot}

%{__install} -Dp -m0644 RPM-GPG-KEY-cutter %{buildroot}%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-cutter

%{__install} -Dp -m0644 cutter.repo %{buildroot}%{_sysconfdir}/yum.repos.d/cutter.repo

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%doc *
%pubkey RPM-GPG-KEY-cutter
%dir %{_sysconfdir}/yum.repos.d/
%config(noreplace) %{_sysconfdir}/yum.repos.d/cutter.repo
%dir %{_sysconfdir}/pki/rpm-gpg/
%{_sysconfdir}/pki/rpm-gpg/RPM-GPG-KEY-cutter

%changelog
* Wed Jan 12 2011 Kouhei Sutou <kou@clear-code.com> - 1.0.0-1
- Do not install GPG key for RPM by hand.

* Sat Feb 06 2010 Kouhei Sutou <kou@clear-code.com>
- (1.0.0-0)
- Initial package.
