# CC = g++
# 
# INTER = Interface.cpp
# PLAYER = Player.cpp
# BOARD = Board.cpp
# CREATE = CreateBoard.cpp
# OBJ = Interface.o Player.o Board.o CreateBoard.o
# 
# # サフィックスルール適用対象の拡張子の定義
# .SUFFIXES: .cpp .o
# 
# # プライマリターゲット
# game: $(OBJ)
# 	$(CC) -o game.exe $^ -lws2_32
# 
# # サフィックスルール
# .cpp.o:
# 	$(CC) -c $< -lws2_32
# 
# .PHONY: clean
# clean:
# 	del *.o





# test用
CC = g++

INTER = Interface.cpp
PLAYER = Player.cpp
BOARD = Board.cpp
CREATE = CreateBoard.cpp
READ = ReadJson.cpp
JSON = JsonManagement.cpp
HTTP = HttpRequest.cpp
OBJ = Interface.o Board.o Player.o CreateBoard.o ReadJson.o JsonManagement.o HttpRequest.cpp
	
# サフィックスルール適用対象の拡張子の定義
.SUFFIXES: .cpp .o

# プライマリターゲット
game: $(OBJ)
	$(CC) -o a.exe $^ -lws2_32

# サフィックスルール
.cpp.o:
	$(CC) -c $< -lws2_32
		
.PHONY: clean
clean:
	del *.o
