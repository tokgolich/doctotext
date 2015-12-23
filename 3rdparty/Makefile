LIBICONV_VERSION=1.11
ZLIB_VERSION=1.2.7
LIBXML2_VERSION=2.7.7
WV2_VERSION=0.2.3_patched_4
HTMLCXX_VERSION=0.85
PTHREADS_VERSION=2.9.1
KOREAN_CMAP_VERSION=1-2
JAPAN_CMAP_VERSION=1-6
GB_CMAP_VERSION=1-5
CNS_CMAP_VERSION=1-6
TO_UNICODE_CMAP_VERSION=20091116
LIBCHARSETDETECT_VERSION=1.0
MIMETIC_VERSION=0.9.7

ifneq (,$(filter $(ARCH),win32 win64))
	ifeq ($(ARCH),win32)
		LIBDIR = lib
	else
		LIBDIR = lib64
	endif
	CONFIG_ARGS =
	ARCH_FLAGS =
	WGET = wget
	WIN = 1
else
ifeq ($(ARCH),x86-linux)
	ARCH_FLAGS = -m32
	WGET = wget
	WIN=0
else
	UNAME = $(shell uname)
	ifeq ($(UNAME),Darwin)
		ARCH = i386-x86_64-macosx
		ifdef MACOSX_ARCH
			ARCH_FLAGS = -arch $(MACOSX_ARCH)
		else
			ARCH_FLAGS = -arch i386 -arch x86_64
		endif
		CONFIG_ARGS = --disable-dependency-tracking
		WGET = curl -O -L
	else
		UNAME_M = $(shell uname -m)
		ifeq ($(UNAME_M),x86_64)
			ARCH = x86_64-linux
			ARCH_FLAGS = -fPIC
		else
			ARCH = x86-linux
			ARCH_FLAGS =
		endif
		WGET = wget
	endif
	WIN = 0
endif
endif

ifeq ($(DEBUG), 1)
	ARCH_D = $(ARCH)-debug
else
	ARCH_D = $(ARCH)
endif

pwd = $(shell pwd)

ifeq ($(WIN),1)

ifeq ($(ARCH),win32)
	CC = i686-w64-mingw32-gcc
else
	CC = x86_64-w64-mingw32-gcc
endif

pthreads: pthreads-$(PTHREADS_VERSION)-$(ARCH).tar.bz2
	#sha1sum -c pthreads-$(PTHREADS_VERSION)-$(ARCH).tar.bz2.sha1
	tar -xjvf pthreads-$(PTHREADS_VERSION)-$(ARCH).tar.bz2
	touch pthreads
	
pthreads-$(PTHREADS_VERSION)-$(ARCH).tar.bz2:
	wget http://silvercoders.com/download/3rdparty/pthreads-$(PTHREADS_VERSION)-$(ARCH).tar.bz2

libiconv: libiconv-$(LIBICONV_VERSION)-$(ARCH).tar.bz2
	#sha1sum -c libiconv-$(LIBICONV_VERSION)-$(ARCH).tar.bz2.sha1
	tar -xjvf libiconv-$(LIBICONV_VERSION)-$(ARCH).tar.bz2
	touch libiconv

libiconv-$(LIBICONV_VERSION)-$(ARCH).tar.bz2:
	wget http://silvercoders.com/download/3rdparty/libiconv-$(LIBICONV_VERSION)-$(ARCH).tar.bz2

zlib: zlib-$(ZLIB_VERSION)-$(ARCH).tar.bz2
	#sha1sum -c zlib-$(ZLIB_VERSION)-$(ARCH).tar.bz2.sha1
	tar -xjvf zlib-$(ZLIB_VERSION)-$(ARCH).tar.bz2
	touch zlib

zlib-$(ZLIB_VERSION)-$(ARCH).tar.bz2:
	wget http://silvercoders.com/download/3rdparty/zlib-$(ZLIB_VERSION)-$(ARCH).tar.bz2

libxml2: libxml2-$(LIBXML2_VERSION)-$(ARCH).tar.bz2
	#sha1sum -c libxml2-$(LIBXML2_VERSION)-$(ARCH).tar.bz2.sha1
	tar -xjvf libxml2-$(LIBXML2_VERSION)-$(ARCH).tar.bz2
	touch libxml2

libxml2-$(LIBXML2_VERSION)-$(ARCH).tar.bz2:
	wget http://silvercoders.com/download/3rdparty/libxml2-$(LIBXML2_VERSION)-$(ARCH).tar.bz2

endif

ifeq ($(WIN),1)
wv2_deps = libiconv
else
wv2_deps =
endif

wv2: $(wv2_deps) wv2-$(WV2_VERSION)-$(ARCH_D).tar.bz2 wv2-$(WV2_VERSION)-private_headers.tar.bz2
	#sha1sum -c wv2-$(WV2_VERSION)-$(ARCH_D).tar.bz2.sha1
	tar -xjvf wv2-$(WV2_VERSION)-$(ARCH_D).tar.bz2
	#sha1sum -c wv2-$(WV2_VERSION)-private_headers.tar.bz2.sha1
	tar -xjvf wv2-$(WV2_VERSION)-private_headers.tar.bz2
	mv wv2-$(WV2_VERSION)-private_headers/*.h wv2/include/wv2/
	rm -rf wv2-$(WV2_VERSION)-private_headers
	touch wv2

wv2-$(WV2_VERSION)-$(ARCH_D).tar.bz2:
	$(WGET) http://silvercoders.com/download/3rdparty/wv2-$(WV2_VERSION)-$(ARCH_D).tar.bz2

wv2-$(WV2_VERSION)-private_headers.tar.bz2:
	$(WGET) http://silvercoders.com/download/3rdparty/wv2-$(WV2_VERSION)-private_headers.tar.bz2

htmlcxx: htmlcxx-$(HTMLCXX_VERSION)-$(ARCH).tar.bz2
	#sha1sum -c htmlcxx-$(HTMLCXX_VERSION)-$(ARCH).tar.bz2.sha1
	tar -xjvf htmlcxx-$(HTMLCXX_VERSION)-$(ARCH).tar.bz2
	touch htmlcxx

htmlcxx-$(HTMLCXX_VERSION)-$(ARCH).tar.bz2:
	$(WGET) http://silvercoders.com/download/3rdparty/htmlcxx-$(HTMLCXX_VERSION)-$(ARCH).tar.bz2
	
libcharsetdetect: libcharsetdetect-$(LIBCHARSETDETECT_VERSION)-$(ARCH).tar.bz2
	#sha1sum -c libcharsetdetect-$(LIBCHARSETDETECT_VERSION)-$(ARCH).tar.bz2.sha1
	tar -xjvf libcharsetdetect-$(LIBCHARSETDETECT_VERSION)-$(ARCH).tar.bz2
	touch libcharsetdetect
	
libcharsetdetect-$(LIBCHARSETDETECT_VERSION)-$(ARCH).tar.bz2:
	$(WGET) http://silvercoders.com/download/3rdparty/libcharsetdetect-$(LIBCHARSETDETECT_VERSION)-$(ARCH).tar.bz2

ifeq ($(ARCH),win32)
mimetic: mimetic-$(MIMETIC_VERSION)-$(ARCH)-2.tar.bz2
	#sha1sum -c mimetic-$(MIMETIC_VERSION)-$(ARCH)-2.tar.bz2.sha1
	tar -xjvf mimetic-$(MIMETIC_VERSION)-$(ARCH)-2.tar.bz2
	touch mimetic

mimetic-$(MIMETIC_VERSION)-$(ARCH)-2.tar.bz2:
	$(WGET) http://silvercoders.com/download/3rdparty/mimetic-$(MIMETIC_VERSION)-$(ARCH)-2.tar.bz2
else
mimetic: mimetic-$(MIMETIC_VERSION)-$(ARCH).tar.bz2
	#sha1sum -c mimetic-$(MIMETIC_VERSION)-$(ARCH).tar.bz2.sha1
	tar -xjvf mimetic-$(MIMETIC_VERSION)-$(ARCH).tar.bz2
	touch mimetic

mimetic-$(MIMETIC_VERSION)-$(ARCH).tar.bz2:
	$(WGET) http://silvercoders.com/download/3rdparty/mimetic-$(MIMETIC_VERSION)-$(ARCH).tar.bz2
endif

unzip: unzip101e/unzip.o

ifeq ($(WIN),1)
unzip101e/unzip.o: zlib unzip101e/.unpacked
	cd unzip101e && $(MAKE) CC=$(CC) CFLAGS=-I../zlib/include unzip.o ioapi.o iowin32.o
else
unzip101e/unzip.o: unzip101e/.unpacked
	cd unzip101e && $(MAKE) CC=$(CC) CFLAGS="$(ARCH_FLAGS)" unzip.o ioapi.o
endif

unzip101e/.unpacked: unzip101e.zip
	unzip -d unzip101e unzip101e.zip
	touch unzip101e/.unpacked

unzip101e.zip:
	wget http://www.winimage.com/zLibDll/unzip101e.zip
	
cmapresources_korean$(KOREAN_CMAP_VERSION).tar.z:
	$(WGET) http://downloads.sourceforge.net/project/cmap.adobe/cmapresources_korean$(KOREAN_CMAP_VERSION).tar.z
	
cmapresources_japan$(JAPAN_CMAP_VERSION).tar.z:
	$(WGET) http://downloads.sourceforge.net/project/cmap.adobe/cmapresources_japan$(JAPAN_CMAP_VERSION).tar.z
	
cmapresources_gb$(GB_CMAP_VERSION).tar.z:
	$(WGET) http://downloads.sourceforge.net/project/cmap.adobe/cmapresources_gb$(GB_CMAP_VERSION).tar.z
	
cmapresources_cns$(CNS_CMAP_VERSION).tar.z:
	$(WGET) http://downloads.sourceforge.net/project/cmap.adobe/cmapresources_cns$(CNS_CMAP_VERSION).tar.z
	
mappingresources4pdf_2unicode_$(TO_UNICODE_CMAP_VERSION).tar.Z:
	$(WGET) http://downloads.sourceforge.net/project/mapping-for-pdf.adobe/mappingresources4pdf_2unicode_$(TO_UNICODE_CMAP_VERSION).tar.Z
	
aj16: cmapresources_japan$(JAPAN_CMAP_VERSION).tar.z
	echo "9467d7ed73c16856d2a49b5897fc5ea477f3a111  cmapresources_japan$(JAPAN_CMAP_VERSION).tar.z" | sha1sum -c -
	tar -xvf cmapresources_japan$(JAPAN_CMAP_VERSION).tar.z
	touch aj16

ak12: cmapresources_korean$(KOREAN_CMAP_VERSION).tar.z
	echo "e4e36995cff0331d8bd5ad00c1c1453c24ab4c07  cmapresources_korean$(KOREAN_CMAP_VERSION).tar.z" | sha1sum -c -
	tar -xvf cmapresources_korean1-2.tar.z
	touch ak12
	
ag15: cmapresources_gb$(GB_CMAP_VERSION).tar.z
	echo "56e6cbd9e053185f9e00118e54fd5159ca118b39  cmapresources_gb$(GB_CMAP_VERSION).tar.z" | sha1sum -c -
	tar -xvf cmapresources_gb$(GB_CMAP_VERSION).tar.z
	touch ag15
	
ac16: cmapresources_cns$(CNS_CMAP_VERSION).tar.z
	echo "80c92cc904c9189cb9611741b913ffd22bcd4036  cmapresources_cns$(CNS_CMAP_VERSION).tar.z" | sha1sum -c -
	tar -xvf cmapresources_cns$(CNS_CMAP_VERSION).tar.z
	touch ac16
	
ToUnicode: mappingresources4pdf_2unicode_$(TO_UNICODE_CMAP_VERSION).tar.Z
	echo "aaf44cb1e5dd2043c932e641b0e41432aee2ca0d  mappingresources4pdf_2unicode_$(TO_UNICODE_CMAP_VERSION).tar.Z" | sha1sum -c -
	tar -xvf mappingresources4pdf_2unicode_$(TO_UNICODE_CMAP_VERSION).tar.Z
	touch ToUnicode
	

clean:
	rm -rf ToUnicode
	rm -rf ac16
	rm -rf ag15
	rm -rf ak12
	rm -rf aj16
	rm -rf libiconv
	rm -rf pthreads
	rm -rf zlib
	rm -rf libxml2
	rm -rf htmlcxx
	rm -rf unzip101e
	rm -rf wv2
	rm -rf libcharsetdetect
	rm -rf mimetic
