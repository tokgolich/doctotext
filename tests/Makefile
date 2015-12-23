ifneq (,$(filter $(ARCH),win32 win64))
	EXEC = wine cmd /c "set PATH=..\\build;%PATH% &&"
	TIME_EXEC = time $(EXEC)
	EXEEXT = .exe
	SOPRE =
	SOEXT = .dll
	ifeq ($(ARCH),win32)
		CC = i686-w64-mingw32-gcc
		CXX = i686-w64-mingw32-g++
		STRIP = i686-w64-mingw32-strip
		LIBDIR = lib
	else
		CC = x86_64-w64-mingw32-gcc
		CXX = x86_64-w64-mingw32-g++
		STRIP = x86_64-w64-mingw32-strip
		LIBDIR = lib64
	endif
	FILE_OWNER = stat -c %U
	WIN = 1
else
ifeq ($(ARCH),x86-linux)
	EXEEXT =
	SOPRE = lib
	LIB_PATH = LD_LIBRARY_PATH=../build
	SOEXT = .so
	FILE_OWNER = stat -c %U
	ARCH_FLAGS = -m32
	EXEC = $(LIB_PATH)
	TIME_EXEC = $(LIB_PATH) time
	CC = gcc
	CXX = g++
	STRIP = strip
	WIN = 0
else
	EXEEXT =
	SOPRE = lib
	UNAME = $(shell uname)
	ifeq ($(UNAME),Darwin)
		LIB_PATH = DYLD_FALLBACK_LIBRARY_PATH=../build
		SOEXT = .dylib
                FILE_OWNER = stat -f %Su
		ifdef MACOSX_ARCH
                        ARCH_FLAGS += -arch $(MACOSX_ARCH)
                else
                        ARCH_FLAGS += -arch i386 -arch x86_64
                endif
	else
		LIB_PATH = LD_LIBRARY_PATH=../build
		SOEXT = .so
		FILE_OWNER = stat -c %U
		ARCH_FLAGS =
	endif
	EXEC = $(LIB_PATH)
	TIME_EXEC = $(LIB_PATH) time
	CC = gcc
	CXX = g++
	STRIP = strip
	WIN = 0
endif
endif

ifeq ($(WIN),1)
CXXFLAGS += -I../3rdparty/pthreads/include
LDFLAGS += -L../3rdparty/pthreads/lib -lpthreadGC2
else
LDFLAGS += -lpthread
endif

SKIP_TESTS = ! $(shell file ../build/doctotext$(EXEEXT) | grep i386 > /dev/null && ! uname -m | grep 64 > /dev/null; echo $$?)

test: c_api_test$(EXEEXT) cpp_api_test$(EXEEXT) multithreading_test$(EXEEXT) read_from_buffer_test$(EXEEXT)
ifeq ($(SKIP_TESTS),0)
	@echo
	@echo WARNING! TESTING 64-BIT BINARIES REQUIRES 64-BIT MACHINE. SKIPPING.
	@echo
else
	@echo
	@echo TESTING PLAIN TEXT EXTRACTOR...
	@echo
	@for f in [1-9]*.odt; do \
		f=`basename $$f .odt`; \
		for e in odt fodt ods fods odp fodp odg fodg rtf doc xls xlsb pdf ppt docx xlsx pptx pages numbers key html; do \
			echo TESTING $$f.$$e; \
			if ! $(EXEC) ../build/doctotext $$f.$$e | tr -d '\r' | diff -u - $$f.$$e.out; then \
				exit 1; \
			fi; \
			echo TESTING READ FROM BUFFER FOR FILE $$f.$$e; \
			if ! $(EXEC) ./read_from_buffer_test $$f.$$e | tr -d '\r' | diff -u - $$f.$$e.out; then \
				exit 1; \
			fi; \
		done; \
	done
	@for f in biff5.xls hebrew_*.rtf japanese.xls embedded_spreadsheet.doc encoding_in_table.doc fields.doc fields.docx crash_test.xls special_para_cmds.rtf \
			header_footer.doc shared_strings.xls shared_strings.xlsb \
			ole.odt ole.fodt nested_tables.html first.eml second.eml third.eml fourth.eml \
			html_with_doc_ext.doc html_with_xls_ext.xls rtf_with_doc_ext.doc \
			$(addprefix comments_libreoffice_3.5.,odt doc docx rtf); do \
		echo TESTING $$f; \
		if ! $(EXEC) ../build/doctotext $$f | tr -d '\r' | diff -u - $$f.out; then \
			exit 1; \
		fi; \
		echo TESTING READ FROM BUFFER FOR FILE $$f; \
		if ! $(EXEC) ./read_from_buffer_test $$f | tr -d '\r' | diff -u - $$f.out; then \
			exit 1; \
		fi; \
	done
	@
	@echo TESTING TXT PARSER...
	@
	@for f in gb18030_without_bom.text utf16_with_bom.text; do \
		echo TESTING $$f; \
		if ! $(EXEC) ../build/doctotext $$f | tr -d '\r' | diff -u - $$f.out; then \
			exit 1; \
		fi; \
	done
	@echo
	@echo TESTING METADATA EXTRACTOR...
	@echo
	@for f in $(addprefix meta_*.,odt ods odp odg rtf doc xls ppt docx xlsx pptx html); do \
		echo TESTING $$f; \
		touch -t 1211041503.20 $$f; \
		if ! $(EXEC) ../build/doctotext --meta $$f | sed s/`$(FILE_OWNER) $$f`/OWNER/g | tr -d '\r' | diff -u - $$f.out; then \
			exit 1; \
		fi; \
	done
	@echo
	@echo TESTING SPEED OF CONVERTER...
	@echo
	@for f in *.gz; do \
		f=`basename $$f .gz`; \
		echo TESTING $$f; \
		gunzip -c $$f.gz > $$f; \
		$(TIME_EXEC) ../build/doctotext $$f > /dev/null; \
		rm $$f; \
	done
	@echo
	@echo TESTING LIBRARY C API...
	@echo
	if ! $(EXEC) ./c_api_test --verbose 1.html | tr -d '\r' | diff -u - c_api_test.out; then \
		exit 1; \
	fi
	if ! cat c_api_test.log.out | tr -d '\r' | diff -u - 1.html.log; then \
			exit 1; \
		fi; \
	rm c_api_test.log.out
	@echo
	@echo TESTING LIBRARY C++ API...
	@echo
	if ! $(EXEC) ./cpp_api_test meta_libreoffice_3.5_modified.odt | tr -d '\r' | diff -u - cpp_api_test.out; then \
		exit 1; \
	fi
	@echo
	@echo TESTING LIBRARY C++ API IN MULTITHREAD ENVIRONMENT...
	@echo
	if ! $(EXEC) ./multithreading_test $(addprefix *.,eml odt fodt ods fods odp fodp odg fodg rtf doc xls xlsb pdf ppt docx xlsx pptx pages numbers key html text) > /dev/null; then \
		exit 1; \
	fi
	@echo
	@echo TESTING LOGS REDIRECTION...
	@echo
	for e in odt fodt ods fods odp fodp odg fodg rtf doc xls xlsb pdf ppt docx xlsx pptx pages numbers key html; do \
		$(EXEC) ../build/doctotext --verbose --log-file=log.txt 1.$$e > /dev/null; \
		if ! cat log.txt | tr -d '\r' | diff -u - 1.$$e.log; then \
			exit 1; \
		fi; \
	done
	rm log.txt
	@echo
	@echo TESTS COMPLETE.
	@echo
endif

update_tests:
	@echo
	@echo UPDATING CONVERTER TESTS...
	@echo
	@for f in [1-9]*.odt; do \
		f=`basename $$f .odt`; \
		for e in odt fodt ods fods odp fodp odg fodg rtf doc xls xlsb pdf ppt docx xlsx pptx pages numbers key html; do \
			echo PROCESSING $$f.$$e; \
			$(EXEC) ../build/doctotext $$f.$$e | tr -d '\r' > $$f.$$e.out; \
		done; \
	done
	$(EXEC) ./c_api_test 1.html | tr -d '\r' > c_api_test.out
	rm c_api_test.log.out
	@for f in biff5.xls hebrew_*.rtf japanese.xls embedded_spreadsheet.doc encoding_in_table.doc fields.doc fields.docx crash_test.xls special_para_cmds.rtf \
			header_footer.doc shared_strings.xls shared_strings.xlsb first.eml second.eml third.eml fourth.eml \
			ole.odt ole.fodt nested_tables.html gb18030_without_bom.txt utf16_with_bom.txt \
			html_with_doc_ext.doc html_with_xls_ext.xls rtf_with_doc_ext.doc \
			$(addprefix comments_libreoffice_3.5.,odt doc docx rtf); do \
		echo PROCESSING $$f; \
		$(EXEC) ../build/doctotext $$f | tr -d '\r' > $$f.out; \
	done
	for f in $(addprefix meta_*.,odt ods odp odg rtf doc xls ppt docx xlsx pptx html); do \
		echo PROCESSING $$f; \
		touch -t 1211041503.20 $$f; \
		$(EXEC) ../build/doctotext --meta $$f | sed s/`stat -c %U $$f`/OWNER/g | tr -d '\r' > $$f.out; \
	done
	for e in odt fodt ods fods odp fodp odg fodg rtf doc xls xlsb pdf ppt docx xlsx pptx pages numbers key html; do \
		echo PROCESSING 1.$$e; \
		$(EXEC) ../build/doctotext --verbose --log-file=1.$$e.log.tmp 1.$$e > /dev/null; \
		cat 1.$$e.log.tmp | tr -d '\r' > 1.$$e.log; \
		rm 1.$$e.log.tmp; \
	done
	@echo
	@echo TESTS UPDATED.
	@echo

c_api_test$(EXEEXT): c_api_test.c ../build/doctotext_c_api.h ../build/$(SOPRE)doctotext$(SOEXT)
	$(LIB_PATH) $(CC) -o c_api_test$(EXEEXT) c_api_test.c -L../build/ -ldoctotext $(ARCH_FLAGS)
	$(STRIP) c_api_test$(EXEEXT)

cpp_api_test$(EXEEXT): cpp_api_test.cpp ../build/plain_text_extractor.h ../build/formatting_style.h ../build/$(SOPRE)doctotext$(SOEXT)
	$(LIB_PATH) $(CXX) -o cpp_api_test$(EXEEXT) cpp_api_test.cpp -L../build/ -ldoctotext $(LDFLAGS) $(ARCH_FLAGS)
	$(STRIP) cpp_api_test$(EXEEXT)

multithreading_test$(EXEEXT): multithreading_test.cpp ../build/plain_text_extractor.h ../build/formatting_style.h ../build/$(SOPRE)doctotext$(SOEXT)
	$(LIB_PATH) $(CXX) -o multithreading_test$(EXEEXT) multithreading_test.cpp -L../build/ -ldoctotext $(LDFLAGS) $(CXXFLAGS) $(ARCH_FLAGS)
	$(STRIP) multithreading_test$(EXEEXT)

read_from_buffer_test$(EXEEXT): read_from_buffer_test.cpp ../build/plain_text_extractor.h ../build/metadata.h ../build/$(SOPRE)doctotext$(SOEXT)
	$(LIB_PATH) $(CXX) -o read_from_buffer_test$(EXEEXT) read_from_buffer_test.cpp -L../build/ -ldoctotext $(LDFAGS) $(CXXFLAGS) $(ARCH_FLAGS)
	$(STRIP) read_from_buffer_test$(EXEEXT)

clean:
	rm -f c_api_test c_api_test.exe cpp_api_test cpp_api_test.exe multithreading_test multithreading_test.exe read_from_buffer_test read_from_buffer_test.exe
