#pragma once

class ServerListDataSource : public Rocket::Controls::DataSource
{
public:
    static void Initialize();
    static void Shutdown();

    void GetRow(Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns) override;
    int GetNumRows(const Rocket::Core::String& table) override;

    static void AddServer(const char* host, const unsigned short port, const char* name);
    static void ClearServer();

private:
    static ServerListDataSource* instance;

    ServerListDataSource();
    ~ServerListDataSource();

    struct Server
    {
        Rocket::Core::String host;
        unsigned short port;
        Rocket::Core::String name;
    };
    std::vector<Server> m_Servers;
};
