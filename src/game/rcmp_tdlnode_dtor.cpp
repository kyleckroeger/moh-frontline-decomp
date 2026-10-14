// A fragment of rcmp_mpc_codec.cpp (0x80104c78): the weak (empty) destructor
// of RCON::TDLNODE<MPC_FRAME>. The template and MPC_FRAME are named by the
// mangled symbol; the destructor is a weak template copy emitted in this file,
// defined __declspec(weak) out of the class and instantiated explicitly. The
// rest of the file is not part of this unit.
struct MPC_FRAME;

namespace RCON {
template <class T>
class TDLNODE {
public:
    ~TDLNODE();
};

template <class T> __declspec(weak) TDLNODE<T>::~TDLNODE() {
}
}

template class RCON::TDLNODE<MPC_FRAME>;
