NAME = new.parser
LLVM_CONFIG = llvm-config

NO_WARNING =  -Wno-return-type \
	-Wno-c++11-compat-deprecated-writable-strings \
	-Wno-deprecated-register \
	-Wno-switch \
	-Wno-inconsistent-missing-override \

CXXFLAGS = `$(LLVM_CONFIG) --cppflags` -std=c++11 $(NO_WARNING)
LDFLAGS = `$(LLVM_CONFIG) --ldflags`
LIBS = `$(LLVM_CONFIG) --libs --system-libs`

all:
	flex -o tokenizer.cpp ${NAME}.l
	bison -d -o parser.cpp ${NAME}.y
	clang++ -g ${CXXFLAGS} *.cpp $(LIBS) $(LDFLAG) -ll -o Simple-C
clean:
	@rm -f parser.output *.o parser.cpp parser.hpp tokenizer.cpp Tiny-Pascal LLVM_LR
