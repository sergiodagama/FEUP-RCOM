all: transmitter receiver

transmitter: app/transmitter/transmitter.c api/api.c
		gcc -Wall app/transmitter/transmitter.c -o t 
# api/api.c api/api_transmitter/api_transmitter.c alarm/alarme.c -o t

receiver: app/receiver/receiver.c api/api.h api/api_receiver/api_receiver.h
		gcc -Wall app/receiver/receiver.c -o e
# api/api.c api/api_receiver/api_receiver.c alarm/alarme.c -o e

clean:
		rm -f t

# app/transmitter/transmitter.h api/api.h api/api_transmitter/api_transmitter.h alarm/alarme.h

# app/receiver/receiver.h api/api.h api/api_receiver/api_receiver.h alarm/alarme.h