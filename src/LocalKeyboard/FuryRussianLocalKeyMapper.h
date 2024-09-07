#ifndef FURYRUSSIANLOCALKEYMAPPER_H
#define FURYRUSSIANLOCALKEYMAPPER_H

#include "FuryBaseLocalKeyMapper.h"

//! Класс отображения локальных русских клавиш на латинские
class FuryRussianLocalKeyMapper : public FuryBaseLocalKeyMapper
{
public:
    //! Конструктор
    FuryRussianLocalKeyMapper();

    /*!
     * \brief mapLocalKeyToLatin
     * \param[in] _localKey - Локальный код клавиши
     * \return Возвращает латинский код клавиши
     */
    int mapLocalKeyToLatin(int _localKey) const override;

    /*!
     * \brief mapLatinKeyToLocal
     * \param[in] _latinKey - Латинский код клавиши
     * \return Возвращает локальный код клавиши
     */
    int mapLatinKeyToLocal(int _latinKey) const override;

private:
    //! Перечисление кодов русских символов
    enum RussianKeyCodes
    {
        KeyRus_A = 1040, //!< а
        KeyRus_B = 1041, //!< б
        KeyRus_V = 1042, //!< в
        KeyRus_G = 1043, //!< г
        KeyRus_D = 1044, //!< д
        KeyRus_E = 1045, //!< е
        KeyRus_YO = 1025, //!< ё
        KeyRus_ZH = 1046, //!< ж
        KeyRus_Z = 1047, //!< з
        KeyRus_I = 1048, //!< и
        KeyRus_I_SHORT = 1049, //!< й
        KeyRus_K = 1050, //!< к
        KeyRus_L = 1051, //!< л
        KeyRus_M = 1052, //!< м
        KeyRus_N = 1053, //!< н
        KeyRus_O = 1054, //!< о
        KeyRus_P = 1055, //!< п
        KeyRus_R = 1056, //!< р
        KeyRus_S = 1057, //!< с
        KeyRus_T = 1058, //!< т
        KeyRus_U = 1059, //!< у
        KeyRus_F = 1060, //!< ф
        KeyRus_H = 1061, //!< х
        KeyRus_TS = 1062, //!< ц
        KeyRus_CH = 1063, //!< ч
        KeyRus_SH = 1064, //!< ш
        KeyRus_SCH = 1065, //!< щ
        KeyRus_HARD_SIGN = 1066, //!< ъ
        KeyRus_Y = 1067, //!< ы
        KeyRus_SOFT_SIGN = 1068, //!< ь
        KeyRus_EH = 1069, //!< э
        KeyRus_YU = 1070, //!< ю
        KeyRus_YA = 1071//!< я
    };
};

#endif // FURYRUSSIANLOCALKEYMAPPER_H
