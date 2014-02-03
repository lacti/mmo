#include "stdafx.h"
#include <tbb/concurrent_unordered_set.h>
#include <ppl.h>
#include <concurrent_unordered_set.h>

void output_debug_string(const char* utf8)
{
    wchar_t wide[512];
    MultiByteToWideChar(CP_UTF8, 0, utf8, strlen(utf8) + 1, wide, _countof(wide));
    OutputDebugStringW(wide);
}

struct TestData
{
    TestData(int k, int d) : key(k), data(d) {}
    TestData() : key(0), data(0) {}
    
    int key;
    int data;
};

bool operator == (const TestData& lhs, const TestData& rhs) { return lhs.key == rhs.key; }

int _tmain(int argc, _TCHAR* argv[])
{
    mmocl_context ctx;
    cl_initialize_params params = {0};
    params.debug_line_output_handler = output_debug_string;

	cl_initialize_context(&ctx, &params);
    
    cl_connect_to_server("localhost");
    
    cl_enter_world("yahoo");

    cl_start_network_thread();

    tbb::concurrent_unordered_set<char> uoset;
    //TestData td(1,2);
    uoset.insert('a');
    uoset.insert('c');
    uoset.insert('d');

    concurrency::concurrent_unordered_set<char> uoset2;
    uoset2.insert('a');
    uoset2.insert('a');
    uoset2.insert('c');

    /*
    uoset.insert(TestData(2,4));
    uoset.insert(TestData(3,6));*/

    while(1)
    {
    }
        
    cl_destroy_context();

	return 0;
}
