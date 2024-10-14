#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Tokkens/lexer.hpp"



int main(int argc , char ** argv)
{
    if (argc < 2)
    {
        std::cout << "Please supply the source file";
        exit(1);
    }
    // pilot example.pi

    std::cout << "[*] COMPILING : " << argv[1] << std::endl;
    std::ifstream sourceFileStream (argv[1]);

    std::stringstream buffer;

    char temp;

    while(sourceFileStream.get(temp))
    {
        buffer << temp;
    }

    std::string sourceCode = buffer.str();
    sourceCode.append("\0");

    
    Lexer lexer(sourceCode);
    std::vector <Token *> tokens = lexer.tokenize();
    int counter = 0;
    if (tokens.back()->TYPE != TOKEN_EOF)
    {
        Token * EOFNode = new Token();
        EOFNode->TYPE = TOKEN_EOF;
        tokens.push_back(EOFNode);
    }

    // for (Token * temp : tokens)
    // {
    //     counter++;
    // 	    std::cout << counter << ") " << temp->VALUE << " " << typeToString(temp->TYPE) << std::endl;
    // }
    // exit(0);-r

   
    
    std::string filenameWithoutExtension = argv[1];
    filenameWithoutExtension.pop_back();
    filenameWithoutExtension.pop_back();
    filenameWithoutExtension.pop_back();
    
    std::cout << "[!] COMPILATION SUCCESSFUL " << std::endl;
    std::cout << "[+] FILE READY : ./" << filenameWithoutExtension << std::endl; 

    std::stringstream assemblerInstruction;
    assemblerInstruction << "nasm -f elf64 " + filenameWithoutExtension + ".le.asm";
    
    system(assemblerInstruction.str().c_str());

    std::stringstream linkerInstruction;
    linkerInstruction << "ld -o " + filenameWithoutExtension + " " + filenameWithoutExtension + ".le.o";
    
    system(linkerInstruction.str().c_str());

    return 0;
}
/*here we are taking argc and argv as they are the command line arguments which are passed to programs which are executed through command line or terminal 
argc is the no of command line argument and argv is a array of strings conataining all the real argument even containing the file name.
so basically i have used ifstream to open a file and then to read the file into a buffer by using a object of stringstream which is buffer it self in the program.
After reading the file we converted it into a string called as a source code .*/