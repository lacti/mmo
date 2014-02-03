#pragma once

class resource_path_resolver
{
public:
    explicit resource_path_resolver(const char* name);
    const char* get() const { return resource_path_buf; }

private:
    resource_path_resolver();
    char resource_path_buf[MAX_PATH];
};
