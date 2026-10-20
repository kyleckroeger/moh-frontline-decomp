// A fragment of publisher.cpp (0x800d80b0): SECB_NullifyPointerOnMarked, the
// subject callback that clears an observer's subject pointer (and its
// subscription) when the subject is marked for destruction (event 2). The file
// name is this project's; the original record is publisher.cpp. The function,
// IObserver::SetSubject<ISubject> and the types are named by the mangled
// symbols; the event value is inferred. SetSubject's instance is local to the
// original file and comes before this function; it is not part of this unit.
class ISubject;
enum ESubjectEvent {};

class IObserver;
typedef void (*SubjectCallback)(IObserver&, ISubject**, ESubjectEvent);

class IObserver {
public:
    template <class T>
    void SetSubject(T**, T*, SubjectCallback);
};

template <>
void IObserver::SetSubject<ISubject>(ISubject**, ISubject*, SubjectCallback);

void SECB_NullifyPointerOnMarked(IObserver& observer, ISubject** slot, ESubjectEvent event) {
    if (event == 2)
        observer.SetSubject(slot, (ISubject*)0, 0);
}
