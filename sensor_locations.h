#pragma once

enum class SensorLocation
{
  Attic,
  Gym,
  LaundryRoom,
  LivingRoom,
  Office,
  Outside,
  Patio
};

inline const char *locationName(SensorLocation location)
{
  switch (location)
  {
  case SensorLocation::Attic:
    return "attic";
  case SensorLocation::Gym:
    return "gym";
  case SensorLocation::LaundryRoom:
    return "laundryroom";
  case SensorLocation::LivingRoom:
    return "livingroom";
  case SensorLocation::Office:
    return "office";
  case SensorLocation::Outside:
    return "outside";
  case SensorLocation::Patio:
    return "patio";
  }

  return "unknown";
}
