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
#define NOCLIENTES 32
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

void generarArchivoAleatorio(){
	printf("GENERANDO ARCHIVO ALEATORIO . . .\n\n");
	//Se crean arreglos que tengan las posibles casillas de una estructura
	char tipo[7][32]={{"Canino@"},{"Felino@"},{"Pez@"},{"Ave@"},{"Hamnster@"},{"Conejo@"},{"Tortuga@"}};
	char raza[28][32]={{"Beagle@"},{"BullDog@"},{"Pug@"},{"PitBull@"},                                //0-3 perros
			{"Siames@"},{"Persa@"},{"Bengala@"},{"Abisinio@"},                                 //4-7 gatos
			{"Dorado@"},{"Gato@"},{"Angel@"},{"Guppy@"},                                       //8-11 peces
			{"Perico@"},{"Canario@"},{"Cacatua@"},{"Loro@"},                                   //12-15 Aves
			{"Angora@"},{"Chino@"},{"Albino@"},{"Dorado@"},                                    //16-19 Hamnsters
			{"Lionhead@"},{"Ingles@"},{"Holandes@"},{"Blanco@"},                               //20-23 Conejos
			{"Florida@"},{"Pintada@"},{"China@"},{"Rusa@"}                                     //24-27 Tortugas
			};
                    
	//1719 nombres en el archivo de texto de 32(maximo) caracteres cada uno
	char nombres[1719][32];
	int returned;
	//Se abre el archivo nombreMascotas.txt para obtener los nombres de las mascotas
	char caracterASCII;
	int contadorNombre=0,contadorLetras=0;
	bool primero=true;
	FILE *apuntadorAArchivoTexto;

	apuntadorAArchivoTexto= fopen("nombresMascotas.txt","r");     
	if(apuntadorAArchivoTexto==NULL){
		perror("Error en generarArchivoAleatorio: fopen(nombresMascotas.txt) en modo r");
		exit(-1);
	}
     
	while(caracterASCII!=EOF){
		//Ejecuta este while hasta que se acabe el archivo de texto
		caracterASCII=fgetc(apuntadorAArchivoTexto);
		if((int)caracterASCII>64 && (int)caracterASCII<91){
			//Entra aca si el caracter leido es una mayuscula
			if(primero){
				//Se usa el bool primero para no desperdiciar el espacio 0 de la matriz
				caracterASCII=toupper(caracterASCII);
				primero=false;
				nombres[contadorNombre][contadorLetras]=caracterASCII;
				contadorLetras++;
			}else{
				//Si se obtiene una mayuscula y no es la mayuscula del primerNombre   
				caracterASCII=toupper(caracterASCII);
				nombres[contadorNombre][contadorLetras]=(char)64;
				contadorNombre++;
				contadorLetras=0;
				nombres[contadorNombre][contadorLetras]=caracterASCII;
				contadorLetras++;
			}
		}else if((int)caracterASCII>96){
			//Entra aca si el caracter leido es una minuscula
			caracterASCII=toupper(caracterASCII);
			nombres[contadorNombre][contadorLetras]=caracterASCII;
			contadorLetras++;
		}
	}

	//Poner la ultima letra del ultimo nombre en @
	nombres[contadorNombre][contadorLetras]=(char)64;               
	returned=fclose(apuntadorAArchivoTexto);
	if(returned!=0){
		perror("Error en generarArchivoAleatorio: fclose(apuntadorAArchivoTexto)");
		exit(-1);
	}

	//Creación de las estructuras aleatorias
	FILE *apuntadorAArchivoBinario;
	apuntadorAArchivoBinario=fopen("dataDogs.dat","wb");
	if(apuntadorAArchivoBinario==NULL){
		perror("Error en generarArchivoAleatorio: fopen(dataDogs.dat) en modo wb");
		exit(-1);
	}
	long int i=0;
	int j=0;
	srand (time(NULL));  
	for(i=0;i<10000000;i++){
		//Este for se ejecuta 10000000 veces para crear el archivo binario aleatorio    
		struct dogType perroAux;
		int aleatorio = rand() % 1719;
		bool flag=false;
		//flag es un bool que indica cuando se llega al final del nombre (es decir un @)
		for(j=0;j<32;j++){
			if(flag){
				//Se rellena el resto del char[32] con @
				perroAux.nombre[j]=(char)64;
			}else if(nombres[aleatorio][j]==(char)64){
				//Cuando se llega al final del nombre flag se vuelve true 
				//y se empieza a rellenar el resto de char[32] con @
				perroAux.nombre[j]=nombres[aleatorio][j];
				flag=true;
			}else{
				//Entra aca si el nombre se sigue leyendo(no se ha finalizado de leer) 
				//y se asigna el caracter correspondiente a la estructura
				perroAux.nombre[j]=nombres[aleatorio][j];
			}
		}
	
		//Se vuelve flag falsa para aplicar el mismo procedimiento de nombre con tipo
		flag=false;
		aleatorio=rand()%7;
		for(j=0;j<32;j++){
			if(flag){
				//Se entra aca para rellenar el resto del char[32] con @
				perroAux.tipo[j]=(char)64;     
			}else if(tipo[aleatorio][j]==(char)64){
				//Se entra aca si se acabó el nombre (aparición de @) 
				//Se notifica con flag=true y se empieza a rellenar con @ el resto del char[32]
				perroAux.tipo[j]=tipo[aleatorio][j];
				flag=true;
			}else{
				//Se entra acá si todavia no se ha terminado de leer el nombre
				//Se asigna el caracter correspondiente a la estructura creada
				perroAux.tipo[j]=tipo[aleatorio][j];
			}
		}
        
		//Se vuelve flag falsa para aplicar el mismo procedimiento para el arreglo de raza
        	flag=false;
		int suma=rand()%4;
		aleatorio=(4*aleatorio)+suma;
		for(j=0;j<16;j++){
			if(flag){
				//Se entra aca si ya se termino de leer el nombre
				//Se rellena el resto del array con @
				perroAux.raza[j]=(char)64;
			}else if(raza[aleatorio][j]==(char)64){
				//Se entra aca si se leyo el final del nombre (aparicion de @)
				//Se notifica con flag= true para que el resto del array se rellene con @
				perroAux.raza[j]=raza[aleatorio][j];
				flag=true;
			}else{
				//Se entra acá si no se ha terminado de leer el nombre
				//Se asigna el caracter correspondiente a la estructura creada
				perroAux.raza[j]=raza[aleatorio][j];
			}
		}
		//La edad queda siendo un numero entre 1 y 14
		perroAux.edad=(rand()%15)+1;    
		//La estatura es un numero entre 20 y 119         
		perroAux.estatura=(rand()%100) +20;
		//El peso es un numero entre 1 y 15
		aleatorio=(rand()%15)+1;
		perroAux.peso=aleatorio;
		//0=Hembra 1=Macho
		aleatorio=rand()%2;
		if(aleatorio==0){
			perroAux.sexo='H';
		}else{
			perroAux.sexo='M';
		}
		//Se escribe la estructura en el archivo binario
		returned=fwrite(&perroAux,sizeof(struct dogType),1,apuntadorAArchivoBinario);
		if(returned==-1){
			perror("Error en generarArchivoAleatorio: fwrite(perroAux)");
			exit(-1);
		}
	}
	returned=fclose(apuntadorAArchivoBinario);
	if(returned!=0){
		perror("Error en generarArchivoAleatorio: fclose(apuntadorAArchivoBinario)");
		exit(-1);
	}
	printf("Archivo aleatorio generado\n\n");
}

long nRegistrosExistentes(int clienteDesc){
	/*
		Esta es una funcion que busca en el archivo
		dataDogs.dat cuantos archivos existentes hay,
		guarda este numero y lo envia al cliente y asimismo
		lo retorna en la funcion, usa las siguientes instancias:
			-apuntadorAArchivoBinario que es un apuntador al archivo dataDogs.dat
			-returned que es un entero que guarda el valor retornado por las funciones
			 para manipular el fichero y la funcion send. Sirve unicamente para 
			 validar errores
			-nRegistrosExistentes que es un long donde se guardara el valor a retornar
			 de la funcion
			-clienteDesc que es descriptor del cliente que esta pidiendo esta funcion
	
	*/
		FILE *apuntadorAArchivoBinario;
		int returned;
		long i,nRegistrosExistentes=0;

	/*
		Se abre el archivo dataDogs.dat en modo lectura binaria y se valida
		el error. Despues, se leen cada una de las estructuras en el archivo
		y se verifica si la edad es diferente a -1, si es asi, significa que
		en esa estructura leida hay una mascota y por lo tanto se debe sumar 
		en uno nRegistrosExistentes. Asi sea hace para cada una de las estructuras.
		Despues se cierra el archivo dataDogs.dat
	*/
		apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
		if(apuntadorAArchivoBinario==NULL){
			perror("Error en nRegistrosExistentes: fopen dataDogs.dat rb");
			exit(-1);
		}
		for(i=0;i<10000000;i++){
			struct dogType perroAux;
		  	fread(&perroAux,sizeof(struct dogType),1,apuntadorAArchivoBinario);
			if(perroAux.edad!=-1){
				nRegistrosExistentes++;
			}
		}
		returned=fclose(apuntadorAArchivoBinario);
		if(returned!=0){
			perror("Error en nRegistrosExistentes: fclose(apuntadorAArchivoBinario)");
			exit(-1);
		}
	/*
		Se envia el valor de nRegistrosExistentes al cliente por medio de un send
		y se verifica el error. Despues se retorna nRegistrosExistentes
	*/
		returned=send(clienteDesc,&nRegistrosExistentes,sizeof(nRegistrosExistentes),0);
		if(returned==-1){
			perror("Error en nRegistrosExistentes: send(nRegistrosExistentes)");
			exit(-1);	
		}
		return nRegistrosExistentes;
}

bool noExistenRegistros(int clienteDesc){
	/*
		Esta funcion sirve para verificar que en el archivo dataDogs.dat
		hay por lo menos un registro vacio(TRUE) o no lo hay(FALSE) y notificar al cliente de esto. 
		Para ello se usaran las siguientes instancias:
			-returned que es un entero que guarda el valor retornado
			 por las funciones para manipular ficheros y por las funcion send
			-apuntadorAArchivoBinario que es un apuntador al archivo dataDogs.dat
			-i que es un long para iterar
			-clienteDesc que tiene el valor del descriptor del cliente
	*/
		int returned;
		FILE *apuntadorAArchivoBinario;
		long i=0;
	
	/*
		Se abre el archivo dataDogs.dat en modo lectura binaria, se verifica 
		el error, despues se itera en cada estructura y si es encuentra una
		que tenga edad=-1 es porque alli hay un espacio vacio entonces 
		inmediatamente se retorna falso y se envia al cliente la cadena '0'
		indicando falso. Si se leen todas las estructuras y no se ha retornado
		falso es porque todas tienen una edad diferente a -1 entonces no hay
		espacios vacios, por lo tanto se retorna verdadero y se envia 
		al cliente la cadena '1' indicando verdadero
	*/
		apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
		if(apuntadorAArchivoBinario==NULL){
			perror("Error en noExistenRegistros: fopen dataDogs.dat rb");
			exit(-1);
		}
		for(i=0;i<10000000;i++){
			struct dogType perroAux;
		  	fread(&perroAux,sizeof(struct dogType),1,apuntadorAArchivoBinario);
			if(perroAux.edad==-1){
				returned=send(clienteDesc,"0",128, 0);
				if(returned==-1){
					perror("Error en noExistenRegistros: send(0)");
					exit(-1);	
				}
				return false;
				break;
			}
		}
		fclose(apuntadorAArchivoBinario);
		returned=send(clienteDesc,"1",128, 0);
		if(returned==-1){
			perror("Error en noExistenRegistros: send(1)");
			exit(-1);	
		}
		return true;
}

bool existenRegistros(int clienteDesc){
	/*
		Esta funcion sirve para verificar que por lo menos hay un registro
		en el archivo dataDogs.dat(TRUE) y por lo tanto se puede hacer una accion
		de borrado, asimismo se notifica este resultado al cliente. Para ello
		se usaran las siguientes instancias:
			-returned que es un entero que recibe los resultados de las funciones
			 de manipulacion de archivos y de send para verificar el error
			-enviar que es un entero que tiene valor 1 para enviar TRUE al cliente
			 y 2 para enviar FALSE al cliente. Este valor es el enviado al cliente
			 para saber el estado del archivo.
			-i que es un long para iterar
			-apuntadorAArchivoBinario que es un apuntador al archivo dataDogs.dat
			-clienteDesc que es un parametro de la funcion que tiene el numero
			 del descriptor del cliente
	*/
		int returned,enviar=-1;
		bool existenRegistros=false;
		long i=0;
		FILE *apuntadorAArchivoBinario;
	/*
		Se abre el archivo dataDogs.dat en modo rb y se leen todas las estructuras.
		Si se encuentra que alguna tiene el atributo edad diferente a -1 significa
		que alli hay una mascota ingresada y por lo tanto la funcion debe retornar
		TRUE, se pone el valor de 1 a enviar y se envia al cliente por medio de un send
	*/
		apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
		if(apuntadorAArchivoBinario==NULL){
			perror("Error en existenRegistros: fopen(dataDogs.dat) en modo rb");
			exit(-1);
		}
		for(i=0;i<10000000;i++){
			struct dogType perroAux;
		  	fread(&perroAux,sizeof(struct dogType),1,apuntadorAArchivoBinario);
			if(perroAux.edad!=-1){
				enviar=1;
				returned=send(clienteDesc,&enviar,sizeof(enviar),0);
				if(returned==-1){
					perror("Error en existenRegistros: send(enviar)");
					exit(-1);	
				}
				return true;
				break;
			}
		}
	/*
		Si se salio del for anterior es porque todas las estructuras tenian el valor
		de -1 en la edad y por lo tanto estaban vacias, entonces se pone 2 en enviar
		y se envia este valor al cliente por medio de un send, ademas se retorna FALSE
	*/
		returned=fclose(apuntadorAArchivoBinario);
		if(returned!=0){
			perror("Error en existenRegistros: fclose(apuntadorAArchivoBinario)");
			exit(-1);
		}
		enviar=2;
		returned=send(clienteDesc,&enviar,sizeof(enviar),0);
		if(returned==-1){
			perror("Error en existenRegistros: send(enviar)");
			exit(-1);	
		}
		return false;

}

bool registroVacio(long id,int clienteDesc){
	/*
		Esta funcion recibe un id y un clienteDesc que es el descriptor
		del cliente. Esta funcion sirve para saber si en la posicion
		id del archivo dataDogs.dat hay un espacio vacio(TRUE). Para ello
		se usaran las siguientes variables:
			-returned que es entero para manipular los errores
			 de las funciones de archivos y de la funcion send
			-apuntadorAArchivoBinario que es un apuntador al archivo dataDogs.dat
			-perroAux que es una estructura dogType donde se guardara
			 la mascota de la posicion id y saber si alli hay un espacio vacio
			 o no
	*/
		int returned;
		FILE *apuntadorAArchivoBinario;
		struct dogType perroAux;

	/*
		Se abre el archivo dataDogs.dat en modo lectura y se verifica el error.
		Despues se posiciona el puntero apuntadorAArchivoBinario en la posicion
		del id por medio de la funcion fseek y se verifica el error. Luego se lee
		el dato en esta posicion por medio de fread y se guarda en perroAux, se verifica
		que el atributo edad sea diferente a -1, con esto se sabra si alli hay alguna
		mascota ingresada(FALSE) o no(TRUE). Se envia un 1 al cliente si alli no hay
		alguna mascota y un 0 si alli hay una mascota, asimismo se retorna el TRUE
		o FALSE correspondiente
	*/
		apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
		if(apuntadorAArchivoBinario==NULL){
			perror("Error en registroVacio: fopen(dataDogs.dat) en modo rb");
			exit(-1);
		}
		returned=fseek(apuntadorAArchivoBinario,96*id,SEEK_SET);
		if(returned==-1){
			perror("Error en registroVacio: fseek(apuntadorAArchivoBinario)");
			exit(-1);
		
		}
		fread(&perroAux,sizeof(struct dogType),1,apuntadorAArchivoBinario);
		if(perroAux.edad!=-1){
			returned=fclose(apuntadorAArchivoBinario);
			if(returned!=0){
				perror("Error en registroVacio: fclose(apuntadorAArchivoBinario)");
				exit(-1);
			}
			returned=send(clienteDesc,"0",sizeof(char),0);
			if(returned==-1){
				perror("Error en registroVacio: send(0)");
				exit(-1);	
			}
			return false;
		}
		returned=fclose(apuntadorAArchivoBinario);
		if(returned!=0){
			perror("Error en registroVacio: fclose(apuntadorAArchivoBinario)");
			exit(-1);
		}
		returned=send(clienteDesc,"1",sizeof(char),0);
		if(returned==-1){
			perror("Error en registroVacio: send(1)");
			exit(-1);	
		}
		return true;
	
}

void ingresar(int clienteDesc,char buffer[128],struct sockaddr_in socketCliente){
	/*
		Esta funcion agrega mascotas al archivo dataDogs.dat. En primer
		lugar usa la funcion noExistenRegistros para saber si hay
		algun lugar donde ingresar algo, si esta retorna FALSE dejara
		hacer una insercion.
	*/
		printf("Cliente %i pidio ingresar\n\n",clienteDesc);
		if(!noExistenRegistros(clienteDesc)){
			/*
				Hasta aca el cliente debe tener datos almacenados de la mascota
				a ingresar y el servidor esta esperando una confirmacion, para saber
				si ingresar o no la mascota a el archivo dataDogs.dat. Esta confirmacion
				se recibe con un recv y se almacena en el buffer[0] dado como argumento
				en la funcion. Se verifica el error
			*/
			int returned;
			returned=recv(clienteDesc,buffer,128,0);
			if(returned==-1){
				perror("Error en ingresar: recv(buffer)");
				exit(-1);	
			}
			if(buffer[0]=='1'){
				//Si se confirmo el ingreso, se reciben los datos digitados en el cliente
				//por medio de la funcion recv y se almacena en las variables mostradas
				//a continuacion
				char nombreIngresado[32];
				char tipoIngresado[32];
				int edadIngresada;
				char razaIngresada[16];
				int estaturaIngresada;
				float pesoIngresado;
				char sexoIngresado;
				returned=recv(clienteDesc,nombreIngresado,sizeof(nombreIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: recv(nombreIngresado)");
					exit(-1);	
				}
				returned=recv(clienteDesc,tipoIngresado,sizeof(tipoIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: recv(tipoIngresado)");
					exit(-1);	
				}
				returned=recv(clienteDesc,razaIngresada,sizeof(razaIngresada),0);
				if(returned==-1){
					perror("Error en ingresar: recv(razaIngresada)");
					exit(-1);	
				}
				returned=recv(clienteDesc,&sexoIngresado,sizeof(sexoIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: recv(sexoIngresado)");
					exit(-1);	
				}
				returned=recv(clienteDesc,&edadIngresada,sizeof(edadIngresada),0);
				if(returned==-1){
					perror("Error en ingresar: recv(edadIngresada)");
					exit(-1);	
				}
				returned=recv(clienteDesc,&estaturaIngresada,sizeof(estaturaIngresada),0);
				if(returned==-1){
					perror("Error en ingresar: recv(estaturaIngresada)");
					exit(-1);	
				}
				returned=recv(clienteDesc,&pesoIngresado,sizeof(pesoIngresado),0);
				if(returned==-1){
					perror("Error en ingresar: recv(pesoIngresado)");
					exit(-1);	
				}
				printf("\tDatos recibidos de Cliente %i al ingresar\n",clienteDesc);

				//Se vuelve a mayusculas el nombre para ingresarlo al archivo,
				//esto se hace para facilidad en la funcion buscar(). Despues
				//se llena la estructura mascotaAIngresar con los datos enviados
				//por el cliente
				long id=0;
				int i=0;
				bool flag=false;
				struct dogType mascotaAIngresar;
				for(i=0;i<32;i++){
					nombreIngresado[i]=toupper(nombreIngresado[i]);
				}
				for(i=0;i<32;i++){
					if(flag){
						//Marcador de final de nombre es @
						mascotaAIngresar.nombre[i]=(char)64;
					}else if(nombreIngresado[i]=='\0'){
						mascotaAIngresar.nombre[i]=(char)64;
						flag=true;
					}else{
						mascotaAIngresar.nombre[i]=nombreIngresado[i];
					}
				}
				flag=false;
				for(i=0;i<32;i++){
					if(flag){
						//Marcador de final de nombre es @
						mascotaAIngresar.tipo[i]=(char)64;
					}else if(tipoIngresado[i]=='\0'){
						mascotaAIngresar.tipo[i]=(char)64;
						flag=true;
					}else{
						mascotaAIngresar.tipo[i]=tipoIngresado[i];
					}
				}
				flag=false;
				for(i=0;i<16;i++){
					if(flag){
						//Marcador de final de nombre es @
						mascotaAIngresar.raza[i]=(char)64;
					}else if(razaIngresada[i]=='\0'){
						mascotaAIngresar.raza[i]=(char)64;
						flag=true;
					}else{
						mascotaAIngresar.raza[i]=razaIngresada[i];
					}
				}
				mascotaAIngresar.edad=edadIngresada;
				mascotaAIngresar.sexo=sexoIngresado;
				mascotaAIngresar.estatura=estaturaIngresada;
				mascotaAIngresar.peso=pesoIngresado;

				//Se agrega la estructura mascotaAIngresar al archivo dataDogs.dat
				//Para ello se hace una busqueda hasta encontrar un espacio vacio
				//y se almacena alli la estructura
				FILE *apuntadorAArchivoBinario;
				apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
				if(apuntadorAArchivoBinario==NULL){
					perror("Error en ingresar: fopen(dataDogs.dat) en modo rb");
					exit(-1);
				}
				//Con este for se obtiene el id a donde ingresar, se busca
				//hasta encontrar una edad igual a -1, alli es donde hay un
				//registro vacio
				for(i=0;i<10000000;i++){
					struct dogType perroAux;
				  	fread(&perroAux,sizeof(struct dogType),1,apuntadorAArchivoBinario);
					if(perroAux.edad==-1){
						id=i;
						break;
					}
				}
				fclose(apuntadorAArchivoBinario);

				//Ingresar en serverDogs.dat
				time_t t;
				struct tm *tm;
				char fechayhora[100];
				int lenghtBuffer;
				t=time(NULL);
				tm=localtime(&t);
				strftime(fechayhora, 100, "%d/%m/%Y", tm);
				for(i=0;i<100;i++){
					if(fechayhora[i]=='\0'){
						lenghtBuffer=i;
						break;
					}
				}
				
				apuntadorAArchivoBinario=fopen("serverDogs.dat","r+b");
				if(apuntadorAArchivoBinario==NULL){
					perror("Error en ingresar: fopen(serverDogs.dat) en modo r+b");
					exit(-1);
				}
				
				fwrite(&fechayhora,sizeof(fechayhora),1,apuntadorAArchivoBinario);
				fwrite(&socketCliente.sin_addr.s_addr,sizeof(socketCliente.sin_addr.s_addr),1,apuntadorAArchivoBinario);
				fwrite("insercion",9*sizeof(char),1,apuntadorAArchivoBinario);
				fwrite(&mascotaAIngresar.nombre,sizeof(mascotaAIngresar.nombre),1,apuntadorAArchivoBinario);
				fclose(apuntadorAArchivoBinario);

				//Teniendo el id de donde ingresar la estructura, se posiciona
				//el puntero apuntadorAArchivoBinario con fseek y se escribe
				//con fwrite la estructura
				apuntadorAArchivoBinario=fopen("dataDogs.dat","r+b");
				if(apuntadorAArchivoBinario==NULL){
					perror("Error en ingresar: fopen(dataDogs.dat) en modo r+b");
					exit(-1);
				}
				returned=fseek(apuntadorAArchivoBinario,96*id,SEEK_SET);
				if(returned==-1){
					perror("Error en ingresar: fseek(apuntadorAArchivoBinario)");
					exit(-1);
				}
				fwrite(&mascotaAIngresar,sizeof(struct dogType),1,apuntadorAArchivoBinario);
				fclose(apuntadorAArchivoBinario);
				//Se envia el id al cliente para que este sepa en que lugar del
				//archivo se guardo la mascota ingresada
				returned=send(clienteDesc,&id,sizeof(id),0);
				if(returned==-1){
					perror("Error en ingresar: send(id)");
					exit(-1);	
				}
				printf("\tCliente %i agrego a ",clienteDesc);
				for(i=0;i<32;i++){
					if(mascotaAIngresar.nombre[i]=='@'){
						break;
					}else{
						printf("%c",mascotaAIngresar.nombre[i]);
					}	
				}
				printf(" correctamente con id=%lu\n\n",id);
			}
		}else{
			//En el caso de que el archivo dataDogs.dat tenga todos sus registros llenos
			printf("\tCliente %i pidio ingresar pero la tabla esta llena . . . \n\n",clienteDesc);	
		}
		printf("Cliente %i termino funcion ingresar\n\n",clienteDesc);
}

void borrar(int clienteDesc,char buffer[128],struct sockaddr_in socketCliente){
	printf("Cliente %i decidio borrar . . .\n\n",clienteDesc);
	/*
		Esta funcion sirve para borrar una estructura del archivo dataDogs.dat
		Primero se verifica que exista por lo menos un registro, para asi
		poder eliminar alguna estructura, esto se hace con la funcion
		existenRegistros
	*/
	if(existenRegistros(clienteDesc)){
		/*
			En el caso de que exista por lo menos un registro para borrar
			se usan las siguientes variables para hacer el borrado:
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
		//EL servidor debe estar sincronizado con el cliente
		//asi que se hace el mismo bucle while que tiene la funcion
		//borrar del cliente
		while(registroVacio(idRegistro,clienteDesc) || !correcto){
			if(!correcto && !primero){
				correcto=true;
			}
			correcto=true;
			primero=false;
			//Se recibe el id enviado por el cliente
			returned=recv(clienteDesc,&idRegistro,sizeof(idRegistro),0);
			if(returned==-1){
				perror("Error en borrar: recv(idRegistro)");
				exit(-1);	
			}
			if(idRegistro<0){
				idRegistro=0;
				correcto=false;
			}
		}
		//Se recibe la confirmacion del cliente
		printf("\tCliente %i decidio borrar registro %lu\n\n",clienteDesc,idRegistro);
		returned=recv(clienteDesc,&confirmacion,sizeof(confirmacion),0);
		if(returned==-1){
				perror("Error en borrar: recv(confirmacion)");
				exit(-1);	
		}
		if(confirmacion=='1'){
			//En el caso de que se confirme el borrado se abre
			//el archivo dataDogs.dat y se pone una estructura
			//vacia en la posicion del id
			FILE *apuntadorAArchivoBinario;
			struct dogType mascotaReemplazada;
			mascotaReemplazada.edad=-1;
			apuntadorAArchivoBinario=fopen("dataDogs.dat","r+b");
			if(apuntadorAArchivoBinario==NULL){
				perror("Error en borrar: fopen(dataDogs.dat) en modo r+b");
				exit(-1);
			}
			fseek(apuntadorAArchivoBinario,96*idRegistro,SEEK_SET);
			fwrite(&mascotaReemplazada,sizeof(struct dogType),1,apuntadorAArchivoBinario);
			fclose(apuntadorAArchivoBinario);

			//Ingresar en serverDogs.dat
			time_t t;
			struct tm *tm;
			char fechayhora[100];
			int lenghtBuffer;
			t=time(NULL);
			tm=localtime(&t);
			strftime(fechayhora, 100, "%d/%m/%Y", tm);
			for(i=0;i<100;i++){
				if(fechayhora[i]=='\0'){
					lenghtBuffer=i;
					break;
				}
			}
				
			apuntadorAArchivoBinario=fopen("serverDogs.dat","r+b");
			if(apuntadorAArchivoBinario==NULL){
				perror("Error en borrar: fopen(serverDogs.dat) en modo r+b");
				exit(-1);
			}
				
			fwrite(&fechayhora,sizeof(fechayhora),1,apuntadorAArchivoBinario);
			fwrite(&socketCliente.sin_addr.s_addr,sizeof(socketCliente.sin_addr.s_addr),1,apuntadorAArchivoBinario);
			fwrite("borrado",7*sizeof(char),1,apuntadorAArchivoBinario);
			fwrite(&idRegistro,sizeof(idRegistro),1,apuntadorAArchivoBinario);
			fclose(apuntadorAArchivoBinario);
		}else{
			//No se hace nada en el caso de que no se confirme
			printf("\tPero no se confirmo . . .\n\n");
		}
	}else{
		//Si no hay nada para borrar se notifica
		printf("\tPero no hay registros para borrar en la tabla\n\n");
	}
}

void ver(int clienteDesc,char buffer[128],struct sockaddr_in socketCliente){
	/*
		Esta funcion sirve para abrir la historia clinica de una
		mascota ingresada previamente. Primero, se verifica
		que haya por lo menos un registro para ver con la funcion
		existenRegistros
	*/
	printf("Cliente %i pidio ver\n\n",clienteDesc);
	if(existenRegistros(clienteDesc)){
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
		while(registroVacio(idRegistro,clienteDesc) || !correcto){
			if(!correcto && !primero){
				correcto=true;
			}
			correcto=true;
			primero=false;
			//Se recibe el id del cliente
			returned=recv(clienteDesc,&idRegistro,sizeof(idRegistro),0);
			if(returned==-1){
				perror("Error en ver: recv(idRegistro)");
				exit(-1);	
			}
			if(idRegistro<0){
				idRegistro=0;
				correcto=false;
			}
		}
		//Se recibe la confirmacion del cliente
		printf("\tCliente %i decidio ver registro %lu\n\n",clienteDesc,idRegistro);
		returned=recv(clienteDesc,&confirmacion,sizeof(confirmacion),0);
		if(returned==-1){
				perror("Error en ver: recv(confirmacion)");
				exit(-1);	
		}
		if(confirmacion=='1'){
			//Si el cliente confirmo ver la historia clinica se abre
			//el archivo dataDogs.dat y se posicion el puntero
			//en la posicion correspondiente con fseek, despues
			//se extrae la mascota con fread y se guarda en 
			//la estructura mascotaEncontrada
			FILE *apuntadorAArchivoBinario;
			struct dogType mascotaEncontrada;
			apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
			if(apuntadorAArchivoBinario==NULL){
				perror("Error en ver: fopen(dataDogs.dat) en modo rb");
				exit(-1);
			}
			returned=fseek(apuntadorAArchivoBinario,96*idRegistro,SEEK_SET);
			if(returned==-1){
				perror("Error en ver: fseek(apuntadorAArchivoBinario)");
				exit(-1);
			}
			fread(&mascotaEncontrada,sizeof(struct dogType),1,apuntadorAArchivoBinario);
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
			
			//Se abre la historia clinica con el nombre de archivo id
			//si esta no existe, el puntero apuntara a NULL y esto implica
			//que la historia clinica no se habia creado antes asi que se crea
			FILE *apuntadorHC;
			apuntadorHC=fopen((const char *)nombreArchivo,"r");
			if(apuntadorHC==NULL){
				//Se envia un 0 al cliente para notificar que la historia clinica sera creada
				printf("\tCliente %i pidio ver un registro %lu sin historia clinica\n",clienteDesc,idRegistro);
				printf("\tEsta sera creada automaticamente . . . \n\n");
				returned=send(clienteDesc,"0",128,0);
				if(returned==-1){
					perror("Error en ver: send(0)");
					exit(-1);	
				}
				apuntadorHC=fopen((const char *)nombreArchivo,"w");
				//Se agregan cadenas de texto al archivo de la historia clinica con fputs y fwrite
				fputs("Historia clinica\n\n\tNombre: ",apuntadorHC);
				for(i=0;i<32;i++){
					if(mascotaEncontrada.nombre[i]<(char)65){
						break;
					}else{
						fwrite(&mascotaEncontrada.nombre[i],sizeof(char),1,apuntadorHC);
					}			
				}
				fputs("\n\tTipo: ",apuntadorHC);
				for(i=0;i<32;i++){
					if(mascotaEncontrada.tipo[i]<(char)65){
						break;
					}else{
						fwrite(&mascotaEncontrada.tipo[i],sizeof(char),1,apuntadorHC);
					}			
				}
				fputs("\n\tEdad: ",apuntadorHC);
				fprintf(apuntadorHC,"%i",mascotaEncontrada.edad);
				fputs("\n\tRaza: ",apuntadorHC);
				for(i=0;i<16;i++){
					if(mascotaEncontrada.raza[i]<(char)65){
						break;
					}else{
						fwrite(&mascotaEncontrada.raza[i],sizeof(char),1,apuntadorHC);
					}			
				}
				fputs("\n\tEstatura: ",apuntadorHC);
				fprintf(apuntadorHC,"%i",mascotaEncontrada.estatura);
				fputs("\n\tPeso: ",apuntadorHC);
				fprintf(apuntadorHC,"%f",mascotaEncontrada.peso);
		                fputs("\n\tSexo: ",apuntadorHC);
				fprintf(apuntadorHC,"%c",mascotaEncontrada.sexo);
				fputs("\n\n----------------------------------------------------\nObservaciones:\n\n",apuntadorHC);
				fclose(apuntadorHC);
				fclose(apuntadorAArchivoBinario);
				//Ingresar en serverDogs.dat
				time_t t;
				struct tm *tm;
				char fechayhora[100];
				int lenghtBuffer;
				t=time(NULL);
				tm=localtime(&t);
				strftime(fechayhora, 100, "%d/%m/%Y", tm);
				for(i=0;i<100;i++){
					if(fechayhora[i]=='\0'){
						lenghtBuffer=i;
						break;
					}
				}
				
				apuntadorAArchivoBinario=fopen("serverDogs.dat","r+b");
				if(apuntadorAArchivoBinario==NULL){
					perror("Error en ver: fopen(serverDogs.dat) en modo r+b");
					exit(-1);
				}
				
				fwrite(&fechayhora,sizeof(fechayhora),1,apuntadorAArchivoBinario);
				fwrite(&socketCliente.sin_addr.s_addr,sizeof(socketCliente.sin_addr.s_addr),1,apuntadorAArchivoBinario);
				fwrite("lectura",7*sizeof(char),1,apuntadorAArchivoBinario);
				fwrite(&idRegistro,sizeof(idRegistro),1,apuntadorAArchivoBinario);
				fclose(apuntadorAArchivoBinario);
			}else{
					//Si la historia clinica ya estaba creada se envia un 1 al cliente notificando
					printf("\tCliente %i pidio ver el registro %lu con historia clinica ",clienteDesc,idRegistro);
					printf("creada anteriormente\n\tEsta sera abierta automaticamente . . . \n\n");
					returned=send(clienteDesc,"1",128,0);
					if(returned==-1){
						perror("Error en ver: send(1)");
						exit(-1);	
					}
					fclose(apuntadorHC);
					fclose(apuntadorAArchivoBinario);
					//Ingresar en serverDogs.dat
					time_t t;
					struct tm *tm;
					char fechayhora[100];
					int lenghtBuffer;
					t=time(NULL);
					tm=localtime(&t);
					strftime(fechayhora, 100, "%d/%m/%Y", tm);
					for(i=0;i<100;i++){
						if(fechayhora[i]=='\0'){
							lenghtBuffer=i;
							break;
						}
					}
				
					apuntadorAArchivoBinario=fopen("serverDogs.dat","r+b");
					if(apuntadorAArchivoBinario==NULL){
						perror("Error en ver: fopen(serverDogs.dat) en modo r+b");
						exit(-1);
					}
				
					fwrite(&fechayhora,sizeof(fechayhora),1,apuntadorAArchivoBinario);
					fwrite(&socketCliente.sin_addr.s_addr,sizeof(socketCliente.sin_addr.s_addr),1,apuntadorAArchivoBinario);
					fwrite("lectura",7*sizeof(char),1,apuntadorAArchivoBinario);
					fwrite(&idRegistro,sizeof(idRegistro),1,apuntadorAArchivoBinario);
					fclose(apuntadorAArchivoBinario);
				}
		}else{
			printf("\tpero no se abrio la historia clinica . . .\n\n");		
		}
	}
	printf("Cliente %i acabo funcion ver\n\n",clienteDesc);
}

void buscar(int clienteDesc,char buffer[128],struct sockaddr_in socketCliente){
	/*
		Esta funcion accede al fichero dataDogs.dat y segun
		una cadena digitada por el usuario se busca las estructuras
		que coincidan en nombre con dicha cadena y las imprime.
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
	printf("Cliente %i decidio buscar . . .\n\n",clienteDesc);
	long i=0,registrosEncontrados=0,answer;
	int j=0,lenght=0,lenghtAux=0,returned;
	bool sigueCoincidiendo=true,existe=false,enviarAnswer=true;
	char nombreIngresado[32];
	//Recibe la cadena digitada por el usuario
	returned=recv(clienteDesc,&nombreIngresado,sizeof(nombreIngresado),0);
	if(returned==-1){
		perror("Error en buscar: recv(nombreIngresado)");
		exit(-1);	
	}
	//Recibe el largo de la cadena digitada por el usuario
	returned=recv(clienteDesc,&lenght,sizeof(lenght),0);
	if(returned==-1){
		perror("Error en buscar: recv(lenght)");
		exit(-1);	
	}
	printf("\tCliente %i buscando a ",clienteDesc);
	for(i=0;i<lenght;i++){
		printf("%c",nombreIngresado[i]);
	}
	printf("\n\n");
	//Abre el archivo y empieza a iterar en cada estructura
	FILE *apuntadorAArchivoBinario;
	struct dogType mascotaEncontrada;
	apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
	if(apuntadorAArchivoBinario==NULL){
		perror("Error en buscar: fopen dataDogs.dat rb");
		exit(-1);
	}		
	for(i=0;i<10000000;i++){
		fread(&mascotaEncontrada,sizeof(struct dogType),1,apuntadorAArchivoBinario);
		sigueCoincidiendo=true;
		for(j=0;j<lenght;j++){
			if(mascotaEncontrada.nombre[j]!=nombreIngresado[j]){
				//Si dejan de coincidar en un caracter se rompe el bucle
				sigueCoincidiendo=false;
				break;
			}
		}
		for(j=0;j<32;j++){
			if((mascotaEncontrada.nombre[j])<65 || (mascotaEncontrada.nombre[j])>122){
				//Busca el final de la cadena de la mascotaLeida
				//esta debe coincidir con el largo del nombre digitado
				//o sino son dos cadenas diferentes, por ejemplo:
				//no es lo mismo BEN que BENJAMIN asi coincidan en
				//tres caracteres
				lenghtAux=j;
				break;
			}
		}
		if(sigueCoincidiendo && lenghtAux==lenght){
			//Si coincidieron en todos los caracteres y en el largo
			//se aumenta el numero de registros encontrados
			//se notifica al cliente que se va a imprimir (answer=1)
			//y se envian los datos correspondientes(mascota e id)
			registrosEncontrados++;
			existe=true;
			answer=1;
			returned=send(clienteDesc,&answer,sizeof(answer),0);
				if(returned==-1){
				perror("Error en buscar: send(answer)");
				exit(-1);	
			}
			returned=send(clienteDesc,&i,sizeof(i),0);
			if(returned==-1){
				perror("Error en buscar: send(i)");
				exit(-1);	
			}
			returned=send(clienteDesc,&mascotaEncontrada,sizeof(mascotaEncontrada),0);
			if(returned==-1){
				perror("Error en buscar: send(mascotaEncontrada)");
				exit(-1);	
			}
		}
		
	}
	if(!existe){
		//Se manda un 0 al cliente para notificar 
		//que ninguna cadena coincidio en la busqueda
		answer=0;
		returned=send(clienteDesc,&answer,sizeof(answer),0);
		if(returned==-1){
			perror("Error en buscar: send(answer)");
			exit(-1);	
		}
	}else{	
		//Se envia un 2 al cliente para notificar el final de
		//las impresiones y se envia el numero de registros encontrados
		answer=2;
		returned=send(clienteDesc,&answer,sizeof(answer),0);
		if(returned==-1){
			perror("Error en buscar: send(answer)");
			exit(-1);	
		}
		returned=send(clienteDesc,&registrosEncontrados,sizeof(registrosEncontrados),0);
		if(returned==-1){
			perror("Error en buscar: send(registrosEncontrados)");
			exit(-1);	
		}	
	}
	//Ingresar en serverDogs.dat
	time_t t;
	struct tm *tm;
	char fechayhora[100];
	int lenghtBuffer;
	t=time(NULL);
	tm=localtime(&t);
	strftime(fechayhora, 100, "%d/%m/%Y", tm);
	for(i=0;i<100;i++){
		if(fechayhora[i]=='\0'){
			lenghtBuffer=i;
			break;
		}
	}
				
	apuntadorAArchivoBinario=fopen("serverDogs.dat","r+b");
	if(apuntadorAArchivoBinario==NULL){
		perror("Error en buscar: fopen(serverDogs.dat) en modo r+b");
		exit(-1);
	}
				
	fwrite(&fechayhora,sizeof(fechayhora),1,apuntadorAArchivoBinario);
	fwrite(&socketCliente.sin_addr.s_addr,sizeof(socketCliente.sin_addr.s_addr),1,apuntadorAArchivoBinario);
	fwrite("busqueda",7*sizeof(char),1,apuntadorAArchivoBinario);
	fwrite(&nombreIngresado,sizeof(nombreIngresado),1,apuntadorAArchivoBinario);
	printf("Cliente %i acabo funcion buscar . . .\n\n",clienteDesc);
	fclose(apuntadorAArchivoBinario);
}

int main(){
	clear();
	printf("Ejecutando servidor . . .\n\n");

	//Implementacion servidor	
	int servidorDesc, clienteDesc,opt = 1,returned;
	struct sockaddr_in socketServidor, socketCliente;
	socklen_t tamano = 0;
	if((servidorDesc=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Error en main: socket()");
		exit(-1);
	}

	socketServidor.sin_family=AF_INET;
	socketServidor.sin_port = htons(PUERTO);
	socketServidor.sin_addr.s_addr = INADDR_ANY;
	bzero(socketServidor.sin_zero, 8);

	setsockopt(servidorDesc,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));
	if((returned=bind(servidorDesc, (struct sockaddr *)&socketServidor, sizeof(struct sockaddr)))==-1){
		perror("Error en main: bind()");
		exit(-1);
	}
	
	if((returned=listen(servidorDesc,NOCLIENTES))==-1){
		perror("Error en main: listen()");
		exit(-1);		
	}

	if((clienteDesc=accept(servidorDesc, (struct sockaddr *)&socketCliente, &tamano))==-1){
		perror("Error en main: accept()");
		exit(-1);	
	}
	printf("Conectado con cliente %i \n\n",clienteDesc);

	//Manipulacion del archivo dataDogs.dat
	FILE *apuntadorAArchivoBinario;
        apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
	if(apuntadorAArchivoBinario==NULL){
		generarArchivoAleatorio();
		apuntadorAArchivoBinario=fopen("dataDogs.dat","rb");
	}else{
		printf("Ya habia un archivo aleatorio, no fue necesario crear uno nuevo . . .\n\n");
	}
	fclose(apuntadorAArchivoBinario);

	//Manipulacion del archivo serverDogs.dat
        apuntadorAArchivoBinario=fopen("serverDogs.dat","rb");
	if(apuntadorAArchivoBinario==NULL){
		printf("CREANDO serverDogs.dat . . .\n\n");
		apuntadorAArchivoBinario=fopen("serverDogs.dat","wb");
		printf("Creado serverDogs.dat\n\n");
	}else{
		printf("Ya habia un archivo serverDogs.dat, no fue necesario crear uno nuevo . . .\n\n");
	}
	fclose(apuntadorAArchivoBinario);

	//Inicio ejecucion
	char buffer[128];
	bool repetir=true;
	while(repetir){
		printf("Cliente %i esta en el menu principal . . .\n\n",clienteDesc);
		returned=recv(clienteDesc,buffer,128,0);
		if(returned==-1){
				perror("Error en main: recv(buffer)");
				exit(-1);	
		}
		switch(buffer[0]){
			case '1':
				ingresar(clienteDesc,buffer,socketCliente);
			break;
			
			case '2':
				ver(clienteDesc,buffer,socketCliente);
			break;
			
			case '3':
				
				borrar(clienteDesc,buffer,socketCliente);
			break;
			
			case '4':
				
				buscar(clienteDesc,buffer,socketCliente);
			break;
			
			case '5':
				repetir=false;
			break;
		}
	}
	printf("Cliente %i decidio acabar ejecucion . . .\n\n",clienteDesc);
	close(servidorDesc);
	return 0;
}
