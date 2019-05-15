
include $(top_srcdir)/Makefile.inc

bin_PROGRAMS =demoPcmPlayer

demoPcmPlayer_SOURCES =  ./demoPcmPlayer.c ./rawPcmParser.c ./tinasoundcontrol.c

demoPcmPlayer_CFLAGS = $(CFLAGS_CDXG)

LOCAL_INCLUDE = -I$(top_srcdir) \
		-I$(top_srcdir)/libcore/base/include \
		-I$(top_srcdir)/libcore/playback/include \
		-I$(top_srcdir)/libcore/parser/include \
		-I$(top_srcdir)/libcore/stream/include \
		-I$(top_srcdir)/libcore/common/iniparser \
		-I$(top_srcdir)/external/include/adecoder \
		-I$(top_srcdir)/external/include \
		-I$(top_srcdir)/../libcedarc/include

demoPcmPlayer_CFLAGS += $(LOCAL_INCLUDE)

demoPcmPlayer_LDFLAGS = $(LDFLAGS_CDXG)

demoPcmPlayer_LDADD = $(top_srcdir)/libcore/parser/base/libcdx_parser.la \
			$(top_srcdir)/libcore/stream/base/libcdx_stream.la \
			$(top_srcdir)/libcore/base/libcdx_base.la \
			$(top_srcdir)/libcore/base/libcdx_base.la \
			$(top_srcdir)/libcore/common/libcdx_common.la

LOCAL_LIB = -lpthread -lz -lssl -lVE -lcrypto -lMemAdapter -lcdc_base -lasound

demoPcmPlayer_LDFLAGS += $(LOCAL_LIB)
