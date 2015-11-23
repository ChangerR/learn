#include "JpgStreamReceiver.h"
#include <stdio.h>

#pragma comment(lib,"libmjpeg")

bool running = true;

DWORD WINAPI err_handler(LPVOID v) {
	running = false;
	printf("error occur\n");

	return 0;
}

int main(int argc, char** argv){

	JpgStreamReceiver* recv = new JpgStreamReceiver(4);
	int times = 4;
	recv->open("http://localhost:8090/?action=stream", err_handler, NULL);

	while (running&&times)
	{
		Image* img = recv->pop();

		if (img != NULL)
		{
			img->writeToFile(".", NULL);
			Sleep(1000);
			delete img;
			times--;
		}
	}

	recv->close();

	return 0;
}