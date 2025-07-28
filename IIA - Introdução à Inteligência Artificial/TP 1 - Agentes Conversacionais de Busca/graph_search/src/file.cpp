#include "File.hpp"
#include <iostream>

using namespace std;

File::File() {}

File::~File() {
    CloseFile();
}

std::ios_base::openmode File::GetIosMode(OpenMode mode) {
    switch(mode) {
        case READ:
            return std::ios::in;
        case WRITE:
            return std::ios::out | std::ios::trunc;
        case READ_WRITE:
            return std::ios::in | std::ios::out;
        case APPEND:
            return std::ios::out | std::ios::app;
        default:
            return std::ios::in;
    }
}

bool File::OpenFromCommandLine(int argc, char* argv[], OpenMode mode) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <nome_do_arquivo>" << endl;
        return false;
    }

    return OpenFile(argv[1], mode);
}

bool File::OpenFile(const string& filename, OpenMode mode) {
    // Fecha o arquivo atual se estiver aberto
    if (IsOpen()) {
        CloseFile();
    }

    // Obtém as flags de abertura correspondentes
    ios_base::openmode iosMode = GetIosMode(mode);

    // Tenta abrir o arquivo
    file.open(filename, iosMode);

    // Se for modo READ_WRITE e falhar, tenta criar o arquivo
    if (!file.is_open() && mode == READ_WRITE) {
        file.open(filename, std::ios::out);
        if (file.is_open()) {
            file.close();
            file.open(filename, iosMode);
        }
    }

    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return false;
    }

    return true;
}

void File::CloseFile() {
    if (file.is_open()) {
        file.close();
    }
}

bool File::IsOpen() const {
    return file.is_open();
}

fstream& File::GetStream() {
    return file;
}