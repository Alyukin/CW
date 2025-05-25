CC = gcc

CFLAGS = -Wall -std=c99

TARGET = cwDone

SRC = cwDone.c

# Библиотека для мат функций
LIBS = -lm

# Компиляция
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
	
# Для пересборки
rebuild: clean $(TARGET)
