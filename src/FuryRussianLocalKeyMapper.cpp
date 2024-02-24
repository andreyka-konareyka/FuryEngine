#include "FuryRussianLocalKeyMapper.h"

FuryRussianLocalKeyMapper::FuryRussianLocalKeyMapper() :
    FuryBaseLocalKeyMapper("Russian")
{

}

int FuryRussianLocalKeyMapper::mapLocalKeyToLatin(int _localKey) const
{
    switch (_localKey) {
    case KeyRus_A:
        return Qt::Key_F;
    case KeyRus_B:
        return Qt::Key_Comma;
    case KeyRus_V:
        return Qt::Key_D;
    case KeyRus_G:
        return Qt::Key_U;
    case KeyRus_D:
        return Qt::Key_L;
    case KeyRus_E:
        return Qt::Key_T;
    case KeyRus_YO:
        return Qt::Key_QuoteLeft;
    case KeyRus_ZH:
        return Qt::Key_Semicolon;
    case KeyRus_Z:
        return Qt::Key_P;
    case KeyRus_I:
        return Qt::Key_B;
    case KeyRus_I_SHORT:
        return Qt::Key_Q;
    case KeyRus_K:
        return Qt::Key_R;
    case KeyRus_L:
        return Qt::Key_K;
    case KeyRus_M:
        return Qt::Key_V;
    case KeyRus_N:
        return Qt::Key_Y;
    case KeyRus_O:
        return Qt::Key_J;
    case KeyRus_P:
        return Qt::Key_G;
    case KeyRus_R:
        return Qt::Key_H;
    case KeyRus_S:
        return Qt::Key_C;
    case KeyRus_T:
        return Qt::Key_N;
    case KeyRus_U:
        return Qt::Key_E;
    case KeyRus_F:
        return Qt::Key_A;
    case KeyRus_H:
        return Qt::Key_BracketLeft;
    case KeyRus_TS:
        return Qt::Key_W;
    case KeyRus_CH:
        return Qt::Key_X;
    case KeyRus_SH:
        return Qt::Key_I;
    case KeyRus_SCH:
        return Qt::Key_O;
    case KeyRus_HARD_SIGN:
        return Qt::Key_BracketRight;
    case KeyRus_Y:
        return Qt::Key_S;
    case KeyRus_SOFT_SIGN:
        return Qt::Key_M;
    case KeyRus_EH:
        return Qt::Key_Apostrophe;
    case KeyRus_YU:
        return Qt::Key_Period;
    case KeyRus_YA:
        return Qt::Key_Z;
    default:
        return 0;
    }
}

int FuryRussianLocalKeyMapper::mapLatinKeyToLocal(int _latinKey) const
{
    switch (_latinKey) {
    case Qt::Key_F:
        return KeyRus_A;
    case Qt::Key_Comma:
        return KeyRus_B;
    case Qt::Key_D:
        return KeyRus_V;
    case Qt::Key_U:
        return KeyRus_G;
    case Qt::Key_L:
        return KeyRus_D;
    case Qt::Key_T:
        return KeyRus_E;
    case Qt::Key_QuoteLeft:
        return KeyRus_YO;
    case Qt::Key_Semicolon:
        return KeyRus_ZH;
    case Qt::Key_P:
        return KeyRus_Z;
    case Qt::Key_B:
        return KeyRus_I;
    case Qt::Key_Q:
        return KeyRus_I_SHORT;
    case Qt::Key_R:
        return KeyRus_K;
    case Qt::Key_K:
        return KeyRus_L;
    case Qt::Key_V:
        return KeyRus_M;
    case Qt::Key_Y:
        return KeyRus_N;
    case Qt::Key_J:
        return KeyRus_O;
    case Qt::Key_G:
        return KeyRus_P;
    case Qt::Key_H:
        return KeyRus_R;
    case Qt::Key_C:
        return KeyRus_S;
    case Qt::Key_N:
        return KeyRus_T;
    case Qt::Key_E:
        return KeyRus_U;
    case Qt::Key_A:
        return KeyRus_F;
    case Qt::Key_BracketLeft:
        return KeyRus_H;
    case Qt::Key_W:
        return KeyRus_TS;
    case Qt::Key_X:
        return KeyRus_CH;
    case Qt::Key_I:
        return KeyRus_SH;
    case Qt::Key_O:
        return KeyRus_SCH;
    case Qt::Key_BracketRight:
        return KeyRus_HARD_SIGN;
    case Qt::Key_S:
        return KeyRus_Y;
    case Qt::Key_M:
        return KeyRus_SOFT_SIGN;
    case Qt::Key_Apostrophe:
        return KeyRus_EH;
    case Qt::Key_Period:
        return KeyRus_YU;
    case Qt::Key_Z:
        return KeyRus_YA;
    default:
        return 0;
    }
}
