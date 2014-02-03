#include "stdafx.h"
#include "serverlistdatasource.h"

ServerListDataSource* ServerListDataSource::instance;

ServerListDataSource::ServerListDataSource()
    : Rocket::Controls::DataSource("server_list")
{
    ROCKET_ASSERT(instance == NULL);
    instance = this;
}

ServerListDataSource::~ServerListDataSource()
{
}

void ServerListDataSource::Initialize()
{
    new ServerListDataSource();
}

void ServerListDataSource::Shutdown()
{
    delete instance;
}

void ServerListDataSource::GetRow(Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns)
{
    if (table == "servers")
    {
        if (row_index < (int)m_Servers.size())
        {
            for (size_t i = 0; i < columns.size(); i++)
            {
                if (columns[i] == "index")
                {
                    row.push_back(Rocket::Core::String(4, "%d", row_index+1));
                }
                else if (columns[i] == "name")
                {
                    row.push_back(m_Servers[row_index].name);
                }
                else if (columns[i] == "host")
                {
                    row.push_back(m_Servers[row_index].host);
                }
                else if (columns[i] == "port")
                {
                    row.push_back(Rocket::Core::String(5, "%d", (int)m_Servers[row_index].port));
                }
            }
        }
    }
}

int ServerListDataSource::GetNumRows(const Rocket::Core::String& table)
{
    return m_Servers.size();
}

void ServerListDataSource::AddServer(const char* host, const unsigned short port, const char* name)
{
    Server s;
    s.host = host;
    s.name = name;
    s.port = port;

    instance->m_Servers.push_back(s);

    instance->NotifyRowAdd("servers", instance->m_Servers.size() - 1, 1);
}

void ServerListDataSource::ClearServer()
{
    instance->NotifyRowRemove("servers", 0, instance->m_Servers.size());

    instance->m_Servers.clear();
}
