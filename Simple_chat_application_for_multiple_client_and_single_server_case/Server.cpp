#include <bits/stdc++.h>
#include <Winsock2.h>

//g++ server.cpp -o t.exe -lwsock32
using namespace std;
//Server side
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
	
	//Structure
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int siz_adr = sizeof(server_addr);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(8000);
	
	//socket making
	SOCKET sockt = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockt==INVALID_SOCKET){
		cout<<"SERVER SOCKET making Failed\n";
	}else{
		cout<<"SERVER SOCKET making Success\n";
	}
	
	//avoid bind error
	BOOL opt = TRUE;
	setsockopt(sockt,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt));
	
	//binding
	int binding = bind(sockt,(SOCKADDR*)&server_addr,siz_adr);
	if(binding==SOCKET_ERROR){
		cout<<"BINDING SERVER SOCKET Failed\n";
	}else{
		cout<<"BINDING SERVER SOCKET Success\n";
	}
	
	//listening
	int listening = listen(sockt,10);
	if(listening==SOCKET_ERROR){
		cout<<"LISTENING SERVER SOCKET Failed\n";
	}else{
		cout<<"LISTENING SERVER SOCKET Success\n";
	}
	
	fd_set read_fd;
	FD_ZERO(&read_fd);
	FD_SET(sockt,&read_fd);
	vector<SOCKET>client_record(10);
	int ff = 0,ff1;
	struct timeval t;
	
	map<string,SOCKET> registered_user;
	set<SOCKET> sender_socket;
	map<string,SOCKET> sender_user;
	set<SOCKET> reciever_socket;
	map<string,SOCKET>::iterator it;
	set<SOCKET>::iterator it1;
	//accepting
	while(1){
		fd_set copy_fd = read_fd;
		int skt_cnt = select(0,&copy_fd,NULL,NULL,NULL);
		for(int i=0;i<skt_cnt;i++){
			SOCKET acc = copy_fd.fd_array[i];
			if(acc==sockt){
				SOCKET client1 = accept(sockt,NULL,NULL);
				if(client1 ==INVALID_SOCKET){
					cout<<"DECLINE the Connecting request from client with SOCKET id : "<<client1<<"\n";
				}else{
					cout<<"ACCEPT the Connecting request from client with SOCKET id : "<<client1<<"\n";
				}
				FD_SET(client1,&read_fd);
			}else{
				char user_recieve[4096];
				ZeroMemory(user_recieve,4096);
				int recin = recv(acc,user_recieve,4096,0);
				if(recin<=0||(strcmp(user_recieve,"Exit")==0)){
					sender_socket.erase(sender_socket.find(acc));
					string sou = "";
					for(it = sender_user.begin();it!=sender_user.end();it++){
						if(it->second==acc){
							sou = it->first;
							break;
						}
					}
					sender_user.erase(sou);
					if(registered_user.find(sou)!=registered_user.end()){
						char user_send[4096] = "Exit";
						int user_len = strlen(user_send) + 1;
						int sending = send(registered_user[sou],user_send,user_len,0);
						if(sending==SOCKET_ERROR){
							cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
						}
						reciever_socket.erase(reciever_socket.find(acc));
						registered_user.erase(sou);
					}
					closesocket(acc);
					FD_CLR(acc,&read_fd);
				}else{
					string cmnd = "";
					for(int i=0;i<4;i++){
						cmnd +=user_recieve[i];
					}
					if(cmnd.compare("SEND")==0){ 
						if(sender_socket.find(acc)==sender_socket.end()){
							char user_send[4096] = "ERROR 101 No user registered";
							int user_len = strlen(user_send) + 1;
							int sending = send(acc,user_send,user_len,0);
							if(sending==SOCKET_ERROR){
								cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
							}
						}else{
							cout<<"Request form client with socket id: "<<acc<<" to forward message."<<"\n";
							string recp_n = "";
							int k = 5,d = 0;
							while(user_recieve[k]!='$'){
								if((user_recieve[k]>='0'&&user_recieve[k]<='9')||(user_recieve[k]>='a'&&user_recieve[k]<='z')||(user_recieve[k]>='A'&&user_recieve[k]<='Z')){
									recp_n+=user_recieve[k];
								}else{
									d = 1;
									break;
								}
								k++;
							}
							if(d==1){
								char user_send[4096] = "ERROR 100 Malformed username";
								int user_len = strlen(user_send) + 1;
								int sending = send(acc,user_send,user_len,0);
								if(sending==SOCKET_ERROR){
									cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
								}
							}else{
								string lenn = "";
								k++;
								while(user_recieve[k]!='$'){
									lenn+=user_recieve[k];
									k++;
								}
								string msg = "";
								k++;
								int fds = k + stoi(lenn);
								int f = strlen(user_recieve);
								fds = min(fds,f);
								for(;k<fds;k++){
									msg+=user_recieve[k];
								}
								if(recp_n=="ALL"){
									string uss;
									int cv = 0;
									for(it = sender_user.begin();it!=sender_user.end();it++){
										if(it->second==acc){
											uss = it->first;
											break;
										}
									}
									SOCKET rama;
									if(registered_user.find(uss)!=registered_user.end()){
										rama = registered_user[uss];
									}
									for(int i=0;i<read_fd.fd_count;i++){
										SOCKET faltu = read_fd.fd_array[i];
										if(faltu!=sockt&&faltu!=acc&&faltu!=rama){
											if(reciever_socket.find(faltu)!=reciever_socket.end()){
												char send_msg[4096];
												string frwd_msg = "FORWARD "+ uss +"$"+lenn+"$"+msg;
												strcpy(send_msg,frwd_msg.c_str());
												int us_lw = strlen(send_msg) + 1;
												int send_rec = send(faltu,send_msg,us_lw,0);
												if(send_rec==SOCKET_ERROR){													
													cout<<"Sending message to client with SOCKET id : "<<faltu<<" is Failed\n";
													char user_send[4096] = "ERROR 102 Unable to send";
													int user_len = strlen(user_send) + 1;
													int sending = send(acc,user_send,user_len,0);
													if(sending==SOCKET_ERROR){
														cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
													}
													cv = 1;
													break;
												}else{
													char recv_msg[4096];
													int recv_rec = recv(faltu,recv_msg,4096,0);
													if(recv_rec==SOCKET_ERROR){														
														cout<<"Recieving message from client with SOCKET id : "<<faltu<<" is Failed\n";
														char user_send[4096] = "ERROR 102 Unable to send";
														int user_len = strlen(user_send) + 1;
														int sending = send(acc,user_send,user_len,0);
														if(sending==SOCKET_ERROR){
															cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
														}
														cv = 1;
														break;
													}else{
														string dfg = "";
														dfg+=recv_msg[0]; dfg+=recv_msg[1]; dfg+=recv_msg[2]; dfg+=recv_msg[3]; 
														dfg+=recv_msg[4]; dfg+=recv_msg[5]; dfg+=recv_msg[6]; dfg+=recv_msg[7];
														if(dfg.compare("RECIEVED")!=0){
															char user_send[4096] = "ERROR 103 Header incomplete";
															int user_len = strlen(user_send) + 1;
															int sending = send(acc,user_send,user_len,0);
															if(sending==SOCKET_ERROR){
																cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
															}
															FD_CLR(acc,&read_fd);
															sender_socket.erase(sender_socket.find(acc));
															string sou = "";
															for(it = sender_user.begin();it!=sender_user.end();it++){
																if(it->second==acc){
																	sou = it->first;
																	break;
																}
															}
															sender_user.erase(sou);
															if(reciever_socket.find(acc)!=reciever_socket.end()){
																reciever_socket.erase(reciever_socket.find(acc));
																registered_user.erase(sou);
															}
															cv = 1;
															break;
														}
													}
												}
											}
										}
									}
									if(cv==0){
										char user_send[4096];
										string xyz = "SENT " +recp_n;
										strcpy(user_send,xyz.c_str());
										int user_len = strlen(user_send) + 1;
										int sending = send(acc,user_send,user_len,0);
										if(sending==SOCKET_ERROR){
											cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
										}
									}
								}else if(registered_user.find(recp_n)!=registered_user.end()){
									string username;
									for(it = sender_user.begin();it!=sender_user.end();it++){
										if(it->second==acc){
											username = it->first;
											break;
										}
									}
									char send_msg[4096];
									msg = "FORWARD "+ username +"$"+lenn+"$"+msg;
									strcpy(send_msg,msg.c_str());
									int us_lw = strlen(send_msg) + 1;
									int send_rec = send(registered_user[recp_n],send_msg,us_lw,0);
									if(send_rec==SOCKET_ERROR){
										cout<<"Sending message to client with SOCKET id : "<<registered_user[recp_n]<<" is Failed\n";
										char user_send[4096] = "ERROR 102 Unable to send";
										int user_len = strlen(user_send) + 1;
										int sending = send(acc,user_send,user_len,0);
										if(sending==SOCKET_ERROR){
											cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
										}
									}else{
										char recv_msg[4096];
										int recv_rec = recv(registered_user[recp_n],recv_msg,4096,0);
										if(recv_rec==SOCKET_ERROR){
											cout<<"Recieving message from client with SOCKET id : "<<registered_user[recp_n]<<" is Failed\n";
											char user_send[4096] = "ERROR 102 Unable to send";
											int user_len = strlen(user_send) + 1;
											int sending = send(acc,user_send,user_len,0);
											if(sending==SOCKET_ERROR){
												cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
											}
										}else{
											string dfg = "";
											dfg+=recv_msg[0]; dfg+=recv_msg[1]; dfg+=recv_msg[2]; dfg+=recv_msg[3]; 
											dfg+=recv_msg[4]; dfg+=recv_msg[5]; dfg+=recv_msg[6]; dfg+=recv_msg[7];
											if(dfg.compare("RECIEVED")==0){
												char user_send[4096];
												string xyz = "SENT " +recp_n;
												strcpy(user_send,xyz.c_str());
												int user_len = strlen(user_send) + 1;
												int sending = send(acc,user_send,user_len,0);
												if(sending==SOCKET_ERROR){
													cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
												}
											}else{
												char user_send[4096] = "ERROR 103 Header incomplete";
												int user_len = strlen(user_send) + 1;
												int sending = send(acc,user_send,user_len,0);
												if(sending==SOCKET_ERROR){
													cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
												}
												FD_CLR(acc,&read_fd);
												sender_socket.erase(sender_socket.find(acc));
												string sou = "";
												for(it = sender_user.begin();it!=sender_user.end();it++){
													if(it->second==acc){
														sou = it->first;
														break;
													}
												}
												sender_user.erase(sou);
												if(reciever_socket.find(acc)!=reciever_socket.end()){
													reciever_socket.erase(reciever_socket.find(acc));
													registered_user.erase(sou);
												}
											}
										}
									}
								}else{
									char user_send[4096] = "ERROR 102 Unable to send";
									int user_len = strlen(user_send) + 1;
									int sending = send(acc,user_send,user_len,0);
									if(sending==SOCKET_ERROR){
										cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
									}
								}
							}
						}
					}else{
						for(int i=4;i<8;i++){
							cmnd+=user_recieve[i];
						}
						if(cmnd.compare("REGISTER")==0){
							cmnd = "";
							for(int i=9;i<15;i++){
								cmnd+=user_recieve[i];
							}
							if(cmnd.compare("TOSEND")==0){
								cout<<"REGISTRATION request for sending messages from socket id: "<<acc<<"\n";
								string username;
								int d = 0;
								for(int i=16;i<strlen(user_recieve);i++){
									if((user_recieve[i]>='0'&&user_recieve[i]<='9')||(user_recieve[i]>='a'&&user_recieve[i]<='z')||(user_recieve[i]>='A'&&user_recieve[i]<='Z')){
										username+=user_recieve[i];
									}else{
										d = 1;
										break;
									}
								}
								if(d==1){
									char user_send[4096] = "ERROR 100 Malformed username";
									int user_len = strlen(user_send) + 1;
									int sending = send(acc,user_send,user_len,0);
									if(sending==SOCKET_ERROR){
										cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
									}
								}else{
									sender_user[username] = acc;
									sender_socket.insert(acc);
									char user_send[4096];
									string s = "REGISTERED TOSEND " + username;
									strcpy(user_send,s.c_str());
									int user_len = strlen(user_send) + 1;
									int sending = send(acc,user_send,user_len,0);
									if(sending==SOCKET_ERROR){
										cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
									}
								}
							}else if(cmnd.compare("TORECV")==0){
								cout<<"REGISTRATION request for sending messages fromo socket id: "<<acc<<"\n";
								string username;
								int d = 0;
								for(int i=16;i<strlen(user_recieve);i++){
									if((user_recieve[i]>='0'&&user_recieve[i]<='9')||(user_recieve[i]>='a'&&user_recieve[i]<='z')||(user_recieve[i]>='A'&&user_recieve[i]<='Z')){
										username+=user_recieve[i];
									}else{
										d = 1;
										break;
									}
								}
								if(d==1){
									char user_send[4096] = "ERROR 100 Malformed username";
									int user_len = strlen(user_send) + 1;
									int sending = send(acc,user_send,user_len,0);
									if(sending==SOCKET_ERROR){
										cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
									}
								}else{
									registered_user[username] = acc;
									reciever_socket.insert(acc);
									char user_send[4096];
									string s = "REGISTERED TORECV " + username;
									strcpy(user_send,s.c_str());
									int user_len = strlen(user_send) + 1;
									int sending = send(acc,user_send,user_len,0);
									if(sending==SOCKET_ERROR){
										cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
									}
								}
							}else{
								string username;
								int d = 0;
								for(int i=16;i<strlen(user_recieve);i++){
									if((user_recieve[i]>='0'&&user_recieve[i]<='9')||(user_recieve[i]>='a'&&user_recieve[i]<='z')||(user_recieve[i]>='A'&&user_recieve[i]<='Z')){
										username+=user_recieve[i];
									}else{
										d = 1;
										break;
									}
								}
								char user_send[4096] = "ERROR : No Command Found";
								int user_len = strlen(user_send) + 1;
								int sending = send(acc,user_send,user_len,0);
								if(sending==SOCKET_ERROR){
									cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
								}
							}
						}else{
							//sending 
							char user_send[4096] = "ERROR : No Command Found";
							int user_len = strlen(user_send) + 1;
							int sending = send(acc,user_send,user_len,0);
							if(sending==SOCKET_ERROR){
								cout<<"Sending message to client with SOCKET id : "<<acc<<" is Failed\n";
							}
						}
					}
				}
			}
		}
	}
	//closing
	int closing = closesocket(sockt);
	if(closing==SOCKET_ERROR){
		cout<<"Closing of SERVER SOCKET Failed\n";
	}else{
		cout<<"Closing of SERVER SOCKET Success\n";
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