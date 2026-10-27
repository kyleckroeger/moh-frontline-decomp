// A fragment of publisher.cpp (0x800d80e0): IObserver's destructor (the
// observer's subscriptions are removed from the publisher before the
// ISubject part is destroyed), ISubject::MarkForDestruction (notifies the
// observers with event 2), the weak NotifyObservers (through
// CPublisher::PublishEvent) and ISubject's destructor (notifies event 3
// before the IDestructible part). The file is compiled with RTTI, so this
// unit carries the classes' type descriptors, their base lists and the two
// virtual tables. The unit is compiled '-inline deferred' without auto
// inlining: the functions are emitted in reverse source order (so they are
// written here in reverse image order, which also gives the original order
// of the base lists and virtual tables), and none of the calls is inlined.
// IObserver, ISubject, IDestructible, CPublisher and ESubjectEvent are named
// by the mangled symbols; the virtual slots follow __vt__8ISubject and the
// base lists (IObserver derives from ISubject). NotifyObservers is inline in
// the original (a weak symbol), so it is defined __declspec(weak). The
// publisher's own functions and the rest of the file are not part of this
// unit.
enum ESubjectEvent {};

class ISubject;
class IObserver;

class CPublisher {
public:
    static void PublishEvent(ISubject&, ESubjectEvent);
    static void RemoveObserver(IObserver&);
};

class IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~IDestructible() {}
    virtual void Destroy();
};

class ISubject : public IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISubject();
    void NotifyObservers(ESubjectEvent);
};

class IObserver : public ISubject {
public:
    virtual ~IObserver();
};

ISubject::~ISubject() {
    NotifyObservers((ESubjectEvent)3);
}

__declspec(weak) void ISubject::NotifyObservers(ESubjectEvent event) {
    CPublisher::PublishEvent(*this, event);
}

void ISubject::MarkForDestruction(int flag) {
    IDestructible::MarkForDestruction(flag);
    NotifyObservers((ESubjectEvent)2);
}

IObserver::~IObserver() {
    CPublisher::RemoveObserver(*this);
}

