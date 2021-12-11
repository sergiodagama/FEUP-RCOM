SDIR=./src
IDIR=./includes
ODIR=./out
CC=gcc

IFLAGS    =-I$(IDIR)
CFLAGS=-g

TRANSMITTER_O_FILES=$(ODIR)/transmitter.o
RECEIVER_O_FILES   =$(ODIR)/receiver.o

O_FILES= $(ODIR)/app_receiver.o $(ODIR)/app_transmitter.o $(ODIR)/api_receiver.o $(ODIR)/api_transmitter.o $(ODIR)/api.o $(ODIR)/buffer_utils.o $(ODIR)/byte_stuffing.o $(ODIR)/file.o 

all: transmitter receiver

transmitter: $(TRANSMITTER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

receiver: $(RECEIVER_O_FILES) $(O_FILES)
	$(CC) $^ -o $@

$(ODIR):
	mkdir -p $(ODIR)

$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean: 
	rm -f transmitter receiver
	rm -rf $(ODIR)