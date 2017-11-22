construir: p2-dogClient.c p2-dogServer.c
	gcc p2-dogClient.c -o exeClient
	gcc p2-dogServer.c -o exeServer

ejeC: exeClient
	./exeClient

ejeS: exeServer
	./exeServer


