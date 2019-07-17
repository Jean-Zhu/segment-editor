#pragma once
#include <QHashFunctions>
#include <QString>

#include <functional>

// TODO merge String.hpp here
namespace std
{
template <>
struct hash<QString>
{
  std::size_t operator()(const QString& path) const { return qHash(path); }
};
} // namespace std
