#* This file is part of 'kio-clipboard'
#* Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
#*
#* $Author$
#* $Revision$
#* $Date$
#*

# norootforbuild

Name:           kio-clipboard
BuildRequires:  cmake libqt4-devel
BuildRequires:  libkde4-devel
License:        GPLv2+
Group:          Utilities/Desktop
Summary:        KDE IO Slave for file based access of clipboard entries
Version:        0.2.1
Release:        1.0
Source:         %name-%version.tar.bz2
Url:            http://kde-apps.org/content/show.php/kio-clipboard?content=143976
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
%kde4_runtime_requires

%description
Overview:
kio_clipboard implements kio-slaves, plugins for KDEs io system. It makes the
content of clipboards available inside file management. That means you can
access the clipboards entries using standard file dialogs like 'File open' or
'Save File', you even can use a file browser and access entries. In general
you can use the slaves wherever you can use an url inside KDE to get access to
the clipboards content. There are no temporary files generated.

Author:
--------
    Christian Reiner <kio-clipboard@christian-reiner.de>


%prep
%setup -q

%build
  %cmake_kde4 -d build
  %make_jobs

%install
  cd build
  %makeinstall
  cd ..
  %kde_post_install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS CHANGELOG
%_kde4_modules/kio_clipboard.so
%_kde4_modules/kio_klipper.so
%_kde_share_dir/services/clipboard.protocol
%_kde_share_dir/services/klipper.protocol

%changelog
* Mon Jul 25 2011 kio-clipboard@christian-reiner.info
- initial release of version 0.1
