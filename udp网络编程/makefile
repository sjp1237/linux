.PHONY:all
all:server client
server:server.cc
	g++ $^ -o $@

client:client.cc
	g++ $^ -o $@

clean:
	rm -f client server
