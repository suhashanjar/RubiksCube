#ifndef MessageManager_h
#define MessageManager_h

#include "../components/IComponent.h"
#include "TriggerOut.h"

#include <algorithm>
#include <vector>
#include <unordered_map>

#pragma region ForwardDeclarations
class IPropertyOut;

template <class T>
class PropertyOut;

class IPropertyIn;

template <class T>
class PropertyIn;


class IMessageOut;

template <class M>
class MessageOut;

class IMessageIn;

template <class M, class O, void(O::*F)(M)>
class MessageIn;


class ITriggerIn;

template <class O, void(O::*F)()>
class TriggerIn;
#pragma endregion

class MessageManager {
    using PropertyConnections_t = std::vector<std::pair<IPropertyOut*, IPropertyIn*>>;
    using MessageConnections_t = std::unordered_map<IMessageOut*, std::vector<IMessageIn*>>;
    using TriggerConnections_t = std::unordered_map<TriggerOut*, std::vector<ITriggerIn*>>;

public:
    /**
     * Type safe managment
     * 
     * Connects and disconnects pipe-out and pipe-in with additional compile-time type check.
     */
    template <class T>
    void Connect(PropertyOut<T>& subject, PropertyIn<T>& observer);

    template <class T>
    void Disconnect(PropertyOut<T>& subject, PropertyIn<T>& observer);

    template <class M, class O, void (O::*F)(M)>
    void Connect(MessageOut<M>& sender, MessageIn<M, O, F>& receiver);

    template <class M, class O, void (O::*F)(M)>
    void Disconnect(MessageOut<M>& sender, MessageIn<M, O, F>& receiver);

    template <class O, void(O::* F)()>
    void Connect(TriggerOut& sender, ITriggerIn& receiver);

    template <class O, void(O::* F)()>
    void Disconnect(TriggerOut& sender, ITriggerIn& receiver);

    void ForwardMessage(IMessageOut* sender, void* message);
    void ForwardTrigger(TriggerOut* sender);

    void RemoveConnections(IComponent* component);

private:
    PropertyConnections_t m_PropertyConnections;
    MessageConnections_t m_MessageConnections;
    TriggerConnections_t m_TriggerConnections;
};

#pragma region InterfacesImplementations
/**************
 *  Property  *
 **************/

class IPropertyOut {
    friend class MessageManager;

public:
    IPropertyOut(IComponent* owner)
        : m_Owner(owner) {}

    IComponent* Owner() const { return m_Owner; }

private:
    IComponent* m_Owner;
};

class IPropertyIn {
    friend class MessageManager;

public:
    IPropertyIn(IComponent* owner)
        : m_Owner(owner) {}

    IComponent* Owner() const { return m_Owner; }

protected:
    virtual void Reset() = 0;

private:
    IComponent* m_Owner;
};


/***************
 *   Message   *
 ***************/

class IMessageOut {
    friend class MessageManager;

public:
    IMessageOut(IComponent* owner)
        : m_MessageManager(nullptr)
        , m_Owner(owner) {}

    IComponent* Owner() const { return m_Owner; }

protected:
    MessageManager* m_MessageManager;

private:
    IComponent* m_Owner;
};

class IMessageIn {
    friend class MessageManager;

public:
    IMessageIn(IComponent* owner)
        :  m_Owner(owner) {}

    virtual void Receive(void* message) = 0;

    IComponent* Owner() const { return m_Owner; }

private:
    IComponent* m_Owner;
};


/*************
 *  Trigger  *
 *************/

class ITriggerIn {
    friend class MessageManager;

public:
    ITriggerIn(IComponent* owner)
        : m_Owner(owner) {}

    virtual void Receive() = 0;

    IComponent* Owner() const { return m_Owner; }

private:
    IComponent* m_Owner;
};
#pragma endregion

template <class T>
void MessageManager::Connect(PropertyOut<T>& subject, PropertyIn<T>& observer) {
    if (observer.m_Source == nullptr) {
        observer.m_Source = &subject;

        m_PropertyConnections.emplace_back(subject, observer);
    } else {
        // TODO DebugLog
        std::cout << "PropertyIn of type " << typeid(T).name() << " already has connection\n";
    }
}

template <class T>
void MessageManager::Disconnect(PropertyOut<T>& subject, PropertyIn<T>& observer) {
    m_PropertyConnections.erase(std::remove_if(m_PropertyConnections.begin(),
                                               m_PropertyConnections.end(),
                                               [=](std::pair<IPropertyOut*, IPropertyIn*>& pair) { 
                                                   if (pair.first == subject && pair.second == observer) {
                                                       pair.second->Reset();
                                                       return true;
                                                   }
                                                   return false; }));
}

template <class M, class O, void(O::*F)(M)>
void MessageManager::Connect(MessageOut<M>& sender, MessageIn<M, O, F>& receiver) {
    sender.m_MessageManager = this;
    m_MessageConnections[&sender].push_back(&receiver);
}

template <class M, class O, void (O::*F)(M)>
void MessageManager::Disconnect(MessageOut<M>& sender, MessageIn<M, O, F>& receiver) {
    m_MessageConnections[&sender].erase(std::remove(m_MessageConnections[&sender].begin(), 
                                                    m_MessageConnections[&sender].end(), 
                                                    receiver));
}

template <class O, void(O::* F)()>
void MessageManager::Connect(TriggerOut& sender, ITriggerIn& receiver) {
    sender.m_MessageManager = this;
    m_TriggerConnections[&sender].push_back(&receiver);
}

template <class O, void(O::* F)()>
void MessageManager::Disconnect(TriggerOut& sender, ITriggerIn& receiver) {
    m_TriggerConnections[&sender].erase(std::remove(m_TriggerConnections[&sender].begin(),
                                                    m_TriggerConnections[&sender].end(),
                                                    receiver));
}

#endif
