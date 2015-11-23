##libmjpeg
----
###编译
    使用vistual studio打开win32/libmjpeg.sln，编译可得到libmjpeg.lib
----
###使用
```cpp
#include "JpgStreamReceiver.h" //包含libmjpeg的头文件
#include <stdio.h>

//连接libmjpeg库文件
#pragma comment(lib,"libmjpeg")

bool running = true;

//访问出错处理函数
DWORD WINAPI err_handler(LPVOID v) {
    running = false;
	printf("error occur\n");

	return 0;
}

int main(int argc, char** argv){

    //new JpgStreamReceiver(4) 4=在缓冲区的最大图片数量 
	JpgStreamReceiver* recv = new JpgStreamReceiver(4);
    
	int times = 4;
    
    /**
    *function open
    *参数1 url
    *参数2 错误处理函数
    *参数3 需要给错误处理函数传递的参数
    *return 打开链接是否成功
    */
	recv->open("http://localhost:8090/?action=stream", err_handler, NULL);

	while (running&&times)
	{
        /**
        *function pop  
        *return 返回从图片缓冲区中得到的图片，如果图片缓冲区没有图片则返回NULL
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

    //在程序的最后需要释放打开的资源
	recv->close();

	return 0;
}
```
----
###测试
测试1 test1.cpp<BR>
测试2 test2.cpp