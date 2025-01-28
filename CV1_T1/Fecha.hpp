#include <ctime>
#include <sstream>
#include <iomanip> 

class Fecha {
public:
    std::string obtenerFechaActual() const {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t); 
        std::ostringstream fechaStream;
        fechaStream << std::put_time(&tm, "%d-%m-%Y %H:%M"); 
        return fechaStream.str();
    }
};


