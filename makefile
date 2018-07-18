EXEC = pulse

SRC = $(wildcard *.c)

TMP = $(EXEC).c

OBJ = $(SRC:.c=.o)

CFLAGS =

LDFLAGS = -lpigpio -lrt -lpthread

BKUPNAME = $(LOGPATH)$(shell date +%d%m_%Hh%Mm%Ss_)$(TMP)

define	colorecho
		@tput setaf $1
		@echo $2
		@tput sgr0
endef

all: $(EXEC)

pulse: $(OBJ)
	@gcc -o $@ $^ $(LDFLAGS)
	$(call colorecho, 2, "binary compilation success.")

pulse.c: pulse.h

.o: %.c
	@gcc -o $@ -c $<
	$(call colorecho, 2, "Objects compilation success.")

.PHONY: clean

clean:
	@rm -rf *.o
	$(call colorecho, 4, "Binary deleted.")

fclean: clean
	@rm -rf $(EXEC)
	$(call colorecho, 4, "Executable deleted.")

re: fclean all

test: re
	$(call colorecho, 5, "Creating backup file...")
	@cp $(TMP) garbage/$(BKUPNAME)
	$(call colorecho, 3, "-------------------------")
	$(call colorecho, 3, "Begin test phase.")
	$(call colorecho, 3, "-------------------------")
	@sudo ./$(EXEC)

compare:
	sudo ../../../electro/Code/C_Code/07.1.1_PCF8591/pop

backup:
	cp -rf ../pulse/ ../pulseb$(BKUPNAME)
