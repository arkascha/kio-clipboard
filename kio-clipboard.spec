#* This file is part of 'kio-clipboard'
#* Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
#*
#* $HeadURL$
#* $Author$
#* $Revision$
#* $Date$
#*
#
# spec file for package kio-clipboard
#
# Copyright (c) 2011 Christian Reiner, Hamburg, Germany
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# norootforbuild

Name:           kio-clipboard
BuildRequires:  cmake libqt4-devel
BuildRequires:  libkde4-devel
License:        GPLv2+
Group:          Utilities/Desktop
Summary:        KDE IO Slave for file based access of clipboard entries
Version:        0.1
Release:        1.0
Source:         %name-%version.tar.bz2
Url:            http://kde-apps.org/content/show.php/kio-clipboard?content=143976
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Requires:       kdebase4 libkde4
Provides:       kio-clipboard
%kde4_runtime_requires

%description
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
