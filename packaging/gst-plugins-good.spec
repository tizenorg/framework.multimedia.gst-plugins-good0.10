Name:       gst-plugins-good
Summary:    GStreamer plugins from the "good" set
Version:    0.10.29
Release:    1
Group:      TO_BE/FILLED_IN
License:    LGPLv2+
Source0:    %{name}-%{version}.tar.gz
Source1001: packaging/gst-plugins-good.manifest 
Patch0 :    gst-plugins-good-disable-gtk-doc.patch
BuildRequires:  gettext-tools
BuildRequires:  which
BuildRequires:  gst-plugins-base-devel  
BuildRequires:  libjpeg-devel
BuildRequires:  pkgconfig(gstreamer-0.10) 
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(liboil-0.3)
BuildRequires:  pkgconfig(libpng)
BuildRequires:  pkgconfig(libsoup-2.4)
BuildRequires:  pkgconfig(libpulse)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xfixes)
BuildRequires:  pkgconfig(xdamage)
BuildRequires:  pkgconfig(xext)

%description
GStreamer is a streaming media framework, based on graphs of filters
which operate on media data.  Applications using this library can do
anything from real-time sound processing to playing videos, and just
about anything else media-related.  Its plugin-based architecture means
that new data types or processing capabilities can be added simply by
installing new plug-ins.
.
This package contains the GStreamer plugins from the "good" set, a set
of good-quality plug-ins under the LGPL license.


%prep
%setup -q 
%patch0 -p1

%build
cp %{SOURCE1001} .
./autogen.sh 
%configure  --disable-static \
	--prefix=%{_prefix} \
%ifarch %{arm}
	--enable-divx-drm \
%endif
	--disable-nls \
	--with-html-dir=/tmp/dump \
	--disable-examples \
	--disable-gconftool    \
	--disable-alpha    \
	--disable-apetag   \
	--disable-audiofx  \
	--disable-auparse  \
	--disable-cutter   \
	--disable-debugutils    \
	--disable-deinterlace  \
	--disable-effectv  \
	--disable-equalizer    \
	--disable-icydemux \
	--disable-interleave   \
	--disable-flx  \
	--disable-goom \
	--disable-goom2k1  \
	--disable-level    \
	--disable-monoscope    \
	--disable-multipart    \
	--disable-replaygain   \
	--disable-smpte    \
	--disable-spectrum \
	--disable-videobox \
	--disable-videomixer   \
	--disable-y4m  \
	--disable-directsound  \
	--disable-oss  \
	--disable-sunaudio \
	--disable-osx_aidio    \
	--disable-osx_video    \
	--disable-aalib    \
	--disable-aalibtest    \
	--disable-annodex  \
	--disable-cairo    \
	--disable-esd  \
	--disable-esdtest  \
	--disable-flac \
	--disable-gconf    \
	--disable-hal  \
	--disable-libcaca  \
	--disable-libdv    \
	--disable-dv1394   \
	--disable-shout2   \
	--disable-shout2test   \
	--disable-speex \
	--disable-taglib


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install




%files
%manifest gst-plugins-good.manifest
%defattr(-,root,root,-)
%{_libdir}/gstreamer-0.10
%{_libdir}/gstreamer-0.10/libgstavi.so
%{_libdir}/gstreamer-0.10/libgstrtsp.so
%{_libdir}/gstreamer-0.10/libgstisomp4.so
%{_libdir}/gstreamer-0.10/libgstvideocrop.so
%{_libdir}/gstreamer-0.10/libgstid3demux.so
%{_libdir}/gstreamer-0.10/libgstpulse.so
%{_libdir}/gstreamer-0.10/libgstmultifile.so
%{_libdir}/gstreamer-0.10/libgstpng.so
%{_libdir}/gstreamer-0.10/libgstflv.so
%{_libdir}/gstreamer-0.10/libgstudp.so
%{_libdir}/gstreamer-0.10/libgstximagesrc.so
%{_libdir}/gstreamer-0.10/libgstalaw.so
%{_libdir}/gstreamer-0.10/libgstrtpmanager.so
%{_libdir}/gstreamer-0.10/libgstaudioparsers.so
%{_libdir}/gstreamer-0.10/libgstimagefreeze.so
%{_libdir}/gstreamer-0.10/libgstjpeg.so
%{_libdir}/gstreamer-0.10/libgstautodetect.so
%{_libdir}/gstreamer-0.10/libgstvideofilter.so
%{_libdir}/gstreamer-0.10/libgstmatroska.so
%{_libdir}/gstreamer-0.10/libgstmulaw.so
%{_libdir}/gstreamer-0.10/libgstrtp.so
%{_libdir}/gstreamer-0.10/libgstwavparse.so
%{_libdir}/gstreamer-0.10/libgstwavenc.so
%{_libdir}/gstreamer-0.10/libgstvideo4linux2.so
%{_libdir}/gstreamer-0.10/libgstshapewipe.so
%{_libdir}/gstreamer-0.10/libgstoss4audio.so
%{_libdir}/gstreamer-0.10/libgstsouphttpsrc.so
