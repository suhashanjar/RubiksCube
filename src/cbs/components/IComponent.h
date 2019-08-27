#ifndef IComponent_h
#define IComponent_h

#include <iostream>
#include <exception>

class Object;
class Transform;

class Component {
    // TODO doc about friendship and constructors
    friend class Object;

public:
    Component() = default;
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;
    virtual ~Component() = default;

    std::uint8_t ID() const { return m_ID; }
    Object& Object() const { return *m_Object; }

protected:
    virtual void Initialize() {};
    virtual void Update() {};
    virtual void Destroy() {};

    void RegisterUpdateCall() const;

private:
    class Object* m_Object{ nullptr };
    std::uint8_t m_ID{ 0 };
};

#endif
