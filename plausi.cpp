#include <string>
#include <deque>
#include <utility>
#include <ctime>
#include <cstdlib>

#include "plausi.h"

#include <QDebug>

Plausi::Plausi(double maxPower, int window) :
    _maxPower(maxPower), _window(window), _value(-1.), _time(0)
{
}

bool Plausi::check(const QString &value)
{
    time_t time;
    qDebug() << "Plausi check: " << value;

    if (value.length() != 7) {
        // exactly 7 digits
        qDebug() << "Plausi rejected: exactly 7 digits";
        return false;
    }

    if (value.lastIndexOf('?') == -1) {
        // no '?' char
        qDebug() << "Plausi rejected: no '?' char";
        return false;
    }

    double dval = value.toDouble() / 10.;

    _queue.push_back(std::make_pair(time, dval));

    if (_queue.size() < _window) {
        qDebug() << "Plausi rejected: not enough values: %d", _queue.size();
        return false;
    }

    if (_queue.size() > _window) {
        _queue.pop_front();
    }

    // iterate through queue and check that all values are ascending
    // and consumption of energy is less than limit
    std::deque<std::pair<time_t, double> >::const_iterator it = _queue.begin();
    time = it->first;
    dval = it->second;
    ++it;

    while (it != _queue.end()) {
        if (it->second < dval) {
            // value must be >= previous value
            qDebug() << "Plausi rejected: value must be >= previous value";
            return false;
        }

        double power = (it->second - dval) / (it->first - time) * 3600.;

        if (power > _maxPower) {
            // consumption of energy must not be greater than limit
            qDebug() << "Plausi rejected: consumption of energy " << power << " must not be greater than limit " << _maxPower;
            return false;
        }

        time = it->first;
        dval = it->second;
        ++it;
    }

    // values in queue are ok: use the center value as candidate, but test again with latest checked value
        qDebug() << queueAsString();

    time_t candTime = _queue.at(_window / 2).first;
    double candValue = _queue.at(_window / 2).second;

    if (candValue < _value) {
        qDebug() << "Plausi rejected: value must be >= previous checked value";
        return false;
    }

    double power = (candValue - _value) / (candTime - _time) * 3600.;

    if (power > _maxPower) {
        qDebug() << "Plausi rejected: consumption of energy (checked value) " << power << " must not be greater than limit " << _maxPower;
        return false;
    }

    // everything is OK -> use the candidate value
    _time = candTime;
    _value = candValue;

    qDebug() << "Plausi accepted: " << _value << " of " << ctime(&_time);

    return true;
}

double Plausi::getCheckedValue()
{
    return _value;
}

time_t Plausi::getCheckedTime()
{
    return _time;
}

QString Plausi::queueAsString()
{
    QString str;

    char buf[20];
    str += "[";
    std::deque<std::pair<time_t, double> >::const_iterator it = _queue.begin();

    for (; it != _queue.end(); ++it) {
        sprintf(buf, "%.1f", it->second);
        str += buf;
        str += ", ";
    }

    str += "]";

    return str;
}


