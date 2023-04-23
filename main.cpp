#include<WinSock2.h>
#include<ws2tcpip.h>
#include<strsafe.h>
#include<vector>
#include<string>
#include<process.h>
#include<iostream>
#include"base64.h"
#include"sha1.h"
#include<vector>
using namespace std;
#pragma comment(lib,"Ws2_32.lib")

string getString = "GET";
char fin_;
char opcode_;
char mask_;
char payload_length_;
char payload_[2048];
char masking_key_[4];
int roomId = 0;

vector<SOCKET>clientArray;
vector<SOCKET>MatchPool;
vector<vector<SOCKET>>Room;

vector<string> splitString(string oriStr, string splitStr)
{
	// 这代码有点傻逼
	vector<string> result;
	string temp = "";
	int begin = 0, end = 0, index = 0;
	for (int i = 0; i < oriStr.size(); i++)
	{
		if (oriStr[i] == splitStr[index])
		{
			index++;
			if (index >= splitStr.size() && i != oriStr.size() - 1)
			{
				index = 0;
				begin = i + 1;
				end = i + 1;
				result.push_back(temp);
				temp = "";
			}
		}
		else
		{
			index = 0;
			for (int j = end; j < i + 1; j++)temp += oriStr[j];
			end = i + 1;
		}
	}
	if (begin < end)
	{
		temp = "";
		for (int i = begin; i < end; i++)
		{
			temp += oriStr[i];
		}
		result.push_back(temp);
	}
	return result;
}

bool isInVertor(vector<SOCKET>& arr, SOCKET s)
{
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr[i] == s)return true;
	}
	return false;
}
bool isInRoom(SOCKET s)
{
	for (int i = 0; i < Room.size(); i++)
	{
		if (isInVertor(Room[i], s))return true;
	}
	return false;
}
void AddInRoom(SOCKET a, SOCKET b)
{
	vector<SOCKET> temp;
	temp.push_back(a);
	temp.push_back(b);
	Room.push_back(temp);
}
void VectorErase(vector<SOCKET>& arr, SOCKET s)
{
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr[i] == s)
		{
			arr.erase(arr.begin() + i);
			break;
		}
	}
}
void RoomErase(SOCKET s)
{
    for(int i=0;i<Room.size();i++)
    {
        VectorErase(Room[i], s);
    }
}
void PutInSet(vector<SOCKET>& arr, SOCKET s)
{
	for (SOCKET i : arr)if (i == s)return;
	arr.push_back(s);
}

void getKey(char* request, string clientkey)
{
    strcat_s(request, 1024, "HTTP/1.1 101 Switching Protocols\r\n");
    strcat_s(request, 1024, "Connection: upgrade\r\n");
    strcat_s(request, 1024, "Sec-WebSocket-Accept:");
    string server_key = clientkey;
    server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    SHA1 sha;
    unsigned int message_digest[5];
    sha.Reset();
    sha << server_key.c_str();
    sha.Result(message_digest);
    for (int i = 0; i < 5; i++)
    {
        message_digest[i] = htonl(message_digest[i]);
    }
    server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
    server_key += "\r\n";
    strcat_s(request, 1024, server_key.c_str());
    strcat_s(request, 1024, "Upgrade:websocket\r\n\r\n");
}

int requestInfo(SOCKET sockClient, char* request)
{
    int result = -1;
    char revData[2048];
    memset(revData, 0, sizeof(revData));
    int ret = recv(sockClient, revData, 2048, 0);
    string revDataString = revData;
    cout << "revDataString" << revDataString << endl;
    string::size_type idx;
    idx = revDataString.find(getString);
    if (idx == string::npos)
    {
        cout << "当前不是握手协议\n";
    }
    else
    {
        cout << "当前是握手协议\n";
        int index = revDataString.find("Sec-WebSocket-Key");
        if (index > 0)
        {
            string secWebSocketKeyString = revDataString.substr(index + 19, 24);
            getKey(request, secWebSocketKeyString);
            result = 1;
        }
        else
        {
            cout << "当前协议不存在 SecWebSocketKey 字段，握手失败" << endl;
        }
    }
    return result;
}

int fetch_fin(char* msg, int& pos) {
    fin_ = (unsigned char)msg[pos] >> 7;
    return 0;
}

int fetch_opcode(char* msg, int& pos) {
    opcode_ = msg[pos] & 0x0f;
    pos++;
    return 0;
}

int fetch_mask(char* msg, int& pos) {
    mask_ = (unsigned char)msg[pos] >> 7;
    return 0;
}

int fetch_masking_key(char* msg, int& pos) {
    if (mask_ != 1)
        return 0;
    for (int i = 0; i < 4; i++)
        masking_key_[i] = msg[pos + i];
    pos += 4;
    return 0;
}

int fetch_payload_length(char* msg, int& pos) {
    payload_length_ = msg[pos] & 0x7f;
    pos++;
    if (payload_length_ == 126) {
        uint16_t length = 0;
        memcpy(&length, msg + pos, 2);
        pos += 2;
        payload_length_ = ntohs(length);
    }
    else if (payload_length_ == 127) {
        uint32_t length = 0;
        memcpy(&length, msg + pos, 4);
        pos += 4;
        payload_length_ = ntohl(length);
    }
    return 0;
}

int fetch_payload(char* msg, int& pos) {
    memset(payload_, 0, sizeof(payload_));
    if (mask_ != 1) {
        memcpy(payload_, msg + pos, payload_length_);
    }
    else {
        for (unsigned int i = 0; i < payload_length_; i++) {
            int j = i % 4;
            payload_[i] = msg[pos + i] ^ masking_key_[j];
        }
    }
    pos += payload_length_;
    return 0;
}

int fetch_websocket_info(char* msg) {
    int pos = 0;
    fetch_fin(msg, pos);
    fetch_opcode(msg, pos);
    fetch_mask(msg, pos);
    fetch_payload_length(msg, pos);
    fetch_masking_key(msg, pos);
    return fetch_payload(msg, pos);
}

void send_content_data(SOCKET sClient, const char* charb)
{
    char buf[1024] = "";
    int first = 0x00;
    int tmp = 0;
    int length = 0;
    length = strlen(charb);
    if (true) {
        first = first + 0x80;
        first = first + 0x1;
    }
    buf[0] = first;
    tmp = 1;
    unsigned int nuNum = (unsigned)length;
    if (length < 126) {
        buf[1] = length;
        tmp = 2;
    }
    else if (length < 65536) {
        buf[1] = 126;
        buf[2] = nuNum >> 8;
        buf[3] = length & 0xFF;
        tmp = 4;
    }
    else {
        //数据长度超过65536
        buf[1] = 127;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        buf[6] = nuNum >> 24;
        buf[7] = nuNum >> 16;
        buf[8] = nuNum >> 8;
        buf[9] = nuNum & 0xFF;
        tmp = 10;
    }

    for (int i = 0; i < length; i++)
    {
        buf[tmp + i] = charb[i];
        printf("要发送的数据字节：%d\n", charb[i]);
    }

    char charbuf[1024] = "";
    memcpy(charbuf, buf, length + tmp);
    send(sClient, charbuf, strlen(charbuf), 0);
}

void WorkThread(SOCKET sockClient)
{
    char request[1024] = "";    // 封装握手响应信息
    char clieninfo[2048] = "";    // 握手后响应信息
    int result = requestInfo(sockClient, request);
    if (result > 0)
    {
        cout << "request:" << request << endl;
        int ret = send(sockClient, request, strlen(request), 0);
        if (ret > 0)
        {
            cout << "握手响应结果ret:" << ret << endl;
            while (true)
            {
                memset(clieninfo, 0, sizeof(clieninfo));
                ret = recv(sockClient, clieninfo, 2048, 0);
                if (ret > 0)
                {
                    fetch_websocket_info(clieninfo);
                    cout << payload_ << endl;
                    if (string(payload_) == "Enter Connect")
                    {
                        PutInSet(clientArray, sockClient);
                    }
                    else if (string(payload_) == "Matching")
                    {
                    	printf("进入匹配模式\n");
                    	if (!isInRoom(sockClient) && !isInVertor(MatchPool, sockClient))
                    	{
                    		if (!MatchPool.empty())
                    		{
                    			SOCKET opponent = MatchPool[0];
                    			MatchPool.erase(MatchPool.begin());
                    			AddInRoom(sockClient, opponent);
                                char sendBuf[128];
                                sprintf_s(sendBuf, "Successful Match:%d:WHITE", roomId);
                    			send_content_data(sockClient, sendBuf);
                                sprintf_s(sendBuf, "Successful Match:%d:BLACK", roomId);
                    			send_content_data(opponent, sendBuf);
                                roomId++;
                    		}
                    		else MatchPool.push_back(sockClient);
                    	}
                    }
                    else if (string(payload_) == "Exit Matching")
                    {
                    	printf("退出匹配模式\n");
                    	if (isInVertor(MatchPool, sockClient))
                    	{
                    		VectorErase(MatchPool, sockClient);
                    		send_content_data(sockClient, "Successful Exit Match");
                    	}
                    }
                    else
                    {
                        vector<string> parseStr = splitString(string(payload_), ":");
                        if (parseStr[0] == "Game")
                        {
                            int roomIndex = stoi(parseStr[1]);
                            Room[roomIndex].push_back(sockClient);
                            if (Room[roomIndex].size() >= 2)
                            {
                                send_content_data(Room[roomIndex][0], "Successful Enter");
                                send_content_data(Room[roomIndex][1], "Successful Enter");
                            }
                        }
                        else if (parseStr[0] == "PutChess")
                        {
                            int roomIndex = stoi(parseStr[2]);
                            if (Room[roomIndex][0] == sockClient)
                            {
                                send_content_data(Room[roomIndex][1], (string("PutChess:") + parseStr[1] + ":" + parseStr[3] + ":" + parseStr[4]).c_str());
                            }
                            else
                            {
                                send_content_data(Room[roomIndex][0], (string("PutChess:") + parseStr[1] + ":" + parseStr[3] + ":" + parseStr[4]).c_str());
                            }
                        }
                        else if (parseStr[0] == "ThreeHand")
                        {
                            int roomIndex = stoi(parseStr[1]);
                            send_content_data(Room[roomIndex][1], (string("ThreeHand:") + parseStr[2]).c_str());
                            send_content_data(Room[roomIndex][0], (string("ThreeHand:") + parseStr[2]).c_str());
                        }
                    }
                }
                else if (ret == 0)
                {
                    cout << "断开连接" << endl;
                    VectorErase(MatchPool, sockClient);
                    VectorErase(clientArray, sockClient);
                    RoomErase(sockClient);
                    break;
                }
            }
        }
        else
        {
            cout << "握手响应失败，关闭socket" << endl;
        }
    }
    closesocket(sockClient);
}

void Initsocket(int port)
{
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, 0);
    if (slisten == INVALID_SOCKET)
    {
        cout << "socket创建失败" << endl;
        return;
    }
    else
    {
        sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.S_un.S_addr = INADDR_ANY;
        if (bind(slisten, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
        {
            cout << "套接字绑定失败" << endl;
            return;
        }
        else
        {
            if (listen(slisten, 5) == SOCKET_ERROR)
            {
                cout << "套接字监听失败" << endl;
                return;
            }
            else
            {
                sockaddr_in remoteAddr;
                int nAddrlen = sizeof(remoteAddr);
                while (true)
                {
                    cout << "等待连接" << endl;
                    SOCKET sClient = accept(slisten, (sockaddr*)&remoteAddr, &nAddrlen);
                    if (sClient == INVALID_SOCKET)
                    {
                        cout << "accept error!" << endl;
                    }
                    else
                    {
                        cout << "准备握手" << endl;
                        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)sClient, 0, 0);
                        if (hThread != NULL)
                        {
                            CloseHandle(hThread);
                        }
                    }
                }
            }
        }
    }
}

int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        cout << "WSAStartup调用失败" << endl;
        return -1;
    }
    int port = 8888;
    Initsocket(port);
    WSACleanup();
    return 0;
}
