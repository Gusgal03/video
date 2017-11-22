#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<ctype.h>
#include<time.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

#define PUERTO 3535
#define clear() printf("\033[H\033[J")

struct dogType{
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo;
};

void imprimirMenu(){
     printf("---------------------VETERINARIA---------------------\n\n");
     printf("\t1. Ingresar registro \n");
     printf("\t2. Ver registros (Historia Clinica)\n");
     printf("\t3. Borrar registro\n");
     printf("\t4. Buscar registro (No. registros existentes)\n");
     printf("\t5. Salir\n");     
     printf("\nQue desea hacer?\n");
}

bool imprimirError(char rangoMenor,char rangoMayor,char opcion){
	/*
		Esta funcion se usa para saber si el usuario ingreso
		un valor(char opcion) entre los rangos [rangoMenor,rangoMayor]
		dados a la funcion. Para ello se revisa primero si lo ingresado
		es un salto de linea(10) o un @(64) que pueden ser enviados
		por el getchar() y se retorna TRUE porque se debe
		imprimir un error, si la opcion esta fuera de los rangos se retorna
		TRUE y ademas se escribe que el usuario ingrese de nuevo un valor.
		En otro caso se retorna FALSE 
	*/
	if((int)opcion==10 || (int)opcion==64){
		return true;
	}else if((int)opcion<(int)rangoMenor || (int)opcion>(int)rangoMayor){
		clear();
		printf("-----------------------------------------------");
		printf("\nError con opcion ingresada. Intentelo de nuevo\n\n");
		return true;
	}else{
		return false;
	}
}

long nRegistrosExistentes(int clienteDesc){
	/*
		Esta funcion sirve para recibir el numero de registros
		existentes del archivo dataDogs.dat por medio del servidor
		Para ello, se espera a que el servidor haga un send con el valor
		del numero de registros existentes en el archivo, el cual se recibe
		con un recv y se almacena en nRegistrosExistentes. Mas tarde, se
		verifica el error y se retorna nRegistrosExistentes.
			-returned es un entero que guarda el valor retornado por las funcion
			 recv 
	*/
		int returned;
		long nRegistrosExistentes=0;
		returned=recv(clienteDesc,&nRegistrosExistentes,sizeof(nRegistrosExistentes),0);
		if(returned==-1){
			perror("Error en nRegistrosExistentes: recv(nRegistrosExistentes)");
			exit(-1);	
		}
		return nRegistrosExistentes;
}

bool noExistenRegistros(int clienteDesc,char buffer[128]){
	/*
		Esta funcion retorna TRUE si no hay un espacio vacio
		en el archivo dataDogs.dat y FALSE si hay por lo menos un
		espacio vacio. Para ello, tiene que esperar a que el servidor
		haga los calculos y recibe un 0 para FALSE y un 1 para TRUE
		que es recibido del servidor por medio de un recv. Usa
		los parametros de clienteDesc para saber el servidor y 
		buffer para guardar el '0' o el '1'
	*/
		int returned;
		returned=recv(clienteDesc,buffer,128,0);
		if(returned==-1){
			perror("Error en noExistenRegistros: recv(buffer)");
			exit(-1);	
		}

		if(buffer[0]=='1'){
			return true;
		}else if(buffer[0]=='0'){
			return false;
		}else{
			printf("\n\nError!!!\n\n");
			return false;
		}
}

bool existenRegistros(int clienteDesc,char buffer[128]){
	/*
		Esta funcion retorna TRUE si hay por lo menos un registro 
		existente en el archivo dataDogs.dat y FALSE si no hay nisiquiera
		un registro existente. Para ello, se tiene que comunicar con el 
		servidor que envia 2(FALSE) si no hay ningun registro y 1(TRUE)
		si hay por lo menos uno. La respuesta se recibe por medio
		de un recv y se verifica el error correspondiente usando 
		la variable entera returned
	*/	
		int returned,recibido=-1;
		returned=recv(clienteDesc,&recibido,sizeof(recibido),0);
		if(returned==-1){
			perror("Error en existenRegistros: recv(buffer)");
			exit(-1);	
		}
		if(recibido==2){
			return false;
		}else if(recibido==1){
			return true;
		}else{
			return true;	
		}

}

bool registroVacio(int clienteDesc,char buffer[128]){
	/*
		Esta funcion retorna TRUE si dado un id
		enviado al servidor antes de esta funcion,
		alli hay un registro vacio(TRUE) o no(FALSE).
		Se espera a que el servidor haga los calculos
		y se recibe una cadena '1' en caso de TRUE
		o '0' en caso de FALSE, esta se guarda en 
		un buffer dado como argumento en la funcion	
	*/
	int returned;
	returned=recv(clienteDesc,buffer,128,0);
	if(returned==-1){
		perror("Error en registroVacio: recv(buffer)");
		exit(-1);	
	}
	if(buffer[0]=='1'){
		return true;
	}else if(buffer[0]=='0'){
		return false;
	}else{
		printf("\n\nError!!!\n\n");
		return false;
	}
	
}

void ingresar(int clienteDesc,char buffer[128]){
	/*
		Esta funcion envia al servidor los datos de una mascota 
		para ingresar y el servidor la agrega al archivo dataDogs.dat.
		En primer lugar, verifica que haya un espacio para agregar
		y para ello usa la funcion noExistenRegistros
	*/
	if(!noExistenRegistros(clienteDesc,buffer)){
		/*
			Se usan las siguientes variables para enviar los datos al
			servidor:
				-returned que es un entero para verificar los errores
				 dadas las funciones send y recv
				-nombreIngresado,razaIngresada,sexoIngresado que son
				 char o arreglos de char para guardar los datos que se 
				 indican en sus nombres
				-edadIngresada,estaturaIngresada,pesoIngresado que son
				 enteros o floats para guardar los datos respectivos
				 a sus nombres
				-id que es un long para guardar el id donde se guardara
				 la mascota a ingresar
		*/
			int returned;
			char nombreIngresado[32];
			char tipoIngresado[32];
			int edadIngresada;
			char razaIngresada[16];
			int estaturaIngresada;
			float pesoIngresado;
			char sexoIngresado;
			long id=0;

		//Se piden los datos
			clear();
			printf("-----------------INGRESAR REGISTRO---------------\n"); 
			printf("\n\tNOMBRE: ");
			scanf("%s",&nombreIngresado[0]);
			printf("\tTIPO: ");
			scanf("%s",&tipoIngresado[0]);
			printf("\tEDAD (anos): ");
			scanf("%i",&edadIngresada);
			printf("\tRAZA: ");
			scanf("%s",&razaIngresada[0]);
			printf("\tESTATURA (cm): ");
			scanf("%i",&estaturaIngresada);
			printf("\tPESO (kg): ");
			scanf("%f",&pesoIngresado);
			printf("\tSEXO(H o M): ");
			scanf("%s",&sexoIngresado);
			printf("\n\n");
		
		//Se pide una confirmacion al usuario
			clear();
			char opcionInsercion='@';
			while(imprimirError('1','2',opcionInsercion)){
				if(opcionInsercion!=(char)10){
					printf("-----------------INGRESAR REGISTRO---------------\n");
					printf("Se guardara el siguiente registro:\n");
					printf("\n\tNOMBRE: %s",nombreIngresado);
					printf("\n\tTIPO: %s",tipoIngresado);
					printf("\n\tEDAD: %i",edadIngresada);
					printf("\n\tRAZA: %s",razaIngresada);
					printf("\n\tESTATURA: %i",estaturaIngresada);
					printf("\n\tPESO: %.2f",pesoIngresado);
					printf("\n\tSEXO: %c",sexoIngresado);
					printf("\n\nConfirmar?");
					printf("\n\t1. Si");
					printf("\n\t2. No\n\n");
				}
				opcionInsercion=getchar();
			}
		//Se envia la confirmacion correspondiente del usuario al servidor, para que este
		//sepa qué hacer dada la confirmacion

			returned=send(clienteDesc,&opcionInsercion,sizeof(opcionInsercion),0);
			if(returned==-1){
				perror("Error en ingresar: send(opcionInsercion)");
				exit(-1);	
			}
			if(opcionInsercion=='1'){
				//En el caso de que se haya confirmado el ingreso, se envian
				//los datos al servidor
				returned=send(clienteDesc,nombreIngresado,sizeof(nombreIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: send(nombreIngresado)");
					exit(-1);	
				}
				returned=send(clienteDesc,tipoIngresado,sizeof(tipoIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: send(tipoIngresado)");
					exit(-1);	
				}
				returned=send(clienteDesc,razaIngresada,sizeof(razaIngresada),0);
				if(returned==-1){
					perror("Error en ingresar: send(razaIngresada)");
					exit(-1);	
				}
				returned=send(clienteDesc,&sexoIngresado,sizeof(sexoIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: send(sexoIngresado)");
					exit(-1);	
				}
				returned=send(clienteDesc,&edadIngresada,sizeof(edadIngresada),0);
				if(returned==-1){
					perror("Error en ingresar: send(edadIngresada)");
					exit(-1);	
				}
				returned=send(clienteDesc,&estaturaIngresada,sizeof(estaturaIngresada),0);
				if(returned==-1){
					perror("Error en ingresar: send(estaturaIngresada)");
					exit(-1);	
				}
				returned=send(clienteDesc,&pesoIngresado,sizeof(pesoIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: send(pesoIngresado)");
					exit(-1);	
				}
				returned=recv(clienteDesc,&id,sizeof(clienteDesc),0);
				if(returned==-1){
					perror("Error en ingresar: recv(id)");
					exit(-1);	
				}
				clear();
				printf("Se inserto a %s con id=%lu\n\n",nombreIngresado,id);
		}else{
			//En el caso de que no se haya confirmado el ingreso, se notifica al usuario
			clear();
			printf("No se ha insertado a %s\n\n",nombreIngresado);
		}
	}else{
		//En el caso de que el archivo no tenga espacio, se notifica al usuario
		clear();
		printf("No hay espacio en la tabla\nYa hay 10.000.000 de registros ocupados\nSi quiere ingresar un registro, borre alguno\n\n");
	}
}

void borrar(int clienteDesc,char buffer[128]){
	/*
		Esta funcion envia un id al servidor para que 
		este borre del archivo dataDogs.dat la correspondiente
		estructura. Primero, debe verificar que existe por lo menos
		un registro para hacer el borrado, esto lo hace con la funcion 
		existenRegistros
	*/
	if(existenRegistros(clienteDesc,buffer)){
		/*
			En el caso de que exista por lo menos un registro para borrar
			se usan las siguientes variables para enviar los datos al servidor:
				-i que es una variable long iteradora para los for.
				-idRegistro que es un long que guarda el id donde se va a hacer
				 el borrado
				-rExistentes que es un long que guarda el numero de registros
				 existentes en el archivo dataDogs.dat
				-correcto y primero que son booleanos para hacer la verificacion
				 de los datos ingresados por el usuario
				-returned que es un entero para manipular los errores
				 de las funciones send y recv 
		*/
		long i=0,idRegistro=0,rExistentes=0;
		rExistentes=nRegistrosExistentes(clienteDesc);
		bool correcto=false,primero=true;
		char confirmacion='@';
		int returned;

		//Se pide el id al usuario
		while(registroVacio(clienteDesc,buffer) || !correcto){
			clear();
			printf("------------BORRAR REGISTRO--------------\n\n");
			printf("Existen %lu registros para borrar:\n\n",rExistentes);
			if(!correcto && !primero){
				printf("\nID digitado incorrecto . . .\n");
				correcto=true;
			}
			correcto=true;
			primero=false;
			printf("\nQue registro desea borrar (ID)?\n\n");
			scanf("%lu",&idRegistro);
			//Se envia el id al servidor
			returned=send(clienteDesc,&idRegistro,sizeof(idRegistro),0);
			if(returned==-1){
				perror("Error en ver: send(idRegistro)");
				exit(-1);	
			}
			if(idRegistro<0){
				idRegistro=0;
				correcto=false;
			}
		}
		clear();

		//Se pide la confirmacion al usuario y se envia al servidor
		while(imprimirError('1','2',confirmacion)){
			if(confirmacion!=(char)10){
				printf("Borrar registro %lu",idRegistro);
				printf("?\n\n\t1. Si");
				printf("\n\t2. No\n\n");
			}
			confirmacion=getchar();
		}
		returned=send(clienteDesc,&confirmacion,sizeof(confirmacion),0);
		if(returned==-1){
				perror("Error en ver: send(confirmacion)");
			exit(-1);	
		}
		if(confirmacion=='1'){
			clear();
			printf("Registro %lu borrado correctamente\n\n",idRegistro);
		}else{
			clear();
			printf("No se borro registro %lu",idRegistro);
			printf("\n\n");
		}
	}else{
		//En el caso de que no hayan registros se notifica al usuario
		printf("\nNo hay registros para borrar en la tabla\n\n");
	}
}

void ver(int clienteDesc,char buffer[128]){
	/*
		Esta funcion envia un id al servidor para que este
		cree una historia clinica o envie una notificacion
		de que la historia clinica ya estaba creada y el cliente
		la abra
	*/
	if(existenRegistros(clienteDesc,buffer)){
		/*
			En el caso de que exista por lo menos un registro para ver
			se usan las siguientes variables para hacer la obtencion del id:
				-i que es una variable long iteradora para los for.
				-idRegistro que es un long que guarda el id donde se va a hacer
				 el borrado
				-rExistentes que es un long que guarda el numero de registros
				 existentes en el archivo dataDogs.dat
				-correcto y primero que son booleanos para hacer la verificacion
				 de los datos ingresados por el usuario
				-returned que es un entero para manipular los errores
				 de las funciones send y recv 
		*/
		int returned;
		long i=0,idRegistro=0,rExistentes=0;
		rExistentes=nRegistrosExistentes(clienteDesc);
		bool correcto=false,primero=true;
		char confirmacion='@';
		while(registroVacio(clienteDesc,buffer) || !correcto){
			clear();
			printf("----------------VER REGISTRO---------------\n\n");
			printf("Existen %lu registros para ver\n\n",rExistentes);
			if(!correcto && !primero){
				printf("\nID digitado incorrecto . . .\n");
				correcto=true;
			}
			correcto=true;
			primero=false;
			printf("\nQue registro desea ver (ID)?\n\n");
			scanf("%lu",&idRegistro);
			//Se envia el id al servidor
			returned=send(clienteDesc,&idRegistro,sizeof(idRegistro),0);
			if(returned==-1){
				perror("Error en ver: send(idRegistro)");
				exit(-1);	
			}
			if(idRegistro<0){
				idRegistro=0;
				correcto=false;
			}
		}
		clear();
		//Se pide la confirmacion
		while(imprimirError('1','2',confirmacion)){
			if(confirmacion!=(char)10){
				printf("Abrir historia clinica de registro %lu",idRegistro);
				printf("?\n\n\t1. Si");
				printf("\n\t2. No\n\n");
			}
			confirmacion=getchar();
		}
		//Se envia la confirmacion al servidor
		returned=send(clienteDesc,&confirmacion,sizeof(confirmacion),0);
		if(returned==-1){
				perror("Error en ver: send(confirmacion)");
			exit(-1);	
		}
		if(confirmacion=='1'){
			//Los archivos con la historia clinica tendran como nombre el id
			//correspondiente en la estructura, estos ids tiene como maximo
			//7 digitos ya que el maximo es 9.999.999, cada digito
			//se guarda en un vector llamado nombreArchivo, que se llena con ' '
			//y despues se llena con el id con la funcion sprintf
			char nombreArchivo[7];
			int lenght=0;
			for(i=0;i<sizeof(nombreArchivo);i++){
				nombreArchivo[i]=' ';
			}
			sprintf(nombreArchivo,"%lu",idRegistro);
			char existe;
			//Se espera un 0 del servidor si la historia clinica no estaba creada
			//o un 1 en el caso de que este creada, esto se guarda en la variable
			//de nombre existe
			returned=recv(clienteDesc,&existe,sizeof(existe),0);
			if(returned==-1){
				perror("Error en ver: recv(existe)");
				exit(-1);	
			}
			//Independientemente de que la historia clinica exista o no, se va a crear
			//un comando de maximo 17 caracteres que dira 'gedit ' concatenado con el id
			//de la historia clinica a abrir. Con ello la terminal ejectua este comando
			//y se abre el archivo txt, este comando se guarda en un vector llamado
			//comando y se ejecuta con la funcion system()
			if(existe=='0'){
				clear();
				printf("El registro %lu no tiene historia clinica aun\nSe creara automaticamente\n\n",idRegistro);
				char comando[17];
				for(i=0;i<sizeof(comando);i++){
					comando[i]=' ';
				}
				comando[0]='g';
				comando[1]='e';
				comando[2]='d';
				comando[3]='i';
				comando[4]='t';
				comando[5]=' ';
				for(i=6;i<sizeof(comando);i++){
					comando[i]=nombreArchivo[i-6];
				}
				system(comando);
			}else if(existe=='1'){
				clear();
				printf("Se abrira un archivo con la historia clinica del registro %lu\n\n",idRegistro);
				char comando[17];
				for(i=0;i<sizeof(comando);i++){
					comando[i]=' ';
				}
				comando[0]='g';
				comando[1]='e';
				comando[2]='d';
				comando[3]='i';
				comando[4]='t';
				comando[5]=' ';
				for(i=6;i<sizeof(comando);i++){
					comando[i]=nombreArchivo[i-6];
				}
				system(comando);
			}
		}else{
			//En el caso de no confirmacion se notifica
			clear();
			printf("No ha abierto la historia clinica del registro %lu",idRegistro);
			printf("\n\n");
		}
		
	}else{
		//Si no hay registros se notifica
		clear();
		printf("\nNo hay registros en la tabla\n\n");
	}
}

void buscar(int clienteDesc){
	/*
		Esta funcion escribe una lista con los ids,nombres,tipo y raza
		de las mascotas que tengan como nombre una cadena escrita por el usuario
		Para esto se usan la siguientes variables:
			-i que son variables iteradoras de los bucles for
			-registrosEncontrados que es una variable que guarda el numero de 
			 registros que coincidieron con la cadena a buscar
			-lenght que son variables enteras que miden el largo de una cadena
			-returned que es un entero que guarda los valores retornados por send y recv
			 para poder manipular los errores
			-answer que es una variable para manipular los estados del servidor:
				1 significa que va a imprimir un nombre 
				2 significa que ya va a dejar de imprimir nombres
				0 significa que no hay registros que coincidan con la cadena digitada
			 esta variable cambia segun lo enviado por el servidor
			-sigueCoincidiendo es un booleano que verifica que caracter a caracter
			 la cadena coindica con el i-esimo registro
			-existe es un booleano que indica que por lo menos hubo un registro
			 que coincidio en la busqueda
			-mascotaEncontrada es una estructura dogType que almacena la estructura
			 leida en cada iteracion del bucle for
	*/
	long i=0,registrosEncontrados=0;
	int j=0,lenght=0,returned,answer;
	bool sigueCoincidiendo=true,existe=false;
	char nombreIngresado[32];
	struct dogType mascotaEncontrada;
	//Se obtiene la cadena a buscar
	clear();
	printf("---------------------BUSCAR----------------------\n\n");
	printf("Que registro quiere buscar (nombre)?\n");
	scanf("%s",nombreIngresado);
	//Se convierte a mayusculas para facilitar la busqueda
	for(j=0;j<32;j++){
		nombreIngresado[j]=toupper(nombreIngresado[j]);	
	}
	for(j=0;j<32;j++){
		if(nombreIngresado[j]=='\0'){
			lenght=j;
			break;
		}
	}
	//Se envia al servidor la cadena a buscar
	returned=send(clienteDesc,nombreIngresado,sizeof(nombreIngresado),0);
	if(returned==-1){
		perror("Error en buscar: send(nombreIngresado)");
		exit(-1);	
	}
	//Se envia el largo de la cadena al servidor
	returned=send(clienteDesc,&lenght,sizeof(lenght),0);
	if(returned==-1){
		perror("Error en buscar: send(lenght)");
		exit(-1);	
	}
	//Se recibe la respuesta del servidor para empezar a iterar por cada registro
	returned=recv(clienteDesc,&answer,sizeof(answer),0);
	if(returned==-1){
		perror("Error en buscar: recv(answer)");
		exit(-1);	
	}
	clear();
	do{
		if(answer==0){
			//Si el servidor envio un 0 es porque ninguna estructura coincidio
			//con la cadena digitada, se pone answer en 2 para salir del bucle
			printf("No hay registros con nombre ");
			printf("%s",nombreIngresado);
			answer=2;
		}else if(answer==1){
			//Si answer es 1 es porque se empezará a escribir un nombre
			//Se recibe el id
			existe=true;
			returned=recv(clienteDesc,&i,sizeof(i),0);
			if(returned==-1){
				perror("Error en buscar: recv(i)");
				exit(-1);	
			}
			//Se recibe la mascota en la estructura mascotaEncontrada
			returned=recv(clienteDesc,&mascotaEncontrada,sizeof(mascotaEncontrada),0);
			if(returned==-1){
				perror("Error en buscar: recv(mascotaEncontrada)");
				exit(-1);	
			}
			//Se imprime con el formato id. Nombre(Tipo Raza)
			printf("\t%lu. ",i);
			for(j=0;j<32;j++){
				if(mascotaEncontrada.nombre[j]=='@'){
					break;
				}else{
					printf("%c",mascotaEncontrada.nombre[j]);
				}
			}
			printf(" (");
			for(j=0;j<32;j++){
				if(mascotaEncontrada.tipo[j]=='@'){
					break;
				}else{
					printf("%c",mascotaEncontrada.tipo[j]);
				}
			}
			printf(" ");
			for(j=0;j<16;j++){
				if(mascotaEncontrada.raza[j]=='@'){
					break;
				}else{
					printf("%c",mascotaEncontrada.raza[j]);
				}
			}
			printf(")\n");
			//Se espera el siguiente estado enviado por el servidor
			returned=recv(clienteDesc,&answer,sizeof(answer),0);
			if(returned==-1){
				perror("Error en buscar: recv(answer)");
				exit(-1);	
			}
		}
	//El bucle while se acaba cuando el servidor envia un 2
	}while(answer!=2);
	if(existe){
		//Si hubo por lo menos un registro que coincidio
		//Se pide al servidor el numero de registros que coincidieron
		//y se imprime
		returned=recv(clienteDesc,&registrosEncontrados,sizeof(registrosEncontrados),0);
		if(returned==-1){
			perror("Error en buscar: recv(registrosEncontrados)");
			exit(-1);	
		}
		printf("\n\n%lu registros encontrados",registrosEncontrados);
	}
	printf("\n\n");
}

int main(){
	//Implementacion cliente
	int clienteDesc, returned;
	struct sockaddr_in socketCliente;
	char buffer[128];
	clienteDesc=socket(AF_INET,SOCK_STREAM,0);
	if(clienteDesc==-1){
		perror("Error en main: socket()");
		exit(-1);
	}
	socketCliente.sin_family = AF_INET;
	socketCliente.sin_port = htons(PUERTO);
	socketCliente.sin_addr.s_addr=inet_addr("127.0.0.1");
	returned=connect(clienteDesc, (struct sockaddr *)&socketCliente, (socklen_t)sizeof(struct sockaddr));
	if(returned==-1){
		perror("Error en main: connect()");
		exit(-1);
	}
	
	//Inicio ejecucion 
	char opcionMenu='@';
	clear();
	while(opcionMenu!='5'){
		while(imprimirError('1','5',opcionMenu)){
			//Se verifica que la opcion no sea un salto de linea obtenido por getchar
			if(opcionMenu!=(char)10){
				imprimirMenu();
			}
			opcionMenu=getchar();
		}
		//Envia la opcion digitada
		returned=send(clienteDesc,&opcionMenu,sizeof(opcionMenu),0);
		if(returned==-1){
			perror("Error en main: send(opcionMenu)");
			exit(-1);	
		}
		switch(opcionMenu){
			case '1':
				ingresar(clienteDesc,buffer);
				opcionMenu='@';
			break;
			
			case '2':
				clear();
				ver(clienteDesc,buffer);
				opcionMenu='@';
			break;
			
			case '3':
				clear();
				borrar(clienteDesc,buffer);
				opcionMenu='@';
			break;
			
			case '4':
				clear();
				buscar(clienteDesc);
				opcionMenu='@';
			break;
			
			case '5':
				clear();
				printf("Fin de la ejecucion . . .\n\n");
			break;
		}
	}
    	returned=close(clienteDesc);
	if(returned==-1){
		perror("Error en main: close(clienteDesc)");
		exit(-1);
	}
	return 0;
}
