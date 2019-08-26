#ifndef DummyComp_h
#define DummyComp_h

#include "IComponent.h"
#include "../Object.h"
#include "../../scenes/IScene.h"
#include "../message_system/MessageOut.h"
#include "../message_system/MessageIn.h"
#include "../message_system/PropertyOut.h"
#include "../message_system/PropertyIn.h"
#include "../message_system/TriggerOut.h"
#include "../message_system/TriggerIn.h"

#include "../../utilities/Input.h"

#include <iostream>
#include <string>

class DummyComp : public IComponent {
public:
    DummyComp();
    ~DummyComp();

    void Initialize() override;
    void Update() override;
    void Destroy() override;

    void OnMessage(std::string msg);
    void OnTrigger();

    MessageOut<std::string> m_MessageOut { this };

    MessageIn<std::string, DummyComp, &DummyComp::OnMessage> m_MessageIn { this };

    PropertyOut<float> m_PropertyOut { this, 0.0f };

    PropertyIn<float> m_PropertyIn { this };

    TriggerOut m_TriggerOut { this };

    TriggerIn<DummyComp, &DummyComp::OnTrigger> m_TriggerIn{ this };

private:
    std::string m_Message;
};

#endif