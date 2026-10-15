// The end of CoverPoint.cpp (0x80053564; the file name is the record's, as the
// static initialiser is named after it): the static initialisation of the
// file-local constant cosCoverPointEffectiveAngle (named by its symbol; const
// as the store scheduling shows), the cosine of
// a 30-degree angle given as a float in radians. The rest of the file is in
// other units.
extern "C" {
#include <math.h>
}

static const float cosCoverPointEffectiveAngle = cos(0.5235988f);
