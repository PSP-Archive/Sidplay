Name: sidplay-base
Summary: A Commodore 64 music player and SID chip emulator.
Version: 1.0.9
Release: 1
Source: %{name}-%{version}.tgz
Icon: sidplay.xpm
Copyright: GPL
Group: Applications/Multimedia
URL: http://www.geocities.com/SiliconValley/Lakes/5147/
BuildRoot: %{_tmppath}/%{name}-buildroot
BuildRequires: libsidplay-devel
Prefix: %{_prefix}

%description
SIDPLAY is basically a music player. It emulates the Sound Interface
Device (SID) chip and the microprocessor unit of the Commodore 64
computer, so it can load and execute C64 machine code programs which
produce music or sound. Normally these are short pieces of code
pulled out of Commodore games or demonstration programs. Using
SIDPLAY, you can listen to thousands of old and new C64 sound files
by infamous artists such as Hubbard and Paul Norman! In emulation,
their music lives on...

%prep
rm -rf %{buildroot}

%setup -q

%build
CXXFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix}
make

%install
mkdir -p %{buildroot}
make DESTDIR=%{buildroot} install
strip %{buildroot}%{_bindir}/*

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%doc COPYING POINTER ChangeLog
%{_bindir}/*
