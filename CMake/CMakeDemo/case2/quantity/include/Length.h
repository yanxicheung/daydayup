#ifndef H77A07165_EC3F_46ED_9899_E9E8A8255C02
#define H77A07165_EC3F_46ED_9899_E9E8A8255C02

#include "Amount.h"

enum LengthUnit
{
    INCH = 1,
    FEET = 12 * INCH,
};

struct Length
{
    Length(Amount amount, LengthUnit unit);

    bool operator==(const Length& rhs) const;
    bool operator!=(const Length& rhs) const;

private:
    const Amount amountInBaseUnit;
};

#endif /* H77A07165_EC3F_46ED_9899_E9E8A8255C02 */
