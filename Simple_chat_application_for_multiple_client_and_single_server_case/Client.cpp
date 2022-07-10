#include <bits/stdc++.h>
#include <Winsock2.h>
#include <thread>
//g++ Client.cpp -o t.exe -lwsock32
using namespace std;

//Client side
string myname;
int ext;
void sendd(SOCKET sockt_send){
	while(true){
		//sending
		char user_send[4096] ;
		cin.getline(user_send,4096);
		if(user_send[0]=='@'){
			string tosend = "SEND ";
			string tof = "";
			int g = 1;
			while(user_send[g]!=':'){
				tosend+=user_send[g];
				tof+=user_send[g];
				g++;
			}
			g++;
			string msg = "";
			for(;g<strlen(user_send);g++){
				msg+=user_send[g];
			}
			int len = msg.length();
			tosend+='$';
			tosend+=to_string(len);
			tosend+='$';
			tosend+=msg;
			strcpy(user_send,tosend.c_str());
			int user_len = strlen(user_send) + 1;
			int sending = send(sockt_send,user_send,user_len,0);
			if(sending==SOCKET_ERROR){
				cout<<"SENDING message Failed to Server\n";
			}
				
			//recieving
			char user_recieve[4096];
			int user_siz = strlen(user_recieve) + 1;	
			int recieving = recv(sockt_send,user_recieve,4096,0);
			if(recieving==SOCKET_ERROR){
				cout<<"Conformation for message sent succesfully Failed\n";
			}else{
				if(strcmp(user_recieve,"ERROR 103 Header incomplete")==0){
					cout<<user_recieve<<"\n";
					ext = 1;
					return ;
				}else{
					cout<<user_recieve<<"\n";
				}
			}
		}else{
			int user_len = strlen(user_send) + 1;
			int sending = send(sockt_send,user_send,user_len,0);
			if(sending==SOCKET_ERROR){
				cout<<"SENDING message Failed to Server\n";
			}
			//recieving
			if(strcmp(user_send,"Exit")==0){
				ext = 1;
				return ;
			}
			char user_recieve[4096];
			int user_siz = strlen(user_recieve) + 1;	
			int recieving = recv(sockt_send,user_recieve,4096,0);
			if(recieving==SOCKET_ERROR){
				cout<<"Conformation for message sent succesfully Failed\n";
			}else{
				cout<<user_recieve<<"\n";
			}
		}
	}
}

void send_reg(SOCKET sockt_send,string ac,string usname){
	char user_send[4096];
	if(ac.compare("TOSEND")==0){
		usname = "REGISTER TOSEND " + usname;
		strcpy(user_send,usname.c_str());
		int lez = strlen(user_send) + 1;
		int sed = send(sockt_send,user_send,lez,0);
		if(sed==SOCKET_ERROR){
			cout<<"Request for REGISTRATION of SENDING SOCKET FAILED\n";
		}
		//recieving
		char user_recieve[4096];
		int user_siz = strlen(user_recieve) + 1;	
		int recieving = recv(sockt_send,user_recieve,4096,0);
		if(recieving==SOCKET_ERROR){
			cout<<"Conformation for REGISTRATION of SENDING SOCKET FAILED\n";
		}else{
			string cmp = "REGISTERED TOSEND " + myname;
			string monu = user_recieve;
			if(monu.compare(cmp)==0){
				cout<<"SENDING SOCKET REGISTERED Successfully\n";
			}else{
				cout<<user_recieve<<"\n";
			}
		}
	}else if(ac.compare("TORECV")==0){
		usname = "REGISTER TORECV " + usname;
		strcpy(user_send,usname.c_str());
		int lez = strlen(user_send) + 1;
		int sed = send(sockt_send,user_send,lez,0);
		if(sed==SOCKET_ERROR){
			cout<<"Request for REGISTRATION of RECIEVING SOCKET FAILED\n";
		}
		//recieving
		char user_recieve[4096];
		int user_siz = strlen(user_recieve) + 1;	
		int recieving = recv(sockt_send,user_recieve,4096,0);
		if(recieving==SOCKET_ERROR){
			cout<<"Conformation for REGISTRATION of RECIEVING SOCKET FAILED\n";
		}else{
			string cmp = "REGISTERED TORECV " + myname;
			string monu = user_recieve;
			if(monu.compare(cmp)==0){
				cout<<"RECIEVING SOCKET REGISTERED Successfully\n";
			}else{
				cout<<user_recieve<<"\n";
			}
		}
	}
}

void recvv(SOCKET sockt_recieve){
	if(ext==1){
		return;
	}
	while(true){
		char user_recieve[4096];
		int user_siz = strlen(user_recieve) + 1;	
		int recieving = recv(sockt_recieve,user_recieve,4096,0);
		if(recieving==SOCKET_ERROR){
			cout<<"RECIEVING Failed for FORWARD messages\n";
			break;
		}else if(strcmp(user_recieve,"Exit")==0){
			break;
		}else{
			string frwd = "";
			int i = 8;
			while(user_recieve[i]!='$'){
				frwd+=user_recieve[i];
				i++;
			}
			i++;
			string len = "";
			while(user_recieve[i]!='$'){
				len+=user_recieve[i];
				i++;
			}
			i++;
			if(len.length()==0){
				char user_send[4096] = "ERROR 103 Header incomplete";
				int use_siz = strlen(user_send) + 1;
				int semding = send(sockt_recieve,user_send,use_siz,0);
				if(semding==SOCKET_ERROR){
					cout<<"SENDING message Failed to Server\n";
				}
			}else{
				string rcvvvv = "";
				for(;i<strlen(user_recieve);i++){
					rcvvvv+=user_recieve[i];
				}
				cout<<"@"<<frwd<<":"<<rcvvvv<<"\n";
				char user_send[4096];
				string faltu = "RECIEVED" + myname;
				strcpy(user_send,faltu.c_str());
				int use_siz = strlen(user_send) + 1;
				int semding = send(sockt_recieve,user_send,use_siz,0);
				if(semding==SOCKET_ERROR){
					cout<<"SENDING message Failed to Server\n";
				}
			}
		}
	}
}

int main()
{
	//WSAStartup
	WSADATA winsockdata;
	int start_wsa;
	int end_wsa;
	
	start_wsa = WSAStartup(MAKEWORD(2,2),&winsockdata);
	
	if(start_wsa!=0){
		cout<<"WSAStartup Failed\n";
	}else{
		cout<<"WSAStartup Success\n";
	}
	
	//socket sending
	SOCKET sockt_send = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockt_send==INVALID_SOCKET){
		cout<<"SENDING SOCKET making Failed\n";
	}else{
		cout<<"SENDING SOCKET making Success\n";
	}
	
	//socket recieving
	SOCKET sockt_recieve = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockt_recieve==INVALID_SOCKET){
		cout<<"RECIEVING SOCKET making Failed\n";
	}else{
		cout<<"RECIEVING SOCKET making Success\n";
	}
	
	//Structure
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int siz_adr = sizeof(server_addr);
	
	cout<<"Enter Username : ";
	string us_name; getline(cin,us_name);
	myname = us_name;
	cout<<"Enter Server's IP Address : ";
	//char* ip_addr; cin>>ip_addr;
	string gh; getline(cin,gh);
	if(gh!="127.0.0.1"){
		cout<<"Address Not found\n";
		return 0;
	}
	server_addr.sin_family = AF_INET;
	
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	server_addr.sin_port = htons(8000);
	
	//connecting sending
	int connecting = connect(sockt_send,(SOCKADDR*)&server_addr,siz_adr);
	if(connecting==SOCKET_ERROR){
		cout<<"Connection to Server for SENDING messages Failed\n";
	}else{
		cout<<"Connection to Server for SENDING messages Success\n";
	}
	
	//connecting recieveing
	int connectig = connect(sockt_recieve,(SOCKADDR*)&server_addr,siz_adr);
	if(connectig==SOCKET_ERROR){
		cout<<"Connection to Server for SENDING messages Failed\n";
	}else{
		cout<<"Connection to Server for SENDING messages Success\n";
	}
	
	int iii =0;
	int regis_on_recv = 0;
	bool f = true;
	
	cout<<"Request for Registration of Sending and Recievinig SOCKTES\n";
	send_reg(sockt_send,"TOSEND",us_name);
	send_reg(sockt_recieve,"TORECV",us_name);
	
	thread t1(sendd,sockt_send);
	thread t2(recvv,sockt_recieve);
	
	t1.join();
	t2.join();
	
	
	//closing sending
	int closing = closesocket(sockt_send);
	if(closing==SOCKET_ERROR){
		cout<<"Closing of SENDING SOCKET Failed\n";
	}else{
		cout<<"Closing of SENDING SOCKET Success\n";
	}
	
	//closing recieveing
	int closin = closesocket(sockt_recieve);
	if(closin==SOCKET_ERROR){
		cout<<"Closing of RECIEVING SOCKET Failed\n";
	}else{
		cout<<"Closing of RECIEVING SOCKET Success\n";
	}
	
	//DLL_clean
	int DLL_clean = WSACleanup();
	if(DLL_clean==SOCKET_ERROR){
		cout<<"DLL_clean Failed"<<WSAGetLastError()<<"\n";
	}else{
		cout<<"DLL_clean Success\n";
	}
	
	system("PAUSE");
	return 0;   
}