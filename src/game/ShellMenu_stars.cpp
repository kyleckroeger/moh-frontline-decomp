// A fragment of ShellMenu.cpp (0x800e85a0): SetStarRatingStatus raises a
// level's star rating (levels 11-64 by mission and level number, two bits
// each, four to a byte, the first in the top bits) to the given rating when
// it is higher. For levels 24, 34, 44, 63 and 64 the code clears the rating
// instead, and that is kept. The function and StarLevelsInBits are named by
// the symbols; the bit layout follows the code and the field names are
// placeholders by level id, as in ShellMenu_unlock.cpp. The rest of the
// file is in other units.
struct StarLevelsInBits {
    unsigned int level11 : 2;
    unsigned int level12 : 2;
    unsigned int level13 : 2;
    unsigned int level14 : 2;
    unsigned int level21 : 2;
    unsigned int level22 : 2;
    unsigned int level23 : 2;
    unsigned int level24 : 2;
    unsigned int level31 : 2;
    unsigned int level32 : 2;
    unsigned int level33 : 2;
    unsigned int level34 : 2;
    unsigned int level41 : 2;
    unsigned int level42 : 2;
    unsigned int level43 : 2;
    unsigned int level44 : 2;
    unsigned int level51 : 2;
    unsigned int level52 : 2;
    unsigned int level53 : 2;
    unsigned int level54 : 2;
    unsigned int level61 : 2;
    unsigned int level62 : 2;
    unsigned int level63 : 2;
    unsigned int level64 : 2;
};

void SetStarRatingStatus(StarLevelsInBits* stars, unsigned int level, unsigned int rating) {
    switch (level) {
    case 11:
        if (stars->level11 < rating)
            stars->level11 = rating;
        break;
    case 12:
        if (stars->level12 < rating)
            stars->level12 = rating;
        break;
    case 13:
        if (stars->level13 < rating)
            stars->level13 = rating;
        break;
    case 14:
        if (stars->level14 < rating)
            stars->level14 = rating;
        break;
    case 21:
        if (stars->level21 < rating)
            stars->level21 = rating;
        break;
    case 22:
        if (stars->level22 < rating)
            stars->level22 = rating;
        break;
    case 23:
        if (stars->level23 < rating)
            stars->level23 = rating;
        break;
    case 24:
        stars->level24 = 0;
        break;
    case 31:
        if (stars->level31 < rating)
            stars->level31 = rating;
        break;
    case 32:
        if (stars->level32 < rating)
            stars->level32 = rating;
        break;
    case 33:
        if (stars->level33 < rating)
            stars->level33 = rating;
        break;
    case 34:
        stars->level34 = 0;
        break;
    case 41:
        if (stars->level41 < rating)
            stars->level41 = rating;
        break;
    case 42:
        if (stars->level42 < rating)
            stars->level42 = rating;
        break;
    case 43:
        if (stars->level43 < rating)
            stars->level43 = rating;
        break;
    case 44:
        stars->level44 = 0;
        break;
    case 51:
        if (stars->level51 < rating)
            stars->level51 = rating;
        break;
    case 52:
        if (stars->level52 < rating)
            stars->level52 = rating;
        break;
    case 53:
        if (stars->level53 < rating)
            stars->level53 = rating;
        break;
    case 54:
        if (stars->level54 < rating)
            stars->level54 = rating;
        break;
    case 61:
        if (stars->level61 < rating)
            stars->level61 = rating;
        break;
    case 62:
        if (stars->level62 < rating)
            stars->level62 = rating;
        break;
    case 63:
        stars->level63 = 0;
        break;
    case 64:
        stars->level64 = 0;
        break;
    }
}
