#include <iostream>
#include <ctime>
#include <iomanip>

class C1 {
public:
    void Saludo(const std::string& nombreEstudiante) const {
        std::string fecha = obtenerFechaActual();
        std::cout << "Hola Mundo. Saludo de " << nombreEstudiante << " hoy " << fecha << std::endl;
    }

private:
    // Función auxiliar para obtener la fecha actual formateada
    std::string obtenerFechaActual() const {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream fechaStream;
        fechaStream << std::put_time(&tm, "%d-%m-%Y %H:%M");
        return fechaStream.str();
    }
};

