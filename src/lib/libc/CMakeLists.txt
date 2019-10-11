cmake_minimum_required(VERSION 3.5)
project(libc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-everything --target=wasm32 -nostdlib -fno-builtin -ffreestanding -nostdinc  -nostdlibinc -fno-threadsafe-statics  -O3")

SET(INC_SRC_FILENAMES alloca.h assert.h byteswap.h crypt.h ctype.h endian.h errno.h features.h float.h inttypes.h iso646.h 
					  langinfo.h libintl.h limits.h locale.h malloc.h math.h memory.h monetary.h nl_types.h search.h stdalign.h 
					  stdarg.h stdbool.h stdc-predef.h stddef.h stdint.h stdio_ext.h stdio.h stdlib.h stdnoreturn.h string.h 
					  strings.h time.h uchar.h unistd.h values.h wchar.h wctype.h)
SET(INC_SRC_FILES "")
FOREACH(FN ${INC_SRC_FILENAMES})
	LIST(APPEND INC_SRC_FILES "musl/include/${FN}")
ENDFOREACH(FN)

SET(INC_INTERNAL_FILENAMES atomic.h floatscan.c floatscan.h intscan.c intscan.h libc.c libc.h libm.h locale_impl.h pthread_impl.h shgetc.c shgetc.h stdio_impl.h vis.h)
SET(INC_INTERNAL_SRC_FILES "")
FOREACH(FN ${INC_INTERNAL_FILENAMES})
	LIST(APPEND INC_INTERNAL_SRC_FILES "musl/src/internal/${FN}")
ENDFOREACH(FN)

file(GLOB CRYPT_SOURCES  "musl/src/crypt/*.c")
file(GLOB CTYPE_SOURCES  "musl/src/ctype/*.c")

SET(ENV_SOURCES_FILENAMES clearenv.c __environ.c getenv.c putenv.c setenv.c unsetenv.c)
SET(ENV_SOURCES "")
FOREACH(FN ${ENV_SOURCES_FILENAMES})
	LIST(APPEND ENV_SOURCES "musl/src/env/${FN}")
ENDFOREACH(FN)

file(GLOB ERRNO_SOURCES  "musl/src/errno/*.c")

SET(EXIT_SOURCES_FILENAMES assert.c atexit.c at_quick_exit.c _Exit.c quick_exit.c)
SET(EXIT_SOURCES "")
FOREACH(FN ${EXIT_SOURCES_FILENAMES})
	LIST(APPEND EXIT_SOURCES "musl/src/exit/${FN}")
ENDFOREACH(FN)

file(GLOB LOCALE_SOURCES "musl/src/locale/*.c")
file(GLOB MATH_SOURCES  "musl/src/math/*.c")
file(GLOB MBYTE_SOURCES  "musl/src/multibyte/*.c")
file(GLOB SEARCH_SOURCES "musl/src/search/*.c")

SET(STDIO_SOURCES_FILENAMES asprintf.c clearerr.c dprintf.c ext2.c ext.c fclose.c __fclose_ca.c feof.c ferror.c fflush.c fgetc.c fgetln.c fgetpos.c fgets.c fgetwc.c fgetws.c 
							fileno.c fmemopen.c fopencookie.c fprintf.c fputc.c fputs.c fputwc.c fputws.c fread.c fscanf.c fseek.c fsetpos.c ftell.c fwide.c fwprintf.c 
							fwrite.c fwscanf.c getc.c getchar.c getchar_unlocked.c getc_unlocked.c getdelim.c getline.c gets.c getw.c getwc.c getwchar.c __lockfile.c 
							ofl_add.c ofl.c open_memstream.c open_wmemstream.c __overflow.c perror.c onto_printf.c putc.c putchar.c putchar_unlocked.c putc_unlocked.c puts.c 
							putw.c putwc.c putwchar.c rewind.c scanf.c setbuf.c setbuffer.c setlinebuf.c setvbuf.c sscanf.c stderr.c stdin.c __stdio_close.c __stdio_exit.c 
							__stdio_read.c __stdio_seek.c __stdio_write.c stdout.c __stdout_write.c __string_read.c swprintf.c swscanf.c __toread.c __towrite.c __uflow.c 
							ungetc.c ungetwc.c vasprintf.c vdprintf.c vfprintf.c vfscanf.c vfwprintf.c vfwscanf.c vprintf.c vscanf.c vsscanf.c vswprintf.c vswscanf.c 
							vwprintf.c vwscanf.c wprintf.c wscanf.c)
SET(STDIO_SOURCES "")
FOREACH(FN ${STDIO_SOURCES_FILENAMES})
	LIST(APPEND STDIO_SOURCES "musl/src/stdio/${FN}")
ENDFOREACH(FN)

file(GLOB STDLIB_SOURCES "musl/src/stdlib/*.c")
file(GLOB STRING_SOURCES "musl/src/string/*.c")

SET(TIME_SOURCES_FILENAMES asctime.c asctime_r.c difftime.c gmtime.c gmtime_r.c __month_to_secs.c __secs_to_tm.c strftime.c strptime.c time.c timegm.c time_impl.h __tm_to_secs.c wcsftime.c __year_to_secs.c)
SET(TIME_SOURCES "")
FOREACH(FN ${TIME_SOURCES_FILENAMES})
	LIST(APPEND TIME_SOURCES "musl/src/time/${FN}")
ENDFOREACH(FN)

set(INTERNAL_SOURCES musl/src/internal/floatscan.c musl/src/internal/intscan.c musl/src/internal/shgetc.c musl/src/internal/libc.c)

add_library(c 
            ${CRYPT_SOURCES} 
            ${CTYPE_SOURCES}
            ${ENV_SOURCES}
            ${ERRNO_SOURCES}
            ${EXIT_SOURCES}
            ${INTERNAL_SOURCES}
            ${LOCALE_SOURCES}
            ${MATH_SOURCES}
            ${MBYTE_SOURCES}
            ${SEARCH_SOURCES}
            ${STDIO_SOURCES}
            ${STDLIB_SOURCES}
            ${STRING_SOURCES}
            ${TIME_SOURCES}
            ${THREAD_SOURCES}
			"musl/src/malloc/posix_memalign.c"
			"musl/src/malloc/memalign.c"
            "musl/src/misc/ffs.c"
            "musl/src/misc/ffsl.c"
            "musl/src/misc/ffsll.c"
			"musl/src/thread/__lock.c"
			)

target_include_directories(c 
						   PUBLIC 
						   ${CMAKE_CURRENT_SOURCE_DIR}/musl/src/include
						   ${CMAKE_CURRENT_SOURCE_DIR}/musl/include
						   ${CMAKE_CURRENT_SOURCE_DIR}/musl/src/internal
						   ${CMAKE_CURRENT_SOURCE_DIR}/musl/arch/onto)

INSTALL(FILES ${INC_SRC_FILES}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libc/)
INSTALL(FILES "musl/include/sys/stat.h" "musl/include/sys/types.h"
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libc/sys/)
INSTALL(DIRECTORY "musl/arch/onto/bits" DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libc/)

INSTALL(TARGETS c
		ARCHIVE DESTINATION lib)
