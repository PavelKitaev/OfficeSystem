#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <time.h>
#pragma warning(disable: 4996)

#define MAX_CLIENTS 100

SOCKET Connections[MAX_CLIENTS]; //������ ���� �������
std::map<int, std::string> clients; //������ � ������ ��������
std::map<int, std::string> securityClients;	//������ � ������ ��������-����������
std::string dataTime;	//���� � ����� ������� �������
int count = 0;	//���������� �������� ��������

std::string GetDataAfterSeparator(int i, char* _msg) //��������� ������ ����� �����������
{
	std::string data;

	while (_msg[i] != '\0')
	{
		data += _msg[i];
		i++;
	}
	return data;
}

int WriteClient(int _index, std::string _name) //����������� �������
{
	//���������� 0 � ��� ������, ���� ����� ����� ��� ���� � �������
	//���������� 1, ���� ����� ��� ������� �������� � ���

	int res = 1;

	std::map<int, std::string> ::iterator it;
	it = clients.find(_index);

	if (it != clients.end())
	{
		res = 0;
	}

	if (res == 1)
	{
		int msg_size = 16;
	  clients.insert(make_pair(_index, _name));

		send(Connections[_index], (char*)&msg_size, sizeof(int), NULL);
		send(Connections[_index], "CONNECT_SUCCESS", msg_size, NULL);
	}
	else
	{
		int msg_size = 14;

		send(Connections[_index], (char*)&msg_size, sizeof(int), NULL);
		send(Connections[_index], "LOGIN_ALREADY", msg_size, NULL);
	}

	return res;
}

void DisconnectClient(int _index) //���������� �������
{
	std::map<int, std::string> ::iterator it1; //�������� ��� ������� �� ���
	it1 = clients.find(_index); //����� �� �������

	if (it1 != clients.end()) //���� �����
	{
		clients.erase(it1); //�������
	}

	std::map<int, std::string> ::iterator it2;
	it2 = securityClients.find(_index);

	if (it2 != securityClients.end())
	{
		securityClients.erase(it2);
	}
	
	count--; //��������� ���������� ������� �������������

	closesocket(_index); //��������� �����
	Connections[_index] = 0; //�������� ����������� ��� ���������� �������������
}

std::string GetUsersOnline() //��������� ������-��������
{
	std::string res_string;
	
	for (int i = 0; i < clients.size(); i++) //�������� �� ����� ���� � ���������� ��������
	{
		if (clients[i] != "")
		{
			res_string += clients[i];
			res_string += ":";
		}
	}

	return res_string;
}

void SafetyCommand(char* _msg) //��������� ������� "Safety"
{
	std::string msg_string = _msg;
	const char* msg = msg_string.data();
	int size = strlen(msg);

	//�������� �� ����� ���� � ��������� ������ ��������
	std::map <int, std::string> ::iterator it;
	it = securityClients.begin();
	for (int j = 0; j < securityClients.size(); j++)
	{
		int tt = it->first;
		send(Connections[tt], (char*)&size, sizeof(int), NULL);
		send(Connections[tt], msg, size, NULL);
		it++;
	}
}

void GetSensorsDataConnamd() //������� ������� ������ � ����������������
{
	int index = -1;
	for (int j = 0; j < clients.size(); j++)
	{
		if (clients[j] == "SafetySystem") //����� ���������������� �� �������
		{
			index = j;
			break;
		}
	}

	if (index != -1) //���� �����
	{
		char request[5] = "GIVE";
		int sizeR = 5;

		send(Connections[index], (char*)&sizeR, sizeof(int), NULL);
		send(Connections[index], request, sizeR, NULL);
	}
}

int ParsingMessage(char* _msg, int _index) //������ ����������� ���������

{
	int res = 1;
	std::string command;

	//�������� ��������� �����(�������) �� ������ ���������
	int i = 0;
	while (_msg[i] != '\0')
	{
		if (_msg[i] == ':')
		{
			i = i + 2;
			break;
		}
		command += _msg[i];
		i++;
	}
	
	//�������, ����� ������� � ��� �� ����������
	if (command == "SAFETY") //������� �� ���������������� �� �������� ������ ��������-����������
	{
		SafetyCommand(_msg);
	}
	if (command == "NEW_CLIENT") //������� ����������� ������ �������
	{
		std::string name = GetDataAfterSeparator(i, _msg);
		int t = WriteClient(_index, name);

		if (t == 1)
		{
			std::cout << "Client connected: " << name << "\n";
		}
	}
	
	if (command == "DISCONNECTING_CLIENT") //������� ���������� �������
	{
		std::string name = GetDataAfterSeparator(i, _msg);
		DisconnectClient(_index);

		std::cout << "Client disconnected: " << name << "\n";
		res = 2;
	}


	if (command == "GET_USERS_ONLINE") //������� ������� ������������� ������
	{
		std::string msg_string = "USERS_ONLINE::" + GetUsersOnline();
		const char* msg = msg_string.data();
		int size = strlen(msg);
		
		send(Connections[_index], (char*)&size, sizeof(int), NULL);
		send(Connections[_index], msg, size, NULL);
	}

	if (command == "MSG") //������� �� �������� ���������
	{
		std::string fritter = GetDataAfterSeparator(i, _msg); //������� ������� �� ���
		std::string login_dst; //����������
		std::string login_srs; //�����������
		i = 0;
		//���������� ����� � �����_��� � ��������� ������� ����� �����������
		while (fritter[i] != '\0')
		{
			if (fritter[i] == ':')
			{
				i = i + 2;
				while (fritter[i] != '\0')
				{
					if (fritter[i] == ':')
					{
						i = i + 2;
						break;
					}
					login_dst += fritter[i];
					i++;
				}
				break;
			}

			login_srs += fritter[i];
			i++;
		}

		//���������� ��� � ������� ������. ����������� �����o� �� ����� ������� ����������. �� ������������ ���, ��� � �����?
		int index_dst = -1;
		std::map <int, std::string> ::iterator it, it2;
		
		for (int j = 0; j < clients.size(); j++)
		{
			it = clients.find(j);
			if (it->second == login_dst)
			{
				index_dst = j;
				break;
			}
		}

		//���� ����� - ���������� ���������
		if (index_dst != -1)
		{
			std::string msg_string = _msg;
			const char* msg = msg_string.data();
			int size = strlen(msg);

			send(Connections[index_dst], (char*)&size, sizeof(int), NULL);
			send(Connections[index_dst], msg, size, NULL);
		}
	}

	if (command == "SERVER_DATATIME") //������� ��������� ���� � ������� ������� �������  
	{
		std::string msg_string = "SERVER_DATATIME::" + dataTime;
		const char* msg = msg_string.data();
		int size = strlen(msg);

		send(Connections[_index], (char*)&size, sizeof(int), NULL);
		send(Connections[_index], msg, size, NULL);
	}

	if (command == "GET_SENSORS_DATA") //������� ������� ������ � ����������������
	{
		GetSensorsDataConnamd();
	}

	if (command == "SECURITY_CLIENT") //����������� �������-���������
	{
		std::string login = GetDataAfterSeparator(i, _msg);
		securityClients.insert(make_pair(_index, login));
	}
	return res;
}

DWORD WINAPI ClientHandler(LPVOID index) //����� ��������� �� �������
{
	int ind = *static_cast<const int*>(index);
	int msg_size = 0;
	int cc = 0;
	while (true) 
	{
		//setlocale(LC_ALL, "Russian");
		if (recv(Connections[ind], (char*)&msg_size, sizeof(int), NULL) > 0)
		{
			char* msg = new char[msg_size + 1];
			msg[msg_size] = '\0';

			if (recv(Connections[ind], msg, msg_size, NULL) > 0)
			{
				//std::cout << msg << std::endl;
				int temp = ParsingMessage(msg, ind);

				if (temp == 2)
				{
					return 0;
				}

				//�������� ��������
				//for (int i = 0; i < count; i++) {
				//	if (i == ind)
				//		continue;
				//
				//	send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
				//	send(Connections[i], msg, msg_size, NULL);
				//}
				delete[] msg;
			}
		}
		else
		{
			DisconnectClient(ind);
			std::cout << "Disconnecting a Lost Client." << std::endl;
			return 0;
		}
	}
}

std::string GetTimeServer()	//��������� ���� � ������� ������� �������
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[20];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%d.%m.%Y %X", &tstruct);

	return buf;
}

int main(int argc, char* argv[]) 
{
	setlocale(LC_ALL, "Russian");
	std::cout << "OfficeSystem[server] Maximum connections - " << MAX_CLIENTS << std::endl;

	//����������� ip �����
	char* ip = new char[16];
	std::cout << "Enter the ip-address: ";
	std::cin >> ip;

	dataTime = GetTimeServer(); //�������� ���� � ����� �������

	DWORD dwThreadId, dwThrdParam;
	HANDLE hThreads[MAX_CLIENTS];

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);

	//�������������� ���������� �������
	if (WSAStartup(DLLVersion, &wsaData) != 0) 
	{
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ip);
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	std::cout << "Server started" << std::endl;

	while (true) //���� �� �������� � ��������� �����������
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr); //�������� �����������

		if (count != MAX_CLIENTS) //���� ����� ���� - ������������
		{
			for (int i = 0; i < MAX_CLIENTS; i++) //���� ������� �����������
			{
				if (newConnection == 0)  //���� ����� ����������� ������� � �������
				{
					std::cout << "Error #2\n";
				}
				else //���� ��� ������
				{
					if (Connections[i] == 0) //���� ��������� ����� � ������� �������
					{
						Connections[i] = newConnection; //����������
						count++; //����������� ����� �������� ��������
						dwThrdParam = i;
						hThreads[i] = CreateThread(NULL, 0, ClientHandler, &dwThrdParam, 0, &dwThreadId); //������� ����� �������� ���������
						break; //������� � ����� ������� �����������
					}
				}
			}
		}
		else //������������
		{
			std::cout << "Overflow!";
			system("pause");
		}
	}

	return 0;
}