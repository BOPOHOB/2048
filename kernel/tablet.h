#ifndef TABLET_H
#define TABLET_H

#include <QtGlobal>

class Tablet
{
public:
    explicit Tablet(const quint64&v) : pId(ID_COUNTER++), val(v) {}
    Tablet(const Tablet& op) : pId(op.pId), val(op.val) {}
    //Tablet() : pId(ID_COUNTER++) { }
    Tablet& operator=(const quint64& v) { val = v; return *this; }
    operator const quint64&() const { return val; }
    operator quint64&() { return val; }
    const quint64& id() const { return pId; }

private:
    const quint64 pId;
    quint64 val;

    static quint64 ID_COUNTER;
};

#endif // TABLET_H
