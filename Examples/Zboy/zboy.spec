#
# spec file for package zboy
#
# Copyright (c) 2013-2015 Mateusz Viste
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name: zboy
Version: 0.60

Release: 1%{?dist}
Summary: A multiplatform, full-featured GameBoy classic emulator
Url: http://zboy.sourceforge.net
Group: Amusements/Games

License: GPL-3.0+
Source0: zboy-%{version}.tar.gz

BuildRequires: SDL2-devel

%description
zBoy is a multiplatform GameBoy classic emulator that provides a load/save feature, can perform PCX screenshots either manually or automatically (every few seconds) and emulates an internal battery for ROMs that were designed to use one (this allows to use the internal save option provided by such games, remember highest scores, etc).

%prep
%setup

%build
make -f Makefile.linux

%check

%install
install -D zboy %buildroot/%{_bindir}/zboy

%files
%attr(644, root, root) %doc zboy.txt history.txt license.txt todo.txt
%attr(755, root, root) %{_bindir}/zboy

%changelog
