#include <iostream>
#include <string>
#include "Fecha.hpp"

class C1 {
private:
    std::string nombre;
public:
    C1(const std::string& nombreEstudiante) : nombre(nombreEstudiante) {}

    void mostrarMensaje() const {
        Fecha fecha;
        std::string fechaActual = fecha.obtenerFechaActual();
        std::cout << "Hola Mundo. Saludo de " << nombre << " hoy " << fechaActual << " ." << std::endl;
    }
};

