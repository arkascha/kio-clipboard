#* This file is part of 'kio-clipboard'
#* Copyright (C) 2011 Christian Reiner <kio-clipboard@christian-reiner.info>
#*
#* $Author$
#* $Revision$
#* $Date$
#*

# norootforbuild

Name:           kio-clipboard
License:        GPLv2+
Group:          Utilities/Desktop
Summary:        KDE IO Slave for file based access of clipboard entries
Version:        0.2.4
Release:        1
Source:         %name-%version.tar.bz2
Url:            http://kde-apps.org/content/show.php/kio-clipboard?content=143976
Requires:       libkde4 >= 4.5
Requires:       libqjson
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  cmake libqt4-devel libqjson-devel
BuildRequires:  libkde4-devel >= 4.5
BuildRequires:  libkdecore4-devel
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
    Christian Reiner <kio-clipboard@christian-reiner.info>


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
* Wed Aug 19 2011 Christian Reiner: version 0.2.4
- a number of internal reorganisations as preparation for version 0.3 integrating further clipboard applciations/services
- removed caching of entry payloads in memory out of scalability interests
- dropped 'delete' action, this did not really make sense for clipboards
- first introduction of a shared memory cache between separate slave processes
- serialization feature for node wrapper objects, required for later persistent storage of clipboard entries between sessions
- convenience improvements by higher abstraction of node lists
* Wed Aug 10 2011 Christian Reiner: version 0.2.3
- enhanced mimetype detection for text based clipboard entries
- detection of a few special types of text entries declared as "code"
- simplyfied setup up of internal DBus clients
- fixed cardinality of numeric index in displayed entry names to honour the clipboards size
* Sun Aug 07 2011 Christian Reiner: version 0.2.2
- new structure for source file collection
- slightly optimized icons by usage of overlays (for kde>=4.5)
- fixed 'delete item' action not to delte the target file points to (!)
- preparations for translation (i18n)
- implementation of the 'copyToFile' feature for protocols
- implementation of the 'copyFromFile' feature for protocols
  + for human readable files the content will copied to the clipboard instead of the url or path
  + for all other files or files that exceed the clipboard entries size limit a linking of the url/path is offered instead
  + the user can chose to accept or cancel
* Sat Jul 30 2011 Christian Reiner: version 0.2.1
- changed technical names of virtual nodes from a numeric index to a check sum
  this fixes addressing the wrong node after changes to the clipboard that were not reflected in an ambedding application
- additional content semantics 'Empty', since such content is not really 'Text'
* Fri Jul 29 2011 Christian Reiner: version 0.2.0
- complete rewrite of the internal storage structure
- fixed display name of clipboard folders not to vanish when being clicked
* Tue Jul 27 2011 Christian Reiner: version 0.1
- initial (and buggy) release
- publication on kde-apps.org