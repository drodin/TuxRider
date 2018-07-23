LOCAL_PATH := $(call my-dir)

GENERIC_DIR := generic
TOMMATH_DIR := libtommath
UNIX_DIR := unix

PKG_DIR := data

include $(CLEAR_VARS)

LOCAL_MODULE := tcl

AC_FLAGS := -DPACKAGE_NAME=\"tcl\" -DPACKAGE_TARNAME=\"tcl\" -DPACKAGE_VERSION=\"8.6\" -DPACKAGE_STRING=\"tcl\ 8.6\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DNO_VALUES_H=1 -DHAVE_LIMITS_H=1 -DHAVE_SYS_PARAM_H=1 -DTCL_CFGVAL_ENCODING=\"iso8859-1\" -DMODULE_SCOPE=extern\ __attribute__\(\(__visibility__\(\"hidden\"\)\)\) -DTCL_SHLIB_EXT=\".so\" -DTCL_CFG_OPTIMIZED=1 -DTCL_CFG_DEBUG=1 -DTCL_TOMMATH=1 -DMP_PREC=4 -DTCL_WIDE_INT_TYPE=long\ long -DHAVE_GETCWD=1 -DHAVE_OPENDIR=1 -DHAVE_STRTOL=1 -DHAVE_WAITPID=1 -DNO_GETWD=1 -DHAVE_GETADDRINFO=1 -DHAVE_SYS_TIME_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_STRUCT_TM_TM_ZONE=1 -DHAVE_TM_ZONE=1 -DHAVE_GMTIME_R=1 -DHAVE_LOCALTIME_R=1 -DHAVE_MKTIME=1 -DHAVE_TM_GMTOFF=1 -DHAVE_TIMEZONE_VAR=1 -DHAVE_STRUCT_STAT_ST_BLKSIZE=1 -DHAVE_ST_BLKSIZE=1 -Dstrtod=fixstrtod -DHAVE_INTPTR_T=1 -DHAVE_UINTPTR_T=1 -DNO_UNION_WAIT=1 -DHAVE_SIGNED_CHAR=1 -DHAVE_SYS_IOCTL_H=1 -DTCL_UNLOAD_DLLS=1

GENERIC_SRCS := \
	$(GENERIC_DIR)/regcomp.c \
	$(GENERIC_DIR)/regexec.c \
	$(GENERIC_DIR)/regfree.c \
	$(GENERIC_DIR)/regerror.c \
	$(GENERIC_DIR)/tclAlloc.c \
	$(GENERIC_DIR)/tclAsync.c \
	$(GENERIC_DIR)/tclBasic.c \
	$(GENERIC_DIR)/tclBinary.c \
	$(GENERIC_DIR)/tclCkalloc.c \
	$(GENERIC_DIR)/tclClock.c \
	$(GENERIC_DIR)/tclCmdAH.c \
	$(GENERIC_DIR)/tclCmdIL.c \
	$(GENERIC_DIR)/tclCmdMZ.c \
	$(GENERIC_DIR)/tclCompCmds.c \
	$(GENERIC_DIR)/tclCompExpr.c \
	$(GENERIC_DIR)/tclCompile.c \
	$(GENERIC_DIR)/tclConfig.c \
	$(GENERIC_DIR)/tclDate.c \
	$(GENERIC_DIR)/tclDictObj.c \
	$(GENERIC_DIR)/tclEncoding.c \
	$(GENERIC_DIR)/tclEnv.c \
	$(GENERIC_DIR)/tclEvent.c \
	$(GENERIC_DIR)/tclExecute.c \
	$(GENERIC_DIR)/tclFCmd.c \
	$(GENERIC_DIR)/tclFileName.c \
	$(GENERIC_DIR)/tclGet.c \
	$(GENERIC_DIR)/tclHash.c \
	$(GENERIC_DIR)/tclHistory.c \
	$(GENERIC_DIR)/tclIndexObj.c \
	$(GENERIC_DIR)/tclInterp.c \
	$(GENERIC_DIR)/tclIO.c \
	$(GENERIC_DIR)/tclIOCmd.c \
	$(GENERIC_DIR)/tclIOGT.c \
	$(GENERIC_DIR)/tclIOSock.c \
	$(GENERIC_DIR)/tclIOUtil.c \
	$(GENERIC_DIR)/tclIORChan.c \
	$(GENERIC_DIR)/tclIORTrans.c \
	$(GENERIC_DIR)/tclLink.c \
	$(GENERIC_DIR)/tclListObj.c \
	$(GENERIC_DIR)/tclLiteral.c \
	$(GENERIC_DIR)/tclLoad.c \
	$(GENERIC_DIR)/tclMain.c \
	$(GENERIC_DIR)/tclNamesp.c \
	$(GENERIC_DIR)/tclNotify.c \
	$(GENERIC_DIR)/tclObj.c \
    $(GENERIC_DIR)/tclPkg.c \
    $(GENERIC_DIR)/tclPkgConfig.c \
    $(GENERIC_DIR)/tclPanic.c \
    $(GENERIC_DIR)/tclParse.c \
	$(GENERIC_DIR)/tclPathObj.c \
	$(GENERIC_DIR)/tclPipe.c \
	$(GENERIC_DIR)/tclPosixStr.c \
	$(GENERIC_DIR)/tclPreserve.c \
	$(GENERIC_DIR)/tclProc.c \
	$(GENERIC_DIR)/tclRegexp.c \
	$(GENERIC_DIR)/tclResolve.c \
	$(GENERIC_DIR)/tclResult.c \
	$(GENERIC_DIR)/tclScan.c \
	$(GENERIC_DIR)/tclStubInit.c \
	$(GENERIC_DIR)/tclStringObj.c \
	$(GENERIC_DIR)/tclStrToD.c \
	$(GENERIC_DIR)/tclThread.c \
	$(GENERIC_DIR)/tclThreadAlloc.c \
	$(GENERIC_DIR)/tclThreadJoin.c \
	$(GENERIC_DIR)/tclThreadStorage.c \
	$(GENERIC_DIR)/tclTimer.c \
	$(GENERIC_DIR)/tclTomMathInterface.c \
	$(GENERIC_DIR)/tclTrace.c \
	$(GENERIC_DIR)/tclUtf.c \
	$(GENERIC_DIR)/tclUtil.c \
	$(GENERIC_DIR)/tclVar.c

OO_SRCS := \
	$(GENERIC_DIR)/tclOO.c \
	$(GENERIC_DIR)/tclOOBasic.c \
	$(GENERIC_DIR)/tclOOCall.c \
	$(GENERIC_DIR)/tclOODefineCmds.c \
	$(GENERIC_DIR)/tclOOInfo.c \
	$(GENERIC_DIR)/tclOOMethod.c \
	$(GENERIC_DIR)/tclOOStubInit.c

STUB_SRCS := \
	$(GENERIC_DIR)/tclStubLib.c

TOMMATH_SRCS := \
	$(TOMMATH_DIR)/bncore.c \
	$(TOMMATH_DIR)/bn_reverse.c \
	$(TOMMATH_DIR)/bn_fast_s_mp_mul_digs.c \
	$(TOMMATH_DIR)/bn_fast_s_mp_sqr.c \
	$(TOMMATH_DIR)/bn_mp_add.c \
	$(TOMMATH_DIR)/bn_mp_add_d.c \
	$(TOMMATH_DIR)/bn_mp_and.c \
	$(TOMMATH_DIR)/bn_mp_clamp.c \
	$(TOMMATH_DIR)/bn_mp_clear.c \
	$(TOMMATH_DIR)/bn_mp_clear_multi.c \
	$(TOMMATH_DIR)/bn_mp_cmp.c \
	$(TOMMATH_DIR)/bn_mp_cmp_d.c \
	$(TOMMATH_DIR)/bn_mp_cmp_mag.c \
	$(TOMMATH_DIR)/bn_mp_copy.c \
	$(TOMMATH_DIR)/bn_mp_count_bits.c \
	$(TOMMATH_DIR)/bn_mp_div.c \
	$(TOMMATH_DIR)/bn_mp_div_d.c \
	$(TOMMATH_DIR)/bn_mp_div_2.c \
	$(TOMMATH_DIR)/bn_mp_div_2d.c \
	$(TOMMATH_DIR)/bn_mp_div_3.c \
	$(TOMMATH_DIR)/bn_mp_exch.c \
	$(TOMMATH_DIR)/bn_mp_expt_d.c \
	$(TOMMATH_DIR)/bn_mp_grow.c \
	$(TOMMATH_DIR)/bn_mp_init.c \
	$(TOMMATH_DIR)/bn_mp_init_copy.c \
	$(TOMMATH_DIR)/bn_mp_init_multi.c \
	$(TOMMATH_DIR)/bn_mp_init_set.c \
	$(TOMMATH_DIR)/bn_mp_init_size.c \
	$(TOMMATH_DIR)/bn_mp_karatsuba_mul.c \
	$(TOMMATH_DIR)/bn_mp_karatsuba_sqr.c \
	$(TOMMATH_DIR)/bn_mp_lshd.c \
	$(TOMMATH_DIR)/bn_mp_mod.c \
	$(TOMMATH_DIR)/bn_mp_mod_2d.c \
	$(TOMMATH_DIR)/bn_mp_mul.c \
	$(TOMMATH_DIR)/bn_mp_mul_2.c \
	$(TOMMATH_DIR)/bn_mp_mul_2d.c \
	$(TOMMATH_DIR)/bn_mp_mul_d.c \
	$(TOMMATH_DIR)/bn_mp_neg.c \
	$(TOMMATH_DIR)/bn_mp_or.c \
	$(TOMMATH_DIR)/bn_mp_radix_size.c \
	$(TOMMATH_DIR)/bn_mp_radix_smap.c \
	$(TOMMATH_DIR)/bn_mp_read_radix.c \
	$(TOMMATH_DIR)/bn_mp_rshd.c \
	$(TOMMATH_DIR)/bn_mp_set.c \
	$(TOMMATH_DIR)/bn_mp_shrink.c \
	$(TOMMATH_DIR)/bn_mp_sqr.c \
	$(TOMMATH_DIR)/bn_mp_sqrt.c \
	$(TOMMATH_DIR)/bn_mp_sub.c \
	$(TOMMATH_DIR)/bn_mp_sub_d.c \
	$(TOMMATH_DIR)/bn_mp_to_unsigned_bin.c \
	$(TOMMATH_DIR)/bn_mp_to_unsigned_bin_n.c \
	$(TOMMATH_DIR)/bn_mp_toom_mul.c \
	$(TOMMATH_DIR)/bn_mp_toom_sqr.c \
	$(TOMMATH_DIR)/bn_mp_toradix_n.c \
	$(TOMMATH_DIR)/bn_mp_unsigned_bin_size.c \
	$(TOMMATH_DIR)/bn_mp_xor.c \
	$(TOMMATH_DIR)/bn_mp_zero.c \
	$(TOMMATH_DIR)/bn_s_mp_add.c \
	$(TOMMATH_DIR)/bn_s_mp_mul_digs.c \
	$(TOMMATH_DIR)/bn_s_mp_sqr.c \
	$(TOMMATH_DIR)/bn_s_mp_sub.c

UNIX_SRCS := \
	$(UNIX_DIR)/tclAppInit.c \
    $(UNIX_DIR)/tclLoadDl.c \
	$(UNIX_DIR)/tclUnixChan.c \
	$(UNIX_DIR)/tclUnixEvent.c \
	$(UNIX_DIR)/tclUnixFCmd.c \
	$(UNIX_DIR)/tclUnixFile.c \
	$(UNIX_DIR)/tclUnixInit.c \
	$(UNIX_DIR)/tclUnixPipe.c \
	$(UNIX_DIR)/tclUnixSock.c \
	$(UNIX_DIR)/tclUnixThrd.c \
	$(UNIX_DIR)/tclUnixTime.c \
	$(UNIX_DIR)/tclUnixCompat.c

NOTIFY_SRCS := \
	$(UNIX_DIR)/tclUnixNotfy.c


LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/$(GENERIC_DIR) \
    $(LOCAL_PATH)/$(TOMMATH_DIR) \
    $(LOCAL_PATH)/$(UNIX_DIR) \

LOCAL_CFLAGS := \
    $(AC_FLAGS) \
    -DCFG_RUNTIME_LIBDIR=\"$(PKG_DIR)\" \
    -DCFG_RUNTIME_BINDIR=\"$(PKG_DIR)\" \
    -DCFG_RUNTIME_SCRDIR=\"$(PKG_DIR)\" \
    -DCFG_RUNTIME_INCDIR=\"$(PKG_DIR)\" \
    -DCFG_RUNTIME_DOCDIR=\"$(PKG_DIR)\" \
    -DCFG_INSTALL_LIBDIR=\"$(PKG_DIR)\" \
    -DCFG_INSTALL_BINDIR=\"$(PKG_DIR)\" \
    -DCFG_INSTALL_SCRDIR=\"$(PKG_DIR)\" \
    -DCFG_INSTALL_INCDIR=\"$(PKG_DIR)\" \
    -DCFG_INSTALL_DOCDIR=\"$(PKG_DIR)\" \
    -DTCL_LIBRARY=\"libtcl\" \
    -DTCL_PACKAGE_PATH=\"$(PKG_DIR)\" \

LOCAL_SRC_FILES := $(GENERIC_SRCS) $(TOMMATH_SRCS) $(UNIX_SRCS) $(NOTIFY_SRCS) $(STUB_SRCS) $(OO_SRCS) undefined.c

include $(BUILD_STATIC_LIBRARY)

