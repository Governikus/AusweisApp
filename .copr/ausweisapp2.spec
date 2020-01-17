Name:           %{project_name}
Version:        %{our_version}
Release:        %{?our_release}%{!?our_release:1}%{?dist}
Summary:        AusweisApp2
Group:          Applications/Utils
License:        EUPL v1.2
URL:            https://github.com/Governikus/AusweisApp2
Source:         %{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  cmake make gcc-c++ qt5-qtbase-devel qt5-qtsvg-devel qt5-linguist qt5-qtwebsockets-devel qt5-qtdeclarative-devel qt5-qtconnectivity-devel qt5-qtquickcontrols2-devel openssl-devel pcsc-lite-devel
Requires: qt5-qtbase qt5-qtsvg qt5-qtwebsockets qt5-qtdeclarative qt5-qtconnectivity qt5-qtquickcontrols2 openssl pcsc-lite

%description
AusweisApp2 is the client application for German Identity Card (Personalausweis)

%global debug_package %{nil}

%prep
%setup

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=release .. 
make
cd ..

%install
cd build
rm -rf ${RPM_BUILD_ROOT}
make DESTDIR=${RPM_BUILD_ROOT} install
cd ..

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%{_bindir}/AusweisApp2
%{_datarootdir}/applications/*
%{_datarootdir}/metainfo/*
%{_datarootdir}/AusweisApp2_CE/*

%changelog
* Fri Jan 17 2020 Jaromir Talir <jaromir.talir@gmail.com> - 0.1-1
- Initial spec file (tested for EPEL8, F30, F31) via COPR
