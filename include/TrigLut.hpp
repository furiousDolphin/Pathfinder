#ifndef ELLIPSE_LUT_HPP_
#define ELLIPSE_LUT_HPP_

#include <array>
#include <cmath>
#include <numbers>

constexpr double PI = 3.14159265358979323846;


constexpr double toRadians(double degrees) 
{
  return degrees * PI / 180.0;
}

constexpr double toDegrees(double radians) 
{
  return radians * 180.0 / PI;
}


constexpr double mySin(double x, int terms = 10) 
{
  double sum = x;
  double term = x;
  for (int i = 1; i < terms; ++i) {
    term *= -x * x / ((2 * i) * (2 * i + 1));
    sum += term;
  }
  return sum;
}


constexpr double myCos(double x, int terms = 10) 
{
  double sum = 1.0;
  double term = 1.0;
  for (int i = 1; i < terms; ++i) {
    term *= -x * x / ((2 * i - 1) * (2 * i));
    sum += term;
  }
  return sum;
}


constexpr std::array<double, 1440> generateSinTable() 
{
  std::array<double, 1440> table{};
  for (int i = 0; i < 1440; ++i) {
    table[i] = mySin(toRadians(i * 0.25));
  }
  return table;
}

constexpr std::array<double, 1440> generateCosTable() 
{
  std::array<double, 1440> table{};
  for (int i = 0; i < 1440; ++i) 
  {
    table[i] = myCos(toRadians(i * 0.25));
  }
  return table;
}

constexpr std::array< std::pair< std::pair<double, double>, double>, 1440> generateAngleTable()
{
  std::array< std::pair< std::pair<double, double>, double>, 1440> table{};
  
  for (int i = 0; i < 1440; ++i) 
  {
    double angle_deg = i * 0.25;
    double angle_rad = toRadians(angle_deg);

    table[i].first.first = myCos(angle_deg);
    table[i].first.second = mySin(angle_deg);
    table[i].second = angle_deg;
  }

  return table;
}


constexpr auto sinTable = generateSinTable();
constexpr auto cosTable = generateCosTable();
constexpr auto angleTable = generateAngleTable();


inline double getTableSin(double angle_deg) 
{
  angle_deg = fmod(angle_deg, 360.0);
  if (angle_deg < 0) angle_deg += 360.0;

  int index = static_cast<int>(angle_deg * 4.0 + 0.5);
  return sinTable[index % 1440];
}

inline double getTableCos(double angle_deg) 
{
  return getTableSin(angle_deg + 90.0);
}




inline double getAngleFromSinCosLookup(double sin_val, double cos_val)
{
    double best_angle = 0.0;
    double best_error = std::numeric_limits<double>::max();

    for (const auto& entry : angleTable)
    {
        double d_cos = entry.first.first  - cos_val;
        double d_sin = entry.first.second - sin_val;
        double error = d_cos * d_cos + d_sin * d_sin;

        if (error < best_error)
        {
            best_error = error;
            best_angle = entry.second;
        }
    }

    return best_angle;
}



inline double getTableAtan2(double sin_val, double cos_val) 
{
    double angle_rad = std::atan2(sin_val, cos_val);
    double angle_deg = angle_rad * 180.0 / PI;
    if (angle_deg < 0) angle_deg += 360.0;
    return angle_deg;
}

#endif