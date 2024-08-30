#include "FuryDoubleValidator.h"

FuryDoubleValidator::FuryDoubleValidator(QObject *_parent) :
    QDoubleValidator(_parent)
{

}

QValidator::State FuryDoubleValidator::validate(QString &_string, int &_position) const
{
    _string.replace(".", ",");
    State result =  QDoubleValidator::validate(_string, _position);
    _string.replace(",", ".");

    return result;
}
