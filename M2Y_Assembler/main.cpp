/*
    M2Y-ISA Assembler developed by Emre Duman
    2023

    It can be used freely
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <bitset>
#include <iomanip>
#include "assembler.h"
#pragma warning(disable : 4996) //disabled strcpy error for next line

using namespace std;


int main()
{
    /*
    string inputFilename;
    cout << "Please enter the input file name for the assembly:" << endl;
    cin >> inputFilename;
    string outputFilename;
    cout << "Please enter the output file name for binary file:" << endl;
    cin >> inputFilename;
    */
    string inputFilename = "Assembly";
    string outputFilename;
    outputFilename = inputFilename + "_out";
    //removeSpacesFromFile(inputFilename, outputFilename); // TESTED ! OK!
    //removeCommentsFromFile(inputFilename, outputFilename); // TESTED ! OK!
    //findExactWord(inputFilename, outputFilename); // removingComments is at the testing stage !
    //find_directives_data(inputFilename, outputFilename); // TESTED ! OK!
    //


    do_assembly(inputFilename, outputFilename);
    cout << "Success !" << endl;
    cout << inputFilename << " file is successfully converted to object file as " << outputFilename << "!" << endl;



   /*
   for (uint32_t i = 0; i < datasection_index; i++) {
        cout << datasection[i] << endl;
    }
    for (uint32_t i = 0; i < labelindex; i++) {
        cout << label_list[i].name << "-> Address:" << label_list[i].address << endl;
    }
    */
    
}
