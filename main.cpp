#include <iostream>
#include <string>
#include <limits>
#include <new>

const int PROD = 5;
const int MP = 4;

int leerEntero(const std::string &mensaje, int minv, int maxv) {
    while (true) {
        std::string linea;
        std::cout << mensaje;
        if (!std::getline(std::cin, linea)) return minv;
        try {
            int val = std::stoi(linea);
            if (val < minv || val > maxv) {
                std::cout << "Valor fuera de rango (" << minv << "-" << maxv << ").\n";
                continue;
            }
            return val;
        } catch (...) {
            std::cout << "Entrada inválida. Ingresa un número entero.\n";
        }
    }
}

float leerFloat(const std::string &mensaje, float minv, float maxv) {
    while (true) {
        std::string linea;
        std::cout << mensaje;
        if (!std::getline(std::cin, linea)) return minv;
        try {
            float val = std::stof(linea);
            if (val < minv || val > maxv) {
                std::cout << "Valor fuera de rango.\n";
                continue;
            }
            return val;
        } catch (...) {
            std::cout << "Entrada inválida. Ingresa un número.\n";
        }
    }
}

float** crearMatriz(int f, int c) {
    float** m = new float*[f];
    for (int i = 0; i < f; ++i) {
        m[i] = new float[c];
        for (int j = 0; j < c; ++j) m[i][j] = 0.0f;
    }
    return m;
}

void borrarMatriz(float** &m, int f) {
    if (!m) return;
    for (int i = 0; i < f; ++i) delete[] m[i];
    delete[] m;
    m = nullptr;
}

bool agregarSemana(float** m, int f, int &c) {
    int n = c + 1;
    try {
        for (int i = 0; i < f; ++i) {
            float* nuevo = new float[n];
            for (int j = 0; j < c; ++j) nuevo[j] = m[i][j];
            nuevo[n-1] = 0.0f;
            delete[] m[i];
            m[i] = nuevo;
        }
        c = n;
        return true;
    } catch (const std::bad_alloc&) {
        std::cout << "Error de memoria al agregar semana.\n";
        return false;
    }
}

void mostrarPlan(float** m, int f, int c) {
    std::cout << "\nPlan (" << f << "x" << c << ")\n";
    for (int i = 0; i < f; ++i) {
        for (int j = 0; j < c; ++j) {
            std::cout << m[i][j];
            if (j < c-1) std::cout << "\t";
        }
        std::cout << "\n";
    }
}

void modificar(float** m, int f, int c) {
    int p = leerEntero("Producto (0-" + std::to_string(f-1) + "): ", 0, f-1);
    int s = leerEntero("Semana (0-" + std::to_string(c-1) + "): ", 0, c-1);
    float q = leerFloat("Cantidad: ", 0.0f, 1e12f);
    m[p][s] = q;
    std::cout << "Registrado.\n";
}

void consumoMP(float** m, int f, int semana, const int req[PROD][MP], float res[MP]) {
    for (int i = 0; i < MP; ++i) res[i] = 0.0f;
    for (int p = 0; p < f; ++p) {
        float cant = m[p][semana];
        for (int k = 0; k < MP; ++k) res[k] += cant * static_cast<float>(req[p][k]);
    }
}

void calcularCOGS(float** m, int f, int c, int semana, const float* costoUnit, const float* invInicial, const float mpCost[MP], const int req[PROD][MP]) {
    float totalUnidades = 0.0f;
    float costoProd = 0.0f;
    for (int p = 0; p < f; ++p) {
        float cant = m[p][semana];
        totalUnidades += cant;
        costoProd += cant * costoUnit[p];
    }
    float cons[MP];
    consumoMP(m, f, semana, req, cons);
    float invFinal[MP];
    float valorInvFinal = 0.0f;
    for (int i = 0; i < MP; ++i) {
        invFinal[i] = invInicial[i] - cons[i];
        if (invFinal[i] < 0.0f) invFinal[i] = 0.0f;
        valorInvFinal += invFinal[i] * mpCost[i];
    }
    std::cout << "\nSemana " << semana << "\n";
    std::cout << "Produccion total: " << totalUnidades << "\n";
    std::cout << "COGS (costo produccion): " << costoProd << "\n";
    std::cout << "Consumo MP:\n";
    for (int i = 0; i < MP; ++i) std::cout << " MP-" << i+1 << ": " << cons[i] << "\n";
    std::cout << "Valor inventario final (MP): " << valorInvFinal << "\n";
}

int main() {
    int semanas = 4;
    float** plan = nullptr;
    float* costo = nullptr;
    float* inv = nullptr;
    const float mpCost[MP] = {2.5f, 1.75f, 0.9f, 0.5f};
    const int req[PROD][MP] = {
        {1,0,2,1},
        {0,1,1,0},
        {2,1,0,1},
        {1,2,1,0},
        {0,1,2,1}
    };
    try {
        plan = crearMatriz(PROD, semanas);
        costo = new float[PROD];
        inv = new float[MP];
    } catch (const std::bad_alloc&) {
        std::cout << "Error de memoria al iniciar.\n";
        return 1;
    }
    for (int i = 0; i < PROD; ++i) costo[i] = 10.0f + 2.0f * i;
    inv[0] = 1000.0f; inv[1] = 800.0f; inv[2] = 600.0f; inv[3] = 400.0f;
    for (int p = 0; p < PROD; ++p)
        for (int s = 0; s < semanas; ++s)
            plan[p][s] = 0.0f;

    while (true) {
        std::cout << "\n1.Ver plan\n2.Agregar semana\n3.Modificar produccion\n4.Calcular COGS e inventario\n5.Reporte consumo MP\n6.Salir\nOpcion: ";
        std::string linea;
        if (!std::getline(std::cin, linea)) break;
        int op;
        try { op = std::stoi(linea); } catch (...) { std::cout << "Opcion invalida.\n"; continue; }

        if (op == 1) {
            mostrarPlan(plan, PROD, semanas);
        } else if (op == 2) {
            if (agregarSemana(plan, PROD, semanas)) std::cout << "Semana agregada. Total semanas: " << semanas << "\n";
        } else if (op == 3) {
            modificar(plan, PROD, semanas);
        } else if (op == 4) {
            int s = leerEntero("Semana (0-" + std::to_string(semanas-1) + "): ", 0, semanas-1);
            calcularCOGS(plan, PROD, semanas, s, costo, inv, mpCost, req);
        } else if (op == 5) {
            int s = leerEntero("Semana (0-" + std::to_string(semanas-1) + "): ", 0, semanas-1);
            float cons[MP];
            consumoMP(plan, PROD, s, req, cons);
            std::cout << "Consumo MP semana " << s << ":\n";
            for (int i = 0; i < MP; ++i) std::cout << " MP-" << i+1 << ": " << cons[i] << "\n";
        } else if (op == 6) {
            break;
        } else {
            std::cout << "Opcion no valida.\n";
        }
    }

    borrarMatriz(plan, PROD);
    delete[] costo;
    delete[] inv;
    return 0;
}
