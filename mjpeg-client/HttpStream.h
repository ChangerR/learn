#ifndef __SLSERVER_HTTPSTREAM_
#define __SLSERVER_HTTPSTREAM_
/***************************************************************************
*
*  Project libmjpeg
*
* Copyright (C) 2014-2015, Changer, <dingjinchengyx@163.com>.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
***************************************************************************/

#include "slconfig.h"
#include "list.h"

class HttpStream {
public:
	HttpStream(int size_count,int cnt = 16);
	
	virtual ~HttpStream();
	
public:
	
	void lock();
	
	void unlock();

	int read(u8* buf,int len);
	
	int readline(u8* buf,int len);
	
	int skip(int len);
	
	int write(const u8* wbuf,int len,bool blocked = false);
	
	int left();

	void alloc_new_page(int cnt);
	
	void setEOF(bool v);
	
	bool isEOF();
	
	int size();
	
	void clear();
	
private:
	int _page_size;
	list<u8*>* _free_page;
	list<u8*>* _p_arr;
	int _read_pos;
	int _write_pos;
	u8* _readpage_addr;
	u8* _writepage_addr;
	CRITICAL_SECTION _write_mutex;
	bool _isEOF;
};

#endif