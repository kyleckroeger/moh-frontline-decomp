// A fragment of scene.cpp (0x800d8a9c): the file-local instance of
// dwi::copy for vectors of collisions (each element assigned through
// CCollision's assignment operator). The dwi containers and CCollision are
// named by the mangled symbols; the iterator's pointer and the collision view
// are inferred, and the template is file-local (static) in the original, as
// its local symbol shows, so it is static here and instantiated explicitly.
// The rest of the file is not part of this unit.
class ISceneNode;

class CCollision {
public:
    CCollision& operator=(const CCollision&);

    ISceneNode* m_node0;
    ISceneNode* m_node1;
    unsigned char data08[24];
};

#include "game/DwiContainers.h"
namespace dwi {

template <class I, class O>
static O copy(I first, I last, O result) {
    while (first != last) {
        *result = *first;
        ++result;
        ++first;
    }
    return result;
}

}

template dwi::vec_iter<CCollision> dwi::copy(dwi::vec_iter<CCollision>, dwi::vec_iter<CCollision>,
                                             dwi::vec_iter<CCollision>);
