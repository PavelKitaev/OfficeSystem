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

SOCKET Connections[MAX_CLIENTS]; //Массив всех сокетов
std::map<int, std::string> clients; //идексы и логины клиентов
std::map<int, std::string> securityClients;	//Идексы и логины клиентов-охранников
std::string dataTime;	//Дата и время запуска сервера
int count = 0;	//Количество активных клиентов

std::string GetDataAfterSeparator(int i, char* _msg) //Получение данных после разделителя
{
	std::string data;

	while (_msg[i] != '\0')
	{
		data += _msg[i];
		i++;
	}
	return data;
}

int WriteClient(int _index, std::string _name) //Авторизация клиента
{
	//Возвращает 0 в том случае, если такой логин уже есть в системе
	//Возвращает 1, если логин был успешно добавлен в Мап

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

void DisconnectClient(int _index) //Отключеник клиента
{
	std::map<int, std::string> ::iterator it1; //Итератор для прохома по мап
	it1 = clients.find(_index); //Поиск по индексу

	if (it1 != clients.end()) //Если нашли
	{
		clients.erase(it1); //Удаляем
	}

	std::map<int, std::string> ::iterator it2;
	it2 = securityClients.find(_index);

	if (it2 != securityClients.end())
	{
		securityClients.erase(it2);
	}
	
	count--; //Уменьшаем количество текущий пользователей

	closesocket(_index); //Закрываем сокет
	Connections[_index] = 0; //Зануляем подключение для повторного использования
}

std::string GetUsersOnline() //Поулчение онлайн-клиентов
{
	std::string res_string;
	
	for (int i = 0; i < clients.size(); i++) //Проходим по всему мапу и записываем клиентов
	{
		if (clients[i] != "")
		{
			res_string += clients[i];
			res_string += ":";
		}
	}

	return res_string;
}

void SafetyCommand(char* _msg) //Обработка команды "Safety"
{
	std::string msg_string = _msg;
	const char* msg = msg_string.data();
	int size = strlen(msg);

	//Проходим по всему мапу и рассылаем данные клиентам
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

void GetSensorsDataConnamd() //Команда запроса данных с микроконтроллера
{
	int index = -1;
	for (int j = 0; j < clients.size(); j++)
	{
		if (clients[j] == "SafetySystem") //Логин микроконтроллера на сервере
		{
			index = j;
			break;
		}
	}

	if (index != -1) //Если нашли
	{
		char request[5] = "GIVE";
		int sizeR = 5;

		send(Connections[index], (char*)&sizeR, sizeof(int), NULL);
		send(Connections[index], request, sizeR, NULL);
	}
}

int ParsingMessage(char* _msg, int _index) //Разбор полученного сообщения

{
	int res = 1;
	std::string command;

	//Отделяем служебную часть(команду) от целого сообщения
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
	
	//Смотрим, какая команда и как ее обработать
	if (command == "SAFETY") //Команда от микроконтроллера на отправку данных клиентам-охранникам
	{
		SafetyCommand(_msg);
	}
	if (command == "NEW_CLIENT") //Команда авторизации нового клиента
	{
		std::string name = GetDataAfterSeparator(i, _msg);
		int t = WriteClient(_index, name);

		if (t == 1)
		{
			std::cout << "Client connected: " << name << "\n";
		}
	}
	
	if (command == "DISCONNECTING_CLIENT") //Команда отключения клиента
	{
		std::string name = GetDataAfterSeparator(i, _msg);
		DisconnectClient(_index);

		std::cout << "Client disconnected: " << name << "\n";
		res = 2;
	}


	if (command == "GET_USERS_ONLINE") //Команда запроса пользователей онлайн
	{
		std::string msg_string = "USERS_ONLINE::" + GetUsersOnline();
		const char* msg = msg_string.data();
		int size = strlen(msg);
		
		send(Connections[_index], (char*)&size, sizeof(int), NULL);
		send(Connections[_index], msg, size, NULL);
	}

	if (command == "MSG") //Команда на отправку сообщения
	{
		std::string fritter = GetDataAfterSeparator(i, _msg); //удаляем команду из мсг
		std::string login_dst; //получатель
		std::string login_srs; //Отправитель
		i = 0;
		//Записываем логин в логин_дст и вычисляем позицию после разделителя
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

		//Перебираем мап в поисках логина. Рассмотреть перехoд на более удобные контейнеры. Мб использовать тот, что в лабах?
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

		//Если нашли - отправляем сообщение
		if (index_dst != -1)
		{
			std::string msg_string = _msg;
			const char* msg = msg_string.data();
			int size = strlen(msg);

			send(Connections[index_dst], (char*)&size, sizeof(int), NULL);
			send(Connections[index_dst], msg, size, NULL);
		}
	}

	if (command == "SERVER_DATATIME") //Команда получения даты и времени запуска сервера  
	{
		std::string msg_string = "SERVER_DATATIME::" + dataTime;
		const char* msg = msg_string.data();
		int size = strlen(msg);

		send(Connections[_index], (char*)&size, sizeof(int), NULL);
		send(Connections[_index], msg, size, NULL);
	}

	if (command == "GET_SENSORS_DATA") //Команда запроса данных с микроконтроллера
	{
		GetSensorsDataConnamd();
	}

	if (command == "SECURITY_CLIENT") //Регистрация клиента-охранника
	{
		std::string login = GetDataAfterSeparator(i, _msg);
		securityClients.insert(make_pair(_index, login));
	}
	return res;
}

DWORD WINAPI ClientHandler(LPVOID index) //Прием сообщений от клиента
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

				//Массовая рассылка
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

std::string GetTimeServer()	//Получение даты и времени запуска сервера
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

	//Запрашиваем ip адрес
	char* ip = new char[16];
	std::cout << "Enter the ip-address: ";
	std::cin >> ip;

	dataTime = GetTimeServer(); //Получаем дату и время запуска

	DWORD dwThreadId, dwThrdParam;
	HANDLE hThreads[MAX_CLIENTS];

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);

	//Инициализируем библиотеку сокетов
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

	while (true) //Цикл на ожидание и обработку подключений
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr); //Ожидание подключения

		if (count != MAX_CLIENTS) //Если места есть - обрабатываем
		{
			for (int i = 0; i < MAX_CLIENTS; i++) //Ищем нулевое подключение
			{
				if (newConnection == 0)  //Если новое подключение создано с ошибкой
				{
					std::cout << "Error #2\n";
				}
				else //Если все хорошо
				{
					if (Connections[i] == 0) //Ищем доступное место в массиве сокетов
					{
						Connections[i] = newConnection; //Записываем
						count++; //Увеличиваем число активных клиентов
						dwThrdParam = i;
						hThreads[i] = CreateThread(NULL, 0, ClientHandler, &dwThrdParam, 0, &dwThreadId); //Создаем поток принятия сообщений
						break; //Выходим и вновь ожидаем подключение
					}
				}
			}
		}
		else //Переполнение
		{
			std::cout << "Overflow!";
			system("pause");
		}
	}

	return 0;
}