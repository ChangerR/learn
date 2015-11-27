##libmjpeg
----
###����
    ʹ��vistual studio��win32/libmjpeg.sln������ɵõ�libmjpeg.lib
----
###ʹ��
```cpp
#include "JpgStreamReceiver.h" //����libmjpeg��ͷ�ļ�
#include <stdio.h>

//����libmjpeg���ļ�
#pragma comment(lib,"libmjpeg")

bool running = true;

//���ʳ�������
DWORD WINAPI err_handler(LPVOID v) {
    running = false;
	printf("error occur\n");

	return 0;
}

int main(int argc, char** argv){

    //new JpgStreamReceiver(4) 4=�ڻ����������ͼƬ���� 
	JpgStreamReceiver* recv = new JpgStreamReceiver(4);
    
	int times = 4;
    
    /**
    *function open
    *����1 url
    *����2 ��������
    *����3 ��Ҫ�������������ݵĲ���
    *return �������Ƿ�ɹ�
    */
	recv->open("http://localhost:8090/?action=stream", err_handler, NULL);

	while (running&&times)
	{
        /**
        *function pop  
        *return ���ش�ͼƬ�������еõ���ͼƬ�����ͼƬ������û��ͼƬ�򷵻�NULL
        */
		Image* img = recv->pop();

		if (img != NULL)
		{
			img->writeToFile(".", NULL);
			Sleep(1000);
			delete img;
			times--;
		}
	}

    //�ڳ���������Ҫ�ͷŴ򿪵���Դ
	recv->close();

	return 0;
}
```
----
###����
����1 test1.cpp<BR>
����2 test2.cpp