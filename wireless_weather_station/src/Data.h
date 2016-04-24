#pragma once

namespace Data {

class WeatherData {
public:
    inline WeatherData()
        : m_temperature( 0 )
        , m_humidity( 0 )
        , m_light( 0 )
        , m_pressure( 0 )
        , m_altitude( 0 )
    {

    }

    inline void setTemperature( const double &temp ) {
        m_temperature = temp;
    }

    inline const double & temparature() const {
        return m_temperature;
    }

    inline void setHumidity( const double &humidity ) {
        m_humidity = humidity;
    }

    inline const double & humidity() const {
        return m_humidity;
    }

    inline void setLight( const double &light ) {
        m_light = light;
    }

    inline const double & light() const  {
        return m_light;
    }

    inline void setPressure( const double &pressure ) {
        m_pressure = pressure;
    }

    inline const double & pressure() const {
        return m_pressure;
    }

    inline void setAltitude( const double &altitude ) {
        m_altitude = altitude;
    }

    inline const double & altitude() const {
        return  m_altitude;
    }

private:
    double m_temperature;

    double m_humidity;

    double m_light;

    double m_pressure;

    double m_altitude;
};

}
