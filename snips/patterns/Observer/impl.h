#ifndef IMPL_BLA_H
#define IMPL_BLA_H
#include <iostream>
#include <memory>
#include <vector>

template<typename MessageT = std::string>
struct Notification {
    MessageT Message;
    size_t clientid = 0;

    Notification() = default;
    explicit Notification(MessageT const& m)
        : Message { m }
    {
    }

    static std::unique_ptr<Notification<MessageT>> create()
    {
        return std::make_unique<Notification<MessageT>>();
    }

    static std::unique_ptr<Notification<MessageT>> create(MessageT const& message)
    {
        return std::make_unique<Notification<MessageT>>(message);
    }
};

class StringMessageClient {
public:
    void onNotify(Notification<std::string> const& notification)
    {
        std::cout << notification.Message << "\n";
    }

    static std::unique_ptr<StringMessageClient> create()
    {
        return std::make_unique<StringMessageClient>();
    }
};

template<typename NotificationT, typename ClientT, typename MessageT>
class Observer {
    std::vector<std::unique_ptr<NotificationT>> m_Notifications;
    std::vector<std::unique_ptr<ClientT>> m_Clients;

public:
    explicit Observer()
    {
        m_Clients.push_back(ClientT::create());
    }

    void insert_client(std::unique_ptr<ClientT>&& client)
    {
        m_Clients.push_back(std::move(client));
    }

    void insert_client()
    {
        m_Clients.push_back(ClientT::create());
    }

    void insert_notification(MessageT const& insertion)
    {
        m_Notifications.push_back(NotificationT::create(insertion));
    }

    void Notify()
    {
        if (m_Notifications.empty() || m_Clients.empty()) {
            return;
        }

        for (auto& client : m_Clients) {
            for (auto& notif : m_Notifications) {
                client->onNotify(*notif);
            }
        }
    }
};
#endif
