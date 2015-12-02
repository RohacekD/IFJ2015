## -*- Makefile -*-
##
## User: Tom�
## Time: 26.11.2015 23:51:25
## Makefile created by Oracle Solaris Studio.
##
## This file is generated automatically.
##


#### Compiler and tool definitions shared by all build targets #####
CC = gcc
BASICOPTS = -g
CFLAGS = -std=c99 -Wall


# Define the target directories.
TARGETDIR_IFJ2015.exe=GNU-amd64-Windows


all: $(TARGETDIR_IFJ2015.exe)/IFJ2015.exe

## Target: IFJ2015.exe
OBJS_IFJ2015.exe =  \
	$(TARGETDIR_IFJ2015.exe)/error-tests.o \
	$(TARGETDIR_IFJ2015.exe)/error.o \
	$(TARGETDIR_IFJ2015.exe)/frameStack.o \
	$(TARGETDIR_IFJ2015.exe)/ial-tests.o \
	$(TARGETDIR_IFJ2015.exe)/ial.o \
	$(TARGETDIR_IFJ2015.exe)/IFJ2015.o \
	$(TARGETDIR_IFJ2015.exe)/insTape.o \
	$(TARGETDIR_IFJ2015.exe)/interpret.o \
	$(TARGETDIR_IFJ2015.exe)/paramList.o \
	$(TARGETDIR_IFJ2015.exe)/parser.o \
	$(TARGETDIR_IFJ2015.exe)/parserExpr.o \
	$(TARGETDIR_IFJ2015.exe)/precStack-test.o \
	$(TARGETDIR_IFJ2015.exe)/precStack.o \
	$(TARGETDIR_IFJ2015.exe)/precTab.o \
	$(TARGETDIR_IFJ2015.exe)/scanner.o \
	$(TARGETDIR_IFJ2015.exe)/str.o \
	$(TARGETDIR_IFJ2015.exe)/tabSym.o \
	$(TARGETDIR_IFJ2015.exe)/tabSymList.o \
	$(TARGETDIR_IFJ2015.exe)/tabSymTest.o \
	$(TARGETDIR_IFJ2015.exe)/variable.o
USERLIBS_IFJ2015.exe = $(SYSLIBS_IFJ2015.exe) 
DEPLIBS_IFJ2015.exe =  
LDLIBS_IFJ2015.exe = $(USERLIBS_IFJ2015.exe)


# Link or archive
$(TARGETDIR_IFJ2015.exe)/IFJ2015.exe: $(TARGETDIR_IFJ2015.exe) $(OBJS_IFJ2015.exe) $(DEPLIBS_IFJ2015.exe)
	$(LINK.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ $(OBJS_IFJ2015.exe) $(LDLIBS_IFJ2015.exe)


# Compile source files into .o files
$(TARGETDIR_IFJ2015.exe)/error-tests.o: $(TARGETDIR_IFJ2015.exe) error-tests.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ error-tests.c

$(TARGETDIR_IFJ2015.exe)/error.o: $(TARGETDIR_IFJ2015.exe) error.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ error.c

$(TARGETDIR_IFJ2015.exe)/frameStack.o: $(TARGETDIR_IFJ2015.exe) frameStack.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ frameStack.c

$(TARGETDIR_IFJ2015.exe)/ial-tests.o: $(TARGETDIR_IFJ2015.exe) ial-tests.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ ial-tests.c

$(TARGETDIR_IFJ2015.exe)/ial.o: $(TARGETDIR_IFJ2015.exe) ial.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ ial.c

$(TARGETDIR_IFJ2015.exe)/IFJ2015.o: $(TARGETDIR_IFJ2015.exe) IFJ2015.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ IFJ2015.c

$(TARGETDIR_IFJ2015.exe)/insTape.o: $(TARGETDIR_IFJ2015.exe) insTape.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ insTape.c

$(TARGETDIR_IFJ2015.exe)/interpret.o: $(TARGETDIR_IFJ2015.exe) interpret.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ interpret.c

$(TARGETDIR_IFJ2015.exe)/paramList.o: $(TARGETDIR_IFJ2015.exe) paramList.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ paramList.c

$(TARGETDIR_IFJ2015.exe)/parser.o: $(TARGETDIR_IFJ2015.exe) parser.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ parser.c

$(TARGETDIR_IFJ2015.exe)/parserExpr.o: $(TARGETDIR_IFJ2015.exe) parserExpr.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ parserExpr.c

$(TARGETDIR_IFJ2015.exe)/precStack-test.o: $(TARGETDIR_IFJ2015.exe) precStack-test.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ precStack-test.c

$(TARGETDIR_IFJ2015.exe)/precStack.o: $(TARGETDIR_IFJ2015.exe) precStack.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ precStack.c

$(TARGETDIR_IFJ2015.exe)/precTab.o: $(TARGETDIR_IFJ2015.exe) precTab.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ precTab.c

$(TARGETDIR_IFJ2015.exe)/scanner.o: $(TARGETDIR_IFJ2015.exe) scanner.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ scanner.c

$(TARGETDIR_IFJ2015.exe)/str.o: $(TARGETDIR_IFJ2015.exe) str.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ str.c

$(TARGETDIR_IFJ2015.exe)/tabSym.o: $(TARGETDIR_IFJ2015.exe) tabSym.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ tabSym.c

$(TARGETDIR_IFJ2015.exe)/tabSymList.o: $(TARGETDIR_IFJ2015.exe) tabSymList.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ tabSymList.c

$(TARGETDIR_IFJ2015.exe)/tabSymTest.o: $(TARGETDIR_IFJ2015.exe) tabSymTest.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ tabSymTest.c

$(TARGETDIR_IFJ2015.exe)/variable.o: $(TARGETDIR_IFJ2015.exe) variable.c
	$(COMPILE.c) $(CFLAGS_IFJ2015.exe) $(CPPFLAGS_IFJ2015.exe) -o $@ variable.c



#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_IFJ2015.exe)/IFJ2015.exe \
		$(TARGETDIR_IFJ2015.exe)/error-tests.o \
		$(TARGETDIR_IFJ2015.exe)/error.o \
		$(TARGETDIR_IFJ2015.exe)/frameStack.o \
		$(TARGETDIR_IFJ2015.exe)/ial-tests.o \
		$(TARGETDIR_IFJ2015.exe)/ial.o \
		$(TARGETDIR_IFJ2015.exe)/IFJ2015.o \
		$(TARGETDIR_IFJ2015.exe)/insTape.o \
		$(TARGETDIR_IFJ2015.exe)/interpret.o \
		$(TARGETDIR_IFJ2015.exe)/paramList.o \
		$(TARGETDIR_IFJ2015.exe)/parser.o \
		$(TARGETDIR_IFJ2015.exe)/parserExpr.o \
		$(TARGETDIR_IFJ2015.exe)/precStack-test.o \
		$(TARGETDIR_IFJ2015.exe)/precStack.o \
		$(TARGETDIR_IFJ2015.exe)/precTab.o \
		$(TARGETDIR_IFJ2015.exe)/scanner.o \
		$(TARGETDIR_IFJ2015.exe)/str.o \
		$(TARGETDIR_IFJ2015.exe)/tabSym.o \
		$(TARGETDIR_IFJ2015.exe)/tabSymList.o \
		$(TARGETDIR_IFJ2015.exe)/tabSymTest.o \
		$(TARGETDIR_IFJ2015.exe)/variable.o
	rm -f -r $(TARGETDIR_IFJ2015.exe)


# Create the target directory (if needed)
$(TARGETDIR_IFJ2015.exe):
	mkdir -p $(TARGETDIR_IFJ2015.exe)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Windows

