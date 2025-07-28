#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>

using namespace std;

class File {

    public:
        // Enum para modos de abertura
        enum OpenMode {
            READ,       // Somente leitura
            WRITE,      // Somente escrita (trunca o arquivo)
            READ_WRITE, // Leitura e escrita
            APPEND      // Escrita no final (append)
        };

        // Construtor/destruidor
        File();
        ~File();

        // Abre arquivo passado por linha de comando (argv) com modo específico
        bool OpenFromCommandLine(int argc, char* argv[], OpenMode mode = READ_WRITE);

        // Abre arquivo recebendo o nome e modo como parâmetro
        bool OpenFile(const string& filename, OpenMode mode = READ_WRITE);

        // Fecha o arquivo
        void CloseFile();

        // Verifica se o arquivo está aberto
        bool IsOpen() const;

        // Acesso ao stream
        fstream& GetStream();

    private:
        fstream file;

        // Método interno para converter OpenMode para ios flags
        ios_base::openmode GetIosMode(OpenMode mode);
};

#endif // FILE_HPP